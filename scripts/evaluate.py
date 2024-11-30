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
import argparse

coo_cases_dir = "./data/coo/"
coo_result_path = "./output/raw/coo_result.txt"

real_word_cases_dir = "./data/ser_violation/"
real_word_result_path = "./output/raw/real_word_result.txt"

ser_violation_root = Path("./data/ser_violation/")

cobra_log_root = "./data/cobra_data/"
be_log_root = "./data/data/"
vbox_data_root = Path("./data/vbox_data/")
raw_root = Path("./output/raw/")
plot_root = Path("./output/plots/")

TIMEOUT = 600


def get_closure_construct_time(info):
    match = re.search(r'Closure Construction Time Taken: (\d+) us', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_solve_time(info):
    match = re.search(r'Constraint Solving Time Taken: (\d+) us', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_total_constraints(info):
    match = re.search(r'Total Item Constriants      (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0


def get_vbox_pruned_constraint(info):
    match = re.search(r'Pruned Item Constraints:  (\d+)', info)
    if match:
        time_taken = int(match.group(1))
        return time_taken
    else:
        return 0
    
def get_cobra_pruned_constraint(info):
    match = re.search(r'After PRUNE #constraint\[2\] = (\d+)', info)
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
                        in_file.read(8)
                        in_file.read(4)
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
                    elif op_type == b'P':
                        oid = struct.unpack('I', in_file.read(4))[0]
                        in_file.read(8)
                        in_file.read(8)
                        in_file.read(8)
                        in_file.read(4)
                        in_file.read(4)
                        size = struct.unpack('I', in_file.read(4))[0]
                        for i in range(size):{
                            in_file.read(8)
                        }
                        for i in range(size):{
                            in_file.read(4)
                        }
                        for i in range(size):{
                            in_file.read(4)
                        }
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

def transform_to_vbox(cobra_log_dir,log_dir):
    for filename in os.listdir(cobra_log_dir):
        if filename.endswith(".log"):
            print(filename)
            input_file_path = os.path.join(cobra_log_dir, filename)
            output_file_path = os.path.join(log_dir, filename)

            with open(input_file_path, 'rb') as in_file, open(output_file_path, 'wb') as out_file:
                buffer = []
                commit_detected = False
                while True:
                    op_type = in_file.read(1)
                    if not op_type:
                        break
                    if op_type == b'S':
                        tid = struct.unpack('>Q', in_file.read(8))[0]
                        buffer.append((b'S', tid))

                    elif op_type == b'C':  # Commit
                        tid = struct.unpack('>Q', in_file.read(8))[0]
                        buffer.append((b'C', tid))
                        commit_detected = True

                    elif op_type == b'W':  # Write
                        wid = struct.unpack('>Q', in_file.read(8))[0]
                        assert wid !=0xbebeebee
                        key_hash = struct.unpack('>Q', in_file.read(8))[0]
                        val_hash = struct.unpack('>Q', in_file.read(8))[0]
                        buffer.append((b'W', wid, key_hash, val_hash))

                    elif op_type == b'R':  # Read
                        prev = struct.unpack('>Q', in_file.read(8))[0]
                        if prev == 0xbebeebee:
                            prev = 0
                        wid = struct.unpack('>Q', in_file.read(8))[0]
                        if wid == 0xbebeebee:
                            wid = 0
                        key_hash = struct.unpack('>Q', in_file.read(8))[0]
                        val_hash = struct.unpack('>Q', in_file.read(8))[0]

                        buffer.append((b'R', prev, wid, key_hash, val_hash))
                    else:
                        print("error")

                    if commit_detected:
                        assert buffer[-1][0] == b'C'
                        for op in buffer:
                            if op[0] == b'S':
                                out_file.write(b'T')
                                out_file.write(struct.pack('<I', op[1]))  # tid
                                out_file.write(struct.pack('<Q', 0))  # start
                                out_file.write(struct.pack('<Q', 9999999))  # end

                                out_file.write(b'S')
                                out_file.write(struct.pack('<I', 0))  # oid
                                out_file.write(struct.pack('<Q', 0))  # start
                                out_file.write(struct.pack('<Q', 9999999))  # end

                            elif op[0] == b'C':
                                out_file.write(b'C')
                                out_file.write(struct.pack('<I', 0))
                                out_file.write(struct.pack('<Q', 0))  # start
                                out_file.write(struct.pack('<Q', 9999999))  # end

                            elif op[0] == b'W':
                                out_file.write(b'W')
                                out_file.write(struct.pack('<I', op[1]))  # oid
                                out_file.write(struct.pack('<Q', 0))  # start
                                out_file.write(struct.pack('<Q', 9999999))  # end
                                out_file.write(struct.pack('<Q', op[2]))  # key
                                out_file.write(struct.pack('<Q', 1))
                                out_file.write(struct.pack('<I', 1))
                            elif op[0] == b'R':
                                out_file.write(b'R')
                                out_file.write(struct.pack('<I', 1))  # oid
                                out_file.write(struct.pack('<Q', 0))  # start
                                out_file.write(struct.pack('<Q', 9999999))  # end
                                out_file.write(struct.pack('<Q', op[3])) #key    
                                out_file.write(struct.pack('<I', op[1]))
                                out_file.write(struct.pack('<I', op[2]))
                                
                        buffer = []
                        commit_detected = False

def transform_to_be(cobra_log_dir, be_log_dir):
    process = subprocess.Popen([f"./src/transform/target/debug/translator", cobra_log_dir, be_log_dir],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               universal_newlines=True)



def transform(log_dir):
    clear_path(cobra_log_root)
    clear_path(be_log_root)
    transform_to_cobra(log_dir, cobra_log_root)
    transform_to_be(cobra_log_root, be_log_root)


def kill_java_processes():
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] == 'java':
            print(f"Killing Java process with PID: {proc.info['pid']}")
            proc.kill()  # Forcefully kill the java process


def monitor_java_memory(memory_usage):
    time.sleep(5)
    try:
        while True:
            result = subprocess.run(
                ["bash", "-c", "ps -eo pid,comm,%mem,%cpu,vsz,rss | grep java"],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                universal_newlines=True
            )

            lines = result.stdout.splitlines()
            if not lines:  
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
            time.sleep(3)  
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
                 verifier="vbox",
                 time_="true",
                 compact="true",
                 merge="true",
                 prune="prune_opt",
                 construct="purdom+",
                 update="italino+",
                 sat="vboxsat",
                 ):
    print("Run Vbox")
    process = subprocess.Popen([f"./build/SerVerifier",
                                log_dir, verifier, time_, compact, merge, prune, construct, update, sat],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                            universal_newlines=True)

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
    print("Running Leopard Verifier...")
    process = subprocess.Popen([f"./build/SerVerifier", log_dir, "leopard"],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               universal_newlines=True)
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


def run_cobra():
    print("Running Cobra GPU Verifier...")
    original_dir = os.getcwd() 
    try:
        command = "source baselines/CobraHome/env.sh && env"
        proc = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True, executable='/bin/bash')
        output, _ = proc.communicate()

        env = os.environ.copy()
        for line in output.decode().splitlines():
            key, value = line.split('=', 1)
            env[key] = value

        cobra_verifier_dir = "./baselines/CobraHome/CobraVerifier"
        cobra_log_dir_abs = os.path.abspath(cobra_log_root)
        #print(cobra_log_dir_abs)
        os.chdir(cobra_verifier_dir)
        process = subprocess.Popen([
            "./run.sh",
            "mono",
            "audit",
            "./cobra.conf.default",
            cobra_log_dir_abs
        ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            env=env)

        memory_usage = [0]
        memory_monitor_thread = threading.Thread(target=monitor_java_memory, args=(memory_usage,), daemon=True)
        memory_monitor_thread.start()

        try:
            stdout, stderr = process.communicate(timeout=TIMEOUT)
            run_time = get_cobra_runtime(stdout) 
            accept = "ACCEPT" in stdout
            if "duplicated wid" in stdout:
                accept = True
            return accept, run_time, memory_usage[0], stdout

        except subprocess.TimeoutExpired:
            print("Timeout occurred, terminating process...")
            process.terminate() 
            try:
                process.wait(timeout=3)  
            except subprocess.TimeoutExpired:
                print("Process did not terminate, forcing kill...")
                process.kill()  
            kill_java_processes()
            run_time = TIMEOUT
            return False, run_time, memory_usage[0], "Process terminated due to timeout."

        except psutil.NoSuchProcess:
            return False, 0, memory_usage[0], "Process no longer exists."

        except Exception as e:
            print(f"Error: {e}")
            return False, 0, memory_usage[0], str(e)

    finally:
        memory_monitor_thread.join(timeout=1)  
        os.chdir(original_dir)


def run_be():
    print("Running BE Verifier...")
    command = [
        "dbcop",
        "verify",
        "--cons", "ser",
        "--out_dir", be_log_root,
        "--ver_dir", be_log_root
    ]

    process = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True
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


def eval_completeness_real_world():
    print("Completeness Experiment.")
    # cases = ['cock-blog','cock-G2','fauna-page','yuga-disW-a','yuga-G2-a','yuga-G2-b','yuga-G2-c']
    tmp_log_dir = Path("./data/vbox_data/tmp/")
    with open(real_word_result_path, "w") as f:
        column_width = 20
        f.write(f"{'Real Word Case'.ljust(40)}{'Vbox'.ljust(column_width)}{'Cobra'.ljust(column_width)}"
                f"{'Leopard'.ljust(column_width)}{'BE'.ljust(column_width)}\n")
        for root, dirs, _ in os.walk(real_word_cases_dir):
            for dir in dirs:
                case_dir = os.path.join(root, dir)
                print(case_dir)
                print(tmp_log_dir)
                clear_path(tmp_log_dir)
                transform_to_vbox(case_dir,tmp_log_dir)
                transform(tmp_log_dir)
                # 1. verifier
                accept_verifier, _, _, stdout = run_vbox(log_dir=tmp_log_dir,time_="false")
                # print(stdout)
                
                # 2. Cobra
                accept_cobra, _, _, stdout = run_cobra()
                print(stdout)
                #accept_cobra = True
                # print(stdout)
                # 3. Leopard
                accept_leopard, _, _, stdout = run_leopard(log_dir=tmp_log_dir)
                print(stdout)
                # 4. BE
                accept_be, _, _, _ = run_be()
                f.write(f"{dir.ljust(40)}{str(accept_verifier).ljust(column_width)}"
                        f"{str(accept_cobra).ljust(column_width)}{str(accept_leopard).ljust(column_width)}"
                        f"{str(accept_be).ljust(column_width)}\n")
        
    
        
        
    


def eval_completeness_coo():
    print("Completeness Experiment.")
    result = [[0 for _ in range(4)] for _ in range(3)]  # RAT,WAT,IAT
    types = ["rat", "wat", "iat"]
    with open(coo_result_path, "w") as f:
        column_width = 20
        f.write(f"{'Coo-case'.ljust(40)}{'Vbox'.ljust(column_width)}{'Cobra'.ljust(column_width)}"
                f"{'Leopard'.ljust(column_width)}{'BE'.ljust(column_width)}\n")

        for root, dirs, _ in os.walk(coo_cases_dir):
            for dir in dirs:
                print(f"Case:{dir}")
                case_type = dir[0:3]
                case_dir = os.path.join(root, dir)
                transform(case_dir)
                # 1. verifier
                accept_verifier, _, _, stdout = run_vbox(log_dir=case_dir)
                # print(stdout)
                result[types.index(case_type)][0] += not accept_verifier
                # 2. Cobra
                accept_cobra, _, _, stdout = run_cobra()
                print(stdout)
                #accept_cobra = True
                # print(stdout)
                result[types.index(case_type)][1] += not accept_cobra
                # 3. Leopard
                accept_leopard, _, _, stdout = run_leopard(log_dir=case_dir)
                #print(stdout)
                result[types.index(case_type)][2] += not accept_leopard
                # 4. BE
                accept_be, _, _, _ = run_be()
                result[types.index(case_type)][3] += not accept_be
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


def eval_efficiency(num_trxs=10000):
    histories = [
        f"blindw_pred_{num_trxs}",
        f"blindw_wh_{num_trxs}",
        f"blindw_wr_{num_trxs}",
        f"blindw_rh_{num_trxs}",
        f"ctwitter_{num_trxs}",
        f"tpcc_{num_trxs}"
    ]
    
    path = raw_root / "efficiency.csv"
    memory_usage = [[] for _ in range(len(histories))]
    run_time = [[]for _ in range(len(histories))]
    total_cons = [0 for _ in range(len(histories))]
    vbox_pruned_cons = [0 for _ in range(len(histories))]
    cobra_pruned_cons = [0 for _ in range(len(histories))]
    with open(path, mode="w", newline="") as csvfile:
        csv_writer = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        headers = ["solver"] + histories
        csv_writer.writerow(headers)
        
        for i, benchmark in enumerate(histories):
            case_dir = vbox_data_root/benchmark
           
            transform(case_dir)
            print(case_dir)
            # 1. vbox
            accept, time, usage, stdout = run_vbox(log_dir=case_dir)
            print(stdout)
            assert accept == True
            run_time[i].append(time)
            memory_usage[i].append(usage)
            total_cons[i] = get_total_constraints(stdout)
            vbox_pruned_cons[i] = get_vbox_pruned_constraint(stdout)
            if 'pred' in benchmark:
                for _ in range(3):
                    run_time[i].append(0)
                    memory_usage[i].append(0)
                continue
            # 2. Cobra
            accept, time, usage, stdout = run_cobra()
            print(stdout)
            memory_usage[i].append(usage)
            if accept == False:
                run_time[i].append(TIMEOUT)
            else:
                run_time[i].append(time)
            cobra_pruned_cons[i] = get_cobra_pruned_constraint(stdout)

            # 3. Leopard
            accept, time, usage, stdout = run_leopard(log_dir=case_dir)
            print(stdout)
            assert accept == True
            run_time[i].append(time)
            memory_usage[i].append(usage)

            # 4. BE
            accept, time, usage, stdout = run_be()
            run_time[i].append(time)
            memory_usage[i].append(usage)
            
        methods = ["Vbox", "Cobra", "Leopard", "BE"]
        
        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(histories)):
                row.append(f"{run_time[i][j]} s") 
            csv_writer.writerow(row)

        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(histories)):
                row.append(f"{memory_usage[i][j]} mb") 
            csv_writer.writerow(row)
        
        row = ["total constraint"]
        for cons in total_cons:
            row.append(str(cons))
        csv_writer.writerow(row)    
        
        row = ["vbox constraint"]
        for cons in vbox_pruned_cons:
            row.append(str(cons))
        csv_writer.writerow(row)    
        
        row = ["cobra constraint"]
        for cons in cobra_pruned_cons:
            row.append(str(cons))
        csv_writer.writerow(row)    
        


