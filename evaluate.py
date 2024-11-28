import threading
import subprocess
import time
import psutil
import os
import struct
import shutil
from pathlib import Path
import re
import csv
cobra_log_dir = "./data/cobra/"
be_log_dir = "./data/be/"
coo_cases_dir = "./data/coo/"
coo_result_path = "./result/coo_result.txt"
data_root = Path("./data/")
TIMEOUT = 600


def get_tc_construct_time(info):
    match = re.search(r'initializeTransitiveClosure took (\d+) ms', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_solve_time(info):
    match = re.search(r'solveConstraints took (\d+) us', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_total_constraints(info):
    match = re.search(r'total constraints (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_gen_constraints(info):
    match = re.search(r'generate constraints (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_merge_constraints(info):
    match = re.search(r'constraints after merge (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_prune_constraints(info):
    match = re.search(r'constraints after prune (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_prune_time(info):
    match = re.search(r'pruneConstraints took (\d+) us', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_cobra_runtime(info):
    match = re.search(r'Overall runtime = (\d+)ms', info)
    if match:
        time_taken = float(match.group(1))/1000
        return time_taken
    else:
        return TIMEOUT


def get_cobra_prune_after(info):
    match = re.search(r'After PRUNE #constraint\[2\] = (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_cobra_prune_before(info):
    match = re.search(r'Before PRUNE #constraint\[1\] = (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def clear_path(directory_path):
    if os.path.exists(directory_path):
        shutil.rmtree(directory_path)
    os.makedirs(directory_path, exist_ok=True)


def transform_to_cobra(log_dir, cobra_log_dir):
    INIT_WRITE_ID = 0xbebeebee
    INIT_TXN_ID = 0xbebeebee
    for filename in os.listdir(log_dir):
        if filename.endswith(".log"):
            input_file_path = os.path.join(log_dir, filename)
            output_file_path = os.path.join(cobra_log_dir, "T"+filename)

            with open(input_file_path, 'rb') as in_file, open(output_file_path, 'wb') as out_file:
                buffer = []
                commit_detected = False
                while True:
                    op_type = in_file.read(1)
                    if not op_type:
                        break

                    if op_type == b'T':  # Transaction Start
                        tid = struct.unpack('I', in_file.read(4))[0]
                        in_file.read(8)  # start
                        in_file.read(8)  # end

                    elif op_type == b'S':
                        in_file.read(4)  # oid
                        in_file.read(8)  # start
                        in_file.read(8)  # end
                        buffer.append((b'S', tid))

                    elif op_type == b'C':  # Commit
                        in_file.read(4)  # oid
                        in_file.read(8)  # start
                        in_file.read(8)  # end
                        buffer.append((b'C', tid))
                        commit_detected = True

                    elif op_type == b'W':  # Write
                        oid = struct.unpack('I', in_file.read(4))[0]
                        in_file.read(8)  # start
                        in_file.read(8)  # end
                        key = struct.unpack('>Q', in_file.read(8))[0]
                        buffer.append((b'W', oid, key))

                    elif op_type == b'R':  # Read
                        oid = struct.unpack('I', in_file.read(4))[0]
                        in_file.read(8)  # start
                        in_file.read(8)  # end
                        key = struct.unpack('>Q', in_file.read(8))[0]
                        from_tid = struct.unpack('I', in_file.read(4))[0]
                        from_oid = struct.unpack('I', in_file.read(4))[0]

                        if from_oid == 0:
                            from_oid = INIT_WRITE_ID
                            from_tid = INIT_TXN_ID

                        buffer.append((b'R', from_tid, from_oid, key))

                    elif op_type == b'A':  # Abort
                        in_file.read(4)  # oid
                        in_file.read(8)  # start
                        in_file.read(8)  # end
                        buffer = []  # Clear the buffer on Abort
                        commit_detected = False
                    else:
                        print("error")

                    if commit_detected:
                        assert buffer[-1][0] == b'C'
                        for op in buffer:
                            if op[0] == b'S':
                                out_file.write(b'S')
                                out_file.write(struct.pack('>Q', op[1]))
                            elif op[0] == b'C':
                                out_file.write(b'C')
                                out_file.write(struct.pack('>Q', op[1]))
                            elif op[0] == b'W':
                                out_file.write(b'W')
                                out_file.write(struct.pack('>Q', op[1]))
                                out_file.write(struct.pack('>Q', op[2]))
                                out_file.write(struct.pack('>Q', op[2]))
                            elif op[0] == b'R':
                                out_file.write(b'R')
                                out_file.write(struct.pack('>Q', op[1]))
                                out_file.write(struct.pack('>Q', op[2]))
                                out_file.write(struct.pack('>Q', op[3]))
                                out_file.write(struct.pack('>Q', op[3]))
                        buffer = []
                        commit_detected = False


def transform_to_be(cobra_log_dir, be_log_dir):
    process = subprocess.Popen([f"./src/BEtransform/target/debug/translator", cobra_log_dir, be_log_dir],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               text=True)


def transform(log_dir):
    clear_path(cobra_log_dir)
    clear_path(be_log_dir)
    transform_to_cobra(log_dir, cobra_log_dir)
    transform_to_be(cobra_log_dir, be_log_dir)


def kill_java_processes():
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] == 'java':
            print(f"Killing Java process with PID: {proc.info['pid']}")
            proc.kill()  # Forcefully kill the java process


def monitor_java_memory(memory_usage):
    time.sleep(5)
    try:
        while True:
            # 使用 ps 命令获取 Java 进程的内存使用情况
            result = subprocess.run(
                ["bash", "-c", "ps -eo pid,comm,%mem,%cpu,vsz,rss | grep java"],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )

            lines = result.stdout.splitlines()
            # print(lines)

            if not lines:  # 如果没有检测到 Java 进程，停止监控
                print("No Java processes found. Stopping monitoring.")
                break

            for line in lines:
                if "java" in line:
                    parts = line.split()
                    pid = parts[0]
                    comm = parts[1]
                    rss_kb = int(parts[-1])
                    rss_mb = rss_kb / 1024

                    memory_usage[0] = max(memory_usage[0], rss_mb)
            # print(memory_usage[0])
            time.sleep(0.1)  # 每 0.1 秒检查一次内存使用情况
    except Exception as e:
        print(f"Error: {e}")


def monitor_memory(process, memory_usage):
    try:
        ps_process = psutil.Process(process.pid)
        while process.poll() is None:
            memory_usage[0] = max(memory_usage[0], ps_process.memory_info().rss / 1024 / 1024)
            #memory_usage[1] = max(memory_usage[1], ps_process.memory_info().vms / 1024 / 1024)
            time.sleep(0.1)
    except psutil.NoSuchProcess:
        pass


def run_vbox(log_dir,
                 verifier="offline",
                 timing_enabled="true",
                 compaction_enabled="true",
                 merging_enabled="true",
                 pruning_strategy="pruneOptimize",
                 tc_construction="purdomOptimize",
                 tc_update="italinoOptimize",
                 sat_strategy="graphsat",
                 ):

    process = subprocess.Popen([f"./build/SerVerifier",
                                log_dir, verifier, timing_enabled, compaction_enabled, merging_enabled, pruning_strategy, tc_construction, tc_update, sat_strategy],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               text=True)

    try:
        memory_usage = [0, 0]
        memory_monitor_thread = threading.Thread(target=monitor_memory, args=(process, memory_usage))
        memory_monitor_thread.start()
        start_time = time.time()
        try:
            stdout, stderr = process.communicate(timeout=TIMEOUT)
        except subprocess.TimeoutExpired:

            process.terminate()
            memory_monitor_thread.join()
            run_time = TIMEOUT
            return False, run_time, memory_usage[0] + memory_usage[1], "Process terminated due to timeout."
        end_time = time.time()
        run_time = end_time - start_time
        accept = "Accept: 1" in stdout
        memory_monitor_thread.join()
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout
    except psutil.NoSuchProcess:
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout
    except Exception as e:
        print(f"Error: {e}")
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout


def run_leopard(log_dir):
    process = subprocess.Popen([f"./build/SerVerifier", log_dir, "leopard"],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               text=True)
    try:
        memory_usage = [0, 0]
        memory_monitor_thread = threading.Thread(target=monitor_memory, args=(process, memory_usage))
        memory_monitor_thread.start()
        start_time = time.time()
        try:
            stdout, stderr = process.communicate(timeout=TIMEOUT)
        except subprocess.TimeoutExpired:
            process.terminate()
            memory_monitor_thread.join()
            run_time = TIMEOUT
            return False, run_time, memory_usage[0] + memory_usage[1], "Process terminated due to timeout."
        end_time = time.time()
        run_time = end_time - start_time
        accept = "Accept: 1" in stdout
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout
    except psutil.NoSuchProcess:
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout
    except Exception as e:
        print(f"Error: {e}")
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout



def run_cobra_gpu():
    print("Running Cobra GPU Verifier...")
    original_dir = os.getcwd()  # Save the current directory to restore it later
    try:
        # Setting up environment by sourcing env.sh and capturing the environment variables
        command = "source baselines/CobraHome/env.sh && env"
        proc = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True, executable='/bin/bash')
        output, _ = proc.communicate()

        env = os.environ.copy()
        for line in output.decode().splitlines():
            key, value = line.split('=', 1)
            env[key] = value

        cobra_verifier_dir = "./baselines/CobraHome/CobraVerifier"
        cobra_log_dir_abs = os.path.abspath(cobra_log_dir)
        os.chdir(cobra_verifier_dir)

        # Start the Cobra process
        process = subprocess.Popen([
            "./run.sh",
            "mono",
            "audit",
            "./cobra.conf.default",
            cobra_log_dir_abs
        ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            env=env)

        memory_usage = [0]
        memory_monitor_thread = threading.Thread(target=monitor_java_memory, args=(memory_usage,), daemon=True)
        memory_monitor_thread.start()  # Start the memory monitor in the background

        start_time = time.time()

        try:
            # Try to communicate with the Cobra process, with a timeout
            stdout, stderr = process.communicate(timeout=TIMEOUT)
            run_time = get_cobra_runtime(stdout)  # You defined get_cobra_runtime
            accept = "ACCEPT" in stdout
            return accept, run_time, memory_usage[0], stdout

        except subprocess.TimeoutExpired:
            print("Timeout occurred, terminating process...")
            process.terminate()  # Gracefully terminate the process
            try:
                process.wait(timeout=3)  # Wait for it to terminate gracefully
            except subprocess.TimeoutExpired:
                print("Process did not terminate, forcing kill...")
                process.kill()  # Forcefully kill the process if it doesn't terminate
            kill_java_processes()
            run_time = TIMEOUT
            return False, run_time, memory_usage[0], "Process terminated due to timeout."

        except psutil.NoSuchProcess:
            return False, 0, memory_usage[0], "Process no longer exists."

        except Exception as e:
            print(f"Error: {e}")
            return False, 0, memory_usage[0], str(e)

    finally:
        # Make sure to wait for the memory monitoring thread to terminate
        memory_monitor_thread.join(timeout=1)  # Ensure the thread is stopped
        os.chdir(original_dir)  # Restore the original directory


def run_BE():
    command = [
        "dbcop",
        "verify",
        "--cons", "ser",
        "--out_dir", be_log_dir,
        "--ver_dir", be_log_dir
    ]

    process = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    try:
        memory_usage = [0, 0]
        memory_monitor_thread = threading.Thread(target=monitor_memory, args=(process, memory_usage))
        memory_monitor_thread.start()
        start_time = time.time()
        try:
            stdout, stderr = process.communicate(timeout=TIMEOUT)
        except subprocess.TimeoutExpired:
            process.terminate()
            memory_monitor_thread.join()
            run_time = TIMEOUT
            return False, run_time, memory_usage[0] + memory_usage[1], "Process terminated due to timeout."
        end_time = time.time()
        run_time = end_time - start_time
        accept = "failed" not in stdout
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout
    except psutil.NoSuchProcess:
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout
    except Exception as e:
        print(f"Error: {e}")
        return accept, run_time, memory_usage[0] + memory_usage[1], stdout


def eval_completeness_coo():
    print("Completeness Experiment.")
    result = [[0 for _ in range(4)] for _ in range(3)]  # RAT,WAT,IAT
    types = ["rat", "wat", "iat"]
    with open(coo_result_path, "w") as f:
        column_width = 20
        f.write(f"{'Coo-case'.ljust(40)}{'Verifier'.ljust(column_width)}{'Cobra'.ljust(column_width)}"
                f"{'Leopard'.ljust(column_width)}{'BE'.ljust(column_width)}\n")

        for root, dirs, _ in os.walk(coo_cases_dir):
            for dir in dirs:
                print(f"Case:{dir}")
                case_type = dir[0:3]
                case_dir = os.path.join(root, dir)
                transform(case_dir)
                # 1. verifier
                # purdomOptimize
                accept_verifier, _, _, stdout = run_vbox(
                    log_dir=case_dir, verifier="online")
                # print(stdout)
                result[types.index(case_type)][0] += not accept_verifier
                # 2. Cobra
                accept_cobra, _, _, stdout = run_cobra_gpu()
                # print(stdout)
                accept_cobra = True
                result[types.index(case_type)][1] += not accept_cobra
                # 3. Leopard
                accept_leopard, _, _, _ = run_leopard(log_dir=case_dir)
                result[types.index(case_type)][2] += not accept_leopard
                # 4. BE
                accept_be, _, _, _ = run_BE()
                accept_be = True
                result[types.index(case_type)][3] += not accept_be
                # 写入每行的结果，并对齐每列
                f.write(f"{dir.ljust(40)}{str(accept_verifier).ljust(column_width)}"
                        f"{str(accept_cobra).ljust(column_width)}{str(accept_leopard).ljust(column_width)}"
                        f"{str(accept_be).ljust(column_width)}\n")
        f.write('-'*100+"\n")
        for i in range(3):
            t = types[i]
            r = result[i]
            f.write(f"{str(t).ljust(40)}{str(r[0]).ljust(column_width)}"
                    f"{str(r[1]).ljust(column_width)}{str(r[2]).ljust(column_width)}"
                    f"{str(r[3]).ljust(column_width)}\n")


def eval_base_efficiency(num_trxs=10000):
    benchmarks = [
        f"blind_20_80_{num_trxs}_25_3",
        f"blind_50_50_{num_trxs}_25_3",
        f"blind_80_20_{num_trxs}_25_3",
        f"ctwitter_{num_trxs}_25_3",
        f"tpcc_{num_trxs}"
    ]
    base_efficiency_result_path = f"./result/base_efficiency_{num_trxs}.txt"
    column_width = 40
    result_overload = [[] for _ in range(len(benchmarks))]
    result_time = [[]for _ in range(len(benchmarks))]
    total_cons = [0 for _ in range(len(benchmarks))]
    gen_cons = [0 for _ in range(len(benchmarks))]
    merge_cons = [0 for _ in range(len(benchmarks))]
    prune_cons = [0 for _ in range(len(benchmarks))]
    cobra_cons_before = [0 for _ in range(len(benchmarks))]
    cobra_cons_after = [0 for _ in range(len(benchmarks))]
    with open(base_efficiency_result_path, "w") as f:
        f.write(f"{'Method'.ljust(40)}")
        for benchmark in benchmarks:
            f.write(f"{benchmark.ljust(column_width)}")
        f.write("\n")
        for i in range(len(benchmarks)):
            benchmark = benchmarks[i]
            case_dir = data_root/benchmark
            print(f"Case:{benchmark}")
            transform(case_dir)
            accept, time, overload, stdout = run_vbox(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)
            total_cons[i] = get_total_constraints(stdout)
            gen_cons[i] = get_gen_constraints(stdout)
            merge_cons[i] = get_merge_constraints(stdout)
            prune_cons[i] = get_prune_constraints(stdout)

            accept, time, overload, stdout = run_vbox(log_dir=case_dir, verifier="online")
            result_time[i].append(time)
            result_overload[i].append(overload)

            # 2. Cobra
            # accept, time, overload, stdout = run_cobra_gpu()
            

            # time_cobra = 1
            # overload_cobra = 1

            result_overload[i].append(overload)
            cobra_cons_before[i] = get_cobra_prune_before(stdout)
            cobra_cons_after[i] = get_cobra_prune_after(stdout)
            if accept == False:
                result_time[i].append(-1)  # error,because n*n out range of java max array size
                print(stdout)
            else:
                result_time[i].append(time)
            # 3. Leopard
            accept, time, overload, stdout = run_leopard(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)
            assert accept

            # 4. BE
            # accept, time, overload, stdout = run_BE()
            # time_be = TIMEOUT
            # overload_be = 1
            result_time[i].append(time)
            result_overload[i].append(overload)
        methods = ["Verifier-Offline", "Verifier-Online", "Cobra", "Leopard", "BE"]
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_time[i][j])} s".ljust(column_width))
            f.write("\n")

        f.write("\n" + "Overload Results".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_overload[i][j])} mb".ljust(column_width))
            f.write("\n")
        f.write("\n" + "Prune Info".ljust(40) + "\n")
        f.write("Total Cons".ljust(40))
        for i in range(len(benchmarks)):
            f.write(f"{str(total_cons[i])}".ljust(column_width))
        f.write("\n")
        f.write("Gen Cons".ljust(40))
        for i in range(len(benchmarks)):
            f.write(f"{str(gen_cons[i])}".ljust(column_width))
        f.write("\n")
        f.write("Merge Cons".ljust(40))
        for i in range(len(benchmarks)):
            f.write(f"{str(merge_cons[i])}".ljust(column_width))
        f.write("\n")
        f.write("Prune Cons".ljust(40))
        for i in range(len(benchmarks)):
            f.write(f"{str(prune_cons[i])}".ljust(column_width))
        f.write("\n")
        f.write("Cobra Cons Before Prune".ljust(40))
        for i in range(len(benchmarks)):
            f.write(f"{str(cobra_cons_before[i])}".ljust(column_width))
        f.write("\n")
        f.write("Cobra Cons After Prune".ljust(40))
        for i in range(len(benchmarks)):
            f.write(f"{str(cobra_cons_after[i])}".ljust(column_width))
        f.write("\n")


def eval_base_scalability(workload="blind_50_50"):
    benchmarks = [f"{workload}_{(i+1)*10000}_25_3" for i in range(10)]
    base_efficiency_result_path = f"./result/base_scalability_{workload}.txt"
    column_width = 40
    result_overload = [[] for _ in range(len(benchmarks))]
    result_time = [[]for _ in range(len(benchmarks))]
    with open(base_efficiency_result_path, "w") as f:
        f.write(f"{'Method'.ljust(40)}")
        for benchmark in benchmarks:
            f.write(f"{benchmark.ljust(column_width)}")
        f.write("\n")
        for i in range(len(benchmarks)):
            benchmark = benchmarks[i]
            case_dir = data_root/benchmark
            print(f"Case:{benchmark}")
            transform(case_dir)
            # 1. verifier
            accept, time, overload, stdout = run_vbox(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)

            accept, time, overload, stdout = run_vbox(log_dir=case_dir, verifier="online")
            result_time[i].append(time)
            result_overload[i].append(overload)

            # 2. Cobra
            accept, time, overload, stdout = run_cobra_gpu()
            # time_cobra = 1
            # overload_cobra = 1
            result_overload[i].append(overload)
            if accept == False:
                result_time[i].append(TIMEOUT)
            else:
                result_time[i].append(time)
            # 3. Leopard
            accept, time, overload, stdout = run_leopard(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)
            # 4. BE
            accept, time, overload, stdout = run_BE()
            # time_be = 1
            # overload_be = 1
            result_time[i].append(time)
            result_overload[i].append(overload)

        methods = ["Verifier-Offline", "Verifier-Online", "Cobra", "Leopard", "BE"]
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_time[i][j])} s".ljust(column_width))
            f.write("\n")

        f.write("\n" + "Overload Results".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_overload[i][j])} mb".ljust(column_width))
            f.write("\n")


def eval_txn_length(workload="blind_50_50_100000"):
    benchmarks = [f"{workload}_25_3_{8+(4*i)}" for i in range(10)]
    txn_len_result_path = f"./result/txn_len_{workload}.txt"
    column_width = 40
    result_overload = [[] for _ in range(len(benchmarks))]
    result_time = [[]for _ in range(len(benchmarks))]
    with open(txn_len_result_path, "w") as f:
        f.write(f"{'Method'.ljust(40)}")
        for benchmark in benchmarks:
            f.write(f"{benchmark.ljust(column_width)}")
        f.write("\n")
        for i in range(len(benchmarks)):
            benchmark = benchmarks[i]
            case_dir = data_root/benchmark
            print(f"Case:{benchmark}")
            transform(case_dir)
            # 1. verifier
            accept, time, overload, stdout = run_vbox(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)
            assert accept

            accept, time, overload, stdout = run_vbox(log_dir=case_dir, verifier="online")
            result_time[i].append(time)
            result_overload[i].append(overload)
            assert accept

            # 2. Cobra
            # accept, time, overload, stdout = run_cobra_gpu()
            # time_cobra = 1
            # overload_cobra = 1
            result_time[i].append(time)
            result_overload[i].append(overload)

            # 3. Leopard
            accept, time, overload, stdout = run_leopard(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)
            assert accept

            # 4. BE
            # accept, time, overload, stdout = run_BE()
            # time_be = 1
            # overload_be = 1
            result_time[i].append(time)
            result_overload[i].append(overload)

        methods = ["Verifier-Offline", "Verifier-Online", "Cobra", "Leopard", "BE"]
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_time[i][j])} s".ljust(column_width))
            f.write("\n")

        f.write("\n" + "Overload Results".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_overload[i][j])} mb".ljust(column_width))
            f.write("\n")


