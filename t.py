import  os
import struct
from pathlib import Path

log_dir =Path("/home/weihua/Vbox/data/g2")
cobra_log_dir =Path("./data/cobra")

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
                    print("w")
                    oid = struct.unpack('I', in_file.read(4))[0]
                    in_file.read(8)  # start
                    in_file.read(8)  # end
                    key = struct.unpack('>Q', in_file.read(8))[0]
                    in_file.read(8)
                    in_file.read(4)
                    buffer.append((b'W', oid, key))

                elif op_type == b'R':  # Read
                    print("r")
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