def eval_scalability(workload="blindw_wr"):
    histories = [f"{workload}_{(i+1)*10000}" for i in range(10)]
    path = raw_root / "scalability.csv"
    memory_usage = [[] for _ in range(len(histories))]
    run_time = [[]for _ in range(len(histories))]
    with open(path, mode="w", newline="") as csvfile:
        csv_writer = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        headers = ["method"] + histories
        csv_writer.writerow(headers)
        
        for i, benchmark in enumerate(histories):
            case_dir = vbox_data_root / benchmark
        
            transform(case_dir)
            # 1. vbox
            accept, time, usage, stdout = run_vbox(log_dir=case_dir)
            print(stdout)
            assert accept == True
            run_time[i].append(time)
            memory_usage[i].append(usage)
            
            # 2. Cobra
            accept, time, usage, stdout = run_cobra()
            print(stdout)
            memory_usage[i].append(usage)
            if accept == False:
                run_time[i].append(TIMEOUT)
            else:
                run_time[i].append(time)
                
            # 3. Leopard
            accept, time, usage, stdout = run_leopard(log_dir=case_dir)
            print(stdout)
            assert accept == True
            run_time[i].append(time)
            memory_usage[i].append(usage)
            
            # 4. BE
            accept, time, usage, stdout = run_be()
            time = 600
            run_time[i].append(600)
            memory_usage[i].append(usage)

        methods = ["Vbox", "Cobra", "Leopard", "BE"]
        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(histories)):
                row.append(f"{run_time[i][j]} s") 
            csv_writer.writerow(row)

        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(histories)):
                row.append(f"{memory_usage[i][j]} mb") 
            csv_writer.writerow(row)