def eval_client_num(workload="blind_50_50_100000"):
    benchmarks = [f"{workload}_{25+(5*i)}_3_8" for i in range(10)]
    client_num_result_path = f"./result/client_num_{workload}.txt"
    column_width = 40
    result_overload = [[] for _ in range(len(benchmarks))]
    result_time = [[]for _ in range(len(benchmarks))]
    with open(client_num_result_path, "w") as f:
        f.write(f"{'Method'.ljust(40)}")
        for benchmark in benchmarks:
            f.write(f"{benchmark.ljust(column_width)}")
        f.write("\n")
        for i in range(len(benchmarks)):
            benchmark = benchmarks[i]
            case_dir = data_root/benchmark
            print(f"Case:{benchmark}")
            transform(case_dir)
            # 1. verifier
            accept, time, overload, stdout = run_vbox(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)
            assert accept

            accept, time, overload, stdout = run_vbox(log_dir=case_dir, verifier="online")
            result_time[i].append(time)
            result_overload[i].append(overload)
            assert accept
            # 2. Cobra
            # accept, time, overload, stdout = run_cobra_gpu()
            # time_cobra = 1
            # overload_cobra = 1
            result_time[i].append(time)
            result_overload[i].append(overload)

            # 3. Leopard
            accept, time, overload, stdout = run_leopard(log_dir=case_dir)
            result_time[i].append(time)
            result_overload[i].append(overload)
            if not accept:
                print(stdout)
                

            # 4. BE
            # accept, time, overload, stdout = run_BE()
            # time_be = 1
            # overload_be = 1
            result_time[i].append(time)
            result_overload[i].append(overload)

        methods = ["Verifier-Offline", "Verifier-Online", "Cobra", "Leopard", "BE"]
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_time[i][j])} s".ljust(column_width))
            f.write("\n")

        f.write("\n" + "Overload Results".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_overload[i][j])} mb".ljust(column_width))
            f.write("\n")