def eval_update_closure(num_trxs=10000):
    histories = [  f"blindw_wh_{num_trxs}",
                  f"blindw_wr_{num_trxs}",
                  f"blindw_rh_{num_trxs}",
                  f"blindw_pred_{num_trxs}",
                  f"ctwitter_{num_trxs}",
                  f"tpcc_{num_trxs}",
    ]
    path = raw_root / "update.csv"
    update_time = [[]for _ in range(len(histories))]
    
    with open(path, mode="w", newline="") as csvfile:
        csv_writer = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        headers = ["method"] + histories
        csv_writer.writerow(headers)
        methods = ["italino+","italino","warshall"]
        
        for i, benchmark in enumerate(histories):
            case_dir = vbox_data_root / benchmark
            for method in methods:
                _, run_time, _, stdout = run_vbox(log_dir=case_dir, update=method)
                print(stdout)
                update_time[i].append(run_time)
            
        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(histories)):
                row.append(f"{update_time[i][j]} us") 
            csv_writer.writerow(row)


def eval_construct_closure(num_trxs=10000):
    histories = [f"blindw_wh_{num_trxs}",
                  f"blindw_wr_{num_trxs}",
                  f"blindw_rh_{num_trxs}",
                  f"blindw_pred_{num_trxs}",
                  f"ctwitter_{num_trxs}",
                  f"tpcc_{num_trxs}",
    ]
    path = raw_root / "construct.csv"
    construct_time = [[]for _ in range(len(histories))]
    
    with open(path, mode="w", newline="") as csvfile:
        csv_writer = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        headers = ["method"] + histories
        csv_writer.writerow(headers)
        methods = ["purdom+","purdom","warshall","italino+","italino"]
        
        for i, benchmark in enumerate(histories):
            case_dir = vbox_data_root / benchmark
            for method in methods:
                _, _, _, stdout = run_vbox(log_dir=case_dir, construct=method)
                print(stdout)
                construct_time[i].append(get_closure_construct_time(stdout))
            
        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(histories)):
                row.append(f"{construct_time[i][j]} us") 
            csv_writer.writerow(row)
        