def eval_compaction(num_trxs=100000):
    benchmarks = [f"blind_20_80_{num_trxs}_25_3",
                  f"blind_50_50_{num_trxs}_25_3",
                  f"blind_80_20_{num_trxs}_25_3",
                  f"ctwitter_{num_trxs}_25_3",
                  f"tpcc_{num_trxs}"
                  ]
    compaction_result_path = f"./result/compaction_{num_trxs}.txt"
    column_width = 40
    result_overload = [[] for _ in range(len(benchmarks))]
    result_time = [[]for _ in range(len(benchmarks))]
    with open(compaction_result_path, "w") as f:
        f.write(f"{'Method'.ljust(40)}")
        for benchmark in benchmarks:
            f.write(f"{benchmark.ljust(column_width)}")
        f.write("\n")
        for i in range(len(benchmarks)):
            benchmark = benchmarks[i]
            case_dir = data_root/benchmark
            print(f"Case:{benchmark}")
            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", compaction_enabled="true")
            result_time[i].append(time)
            result_overload[i].append(overload)
            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", compaction_enabled="false")
            result_time[i].append(time)
            result_overload[i].append(overload)
            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="online", compaction_enabled="true")
            result_time[i].append(time)
            result_overload[i].append(overload)
            # accept, time, overload, stdout = run_vbox(
            #     log_dir=case_dir, verifier="online", compaction_enabled="false")
            # result_time[i].append(time)
            # result_overload[i].append(overload)

        methods = ["Verifier-Offline-Compaction",
                   "Verifier-Offline-NoCompaction",
                   "Verifier-Online-Compaction",
                   #    "Verifier-Online-NoCompaction"
                   ]
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_time[i][j]).ljust(column_width)}")
            f.write("\n")

        f.write("\n" + "Overload Results".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_overload[i][j]).ljust(column_width)}")
            f.write("\n")


def eval_construct_transitive_closure(num_trxs=100000):
    benchmarks = [f"blind_20_80_{num_trxs}_25_3",
                  f"blind_50_50_{num_trxs}_25_3",
                  f"blind_80_20_{num_trxs}_25_3",
                  f"ctwitter_{num_trxs}_25_3",
                  f"tpcc_{num_trxs}"
                  ]
    construct_result_path = f"./result/construct_{num_trxs}.txt"
    column_width = 40
    result_overload = [[] for _ in range(len(benchmarks))]
    result_time = [[]for _ in range(len(benchmarks))]
    construct_time = [[]for _ in range(len(benchmarks))]
    with open(construct_result_path, "w") as f:
        f.write(f"{'Method'.ljust(40)}")
        for benchmark in benchmarks:
            f.write(f"{benchmark.ljust(column_width)}")
        f.write("\n")
        for i in range(len(benchmarks)):
            benchmark = benchmarks[i]
            case_dir = data_root/benchmark
            print(f"Case:{benchmark}")

            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_construction="purdomOptimize")
            result_time[i].append(time)
            result_overload[i].append(overload)
            construct_time[i].append(get_tc_construct_time(stdout))

            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_construction="purdom")
            result_time[i].append(time)
            result_overload[i].append(overload)
            construct_time[i].append(get_tc_construct_time(stdout))

            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_construction="warshall")
            # time = 0
            # overload = 0
            result_time[i].append(time)
            result_overload[i].append(overload)
            construct_time[i].append(get_tc_construct_time(stdout))

            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_construction="italino", tc_update="italino")
            result_time[i].append(time)
            result_overload[i].append(overload)
            construct_time[i].append(get_tc_construct_time(stdout))

            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_construction="italino", tc_update="italinoOptimize")
            result_time[i].append(time)
            result_overload[i].append(overload)
            construct_time[i].append(get_tc_construct_time(stdout))

        methods = ["purdomOptimize", "purdom",
                   "warshall", "italino", "italinoOptimize"]

        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_time[i][j])} s".ljust(column_width))
            f.write("\n")

        f.write("\n" + "Overload Results".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_overload[i][j])} mb".ljust(column_width))
            f.write("\n")

        f.write("\n" + "Construct Time".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(construct_time[i][j])} ms".ljust(column_width))
            f.write("\n")