def eval_solver(num_trxs=10000):
    histories = [
        f"blindw_wh_{num_trxs}",
        f"blindw_wr_{num_trxs}",
        f"blindw_rh_{num_trxs}",
        f"ctwitter_{num_trxs}",
    ]
    path = raw_root / "solver.csv"
    solve_time = [[] for _ in range(len(histories))]

    with open(path, mode="w", newline="") as csvfile:
        csv_writer = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        headers = ["solver"] + histories
        csv_writer.writerow(headers)
        
        methods = ["vboxsat", "monosat", "minisat"]
        
        for i, benchmark in enumerate(histories):
            case_dir = vbox_data_root / benchmark
            for method in methods:
                _, _, _, stdout = run_vbox(log_dir=case_dir, sat=method)
                solve_time[i].append(get_solve_time(stdout))
            
        for j, method in enumerate(methods):
            row = [method]
            for i in range(len(histories)):
                row.append(f"{solve_time[i][j]} us") 
            csv_writer.writerow(row)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('eval', type=str, choices=['base', 'scalability','completeness','construct','update','solver'])
    args = parser.parse_args()
    if args.eval == "base":
        eval_efficiency()
    elif args.eval =="scalability":
        eval_scalability()
        eval_completeness_real_world()
    elif args.eval == "completeness":
        eval_completeness_real_world()
        eval_completeness_coo()
    elif args.eval == "construct":
        eval_construct_closure()
    elif args.eval == "update":
        eval_update_closure()
    elif args.eval == "solver":
        eval_solver()
        
    
    