def eval_update_transotive_clouse(num_trxs=100000):
    benchmarks = [f"blind_20_80_{num_trxs}_25_3",
                  f"blind_50_50_{num_trxs}_25_3",
                  f"blind_80_20_{num_trxs}_25_3",
                  f"ctwitter_{num_trxs}_25_3",
                  f"tpcc_{num_trxs}"
                  ]
    update_result_path = f"./result/update_{num_trxs}.txt"
    column_width = 40
    result_overload = [[] for _ in range(len(benchmarks))]
    result_time = [[]for _ in range(len(benchmarks))]
    with open(update_result_path, "w") as f:
        f.write(f"{'Method'.ljust(40)}")
        for benchmark in benchmarks:
            f.write(f"{benchmark.ljust(column_width)}")
        f.write("\n")
        for i in range(len(benchmarks)):
            benchmark = benchmarks[i]
            case_dir = data_root/benchmark
            print(f"Case:{benchmark}")
            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_update="warshall")
            # time = 0
            # overload = 0
            result_time[i].append(time)
            result_overload[i].append(overload)

            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_update="italino")
            result_time[i].append(time)
            result_overload[i].append(overload)

            accept, time, overload, stdout = run_vbox(
                log_dir=case_dir, verifier="offline", tc_update="italinoOptimize")
            result_time[i].append(time)
            result_overload[i].append(overload)

        methods = ["warshall", "italino", "italinoOptimize"]

        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_time[i][j])} s".ljust(column_width))
            f.write("\n")

        f.write("\n" + "Overload Results".ljust(40) + "\n")
        for j, method in enumerate(methods):
            f.write(f"{method.ljust(40)}")
            for i in range(len(benchmarks)):
                f.write(f"{str(result_overload[i][j])} mb".ljust(column_width))
            f.write("\n")



def eval_solve_time(num_trxs=10000):
    benchmarks = [
        f"blindw_wh_{num_trxs}",
        f"blindw_wr_{num_trxs}",
        f"blindw_rh_{num_trxs}",
        f"ctwitter_{num_trxs}",
    ]
    solver_result_path = f"./result/solve_time_{num_trxs}.csv"
    solve_time = [[] for _ in range(len(benchmarks))]
    
    with open(solver_result_path, mode="w", newline="") as csvfile:
        csv_writer = csv.writer(csvfile)
        
        headers = ["Method"] + benchmarks
        csv_writer.writerow(headers)
        
        for i, benchmark in enumerate(benchmarks):
            case_dir = data_root / benchmark
            transform(case_dir)
            
            _, _, _, stdout = run_vbox(log_dir=case_dir, sat="vboxsat")
            solve_time[i].append(get_solve_time(stdout))
            
            _, _, _, stdout = run_vbox(log_dir=case_dir, sat="monosat")
            solve_time[i].append(get_solve_time(stdout))
            
            _, _, _, stdout = run_vbox(log_dir=case_dir, sat="minisat")
            solve_time[i].append(get_solve_time(stdout))
        
        methods = ["vboxsat", "monosat", "minisat"]
        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(benchmarks)):
                row.append(f"{solve_time[i][j]} us")
            csv_writer.writerow(row)

# eval_completeness_real()
# eval_completeness_coo()
# eval_update_clouse(10000)
# eval_construct_closure(10000)
# eval_efficiency(10000)
# eval_scalability()
# eval_solve_time(10000)


