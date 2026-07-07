#!/usr/bin/python3

import time
import os
import struct
import subprocess
import sys

if len(sys.argv) >= 2:
    FUZZ_DIR = sys.argv[1]
else:
    exit(1)
'''
 SLEEP_TIME is the time in minutes for which this script will sleep
 Effective sleep time will be SLEEP_TIME*SECONDS, while total exec time
 will be TOTAL*SECONDS.
'''
SLEEP_TIME = 5
SECONDS = 60
TOTAL = 243
# TOTAL = 122
data_path = "data/"

stat_paths = []

def collect_single_files(t):
    # statistics = {"execs_done": 0, "bitmap_cvg": 0.0, "saved_crashes": 0, "fuzz_time": 0, "mpi_send_time": 0.0, "mpi_recv_time": 0.0}
    content = []
    # stat_paths = []

    # for root, dirs, files in os.walk(FUZZ_DIR):
    #     if "fuzzer_stats" in files:
    #         stat_paths.append(root+"/fuzzer_stats")

    for p in stat_paths:
        statistics = { "execs_done": 0, "bitmap_cvg": 0.0, "saved_crashes": 0,
                "fuzz_time": 0, "mpi_send_time": 0.0, "mpi_recv_time": 0.0 , "ammuina": 0 , "ammuina_msg": 0, "received": 0}
        k = p.split("/")[-2]
        with open(p, "r") as sf:
            content = sf.read()
        lines = content.split("\n")
        for line in lines:
            line = line.replace("\t", "    ")
            line = line.split(" : ")
            if line[0].strip() == "execs_done":
                statistics[line[0].strip()] = int(line[1].strip())
            elif line[0].strip() == "bitmap_cvg":
                statistics[line[0].strip()] = float(line[1].strip("%"))
            elif line[0].strip() == "saved_crashes":
                statistics[line[0].strip()] = int(line[1].strip())
            elif line[0].strip() == "fuzz_time":
                statistics[line[0].strip()] = int(line[1].strip())
            elif "mpi_send_time" in line[0].strip():
                statistics[line[0].strip()] = int(line[1].strip())
            elif line[0].strip() == "mpi_recv_time":
                statistics[line[0].strip()] = int(line[1].strip())
            elif "ammuina" in line[0].strip():
                statistics[line[0].strip()] = float(line[1].strip())
            elif "ammuina_msg" in line[0].strip():
                statistics[line[0].strip()] = int(line[1].strip())
            elif "received" in line[0].strip():
                statistics[line[0].strip()] = int(line[1].strip())

        with open(data_path+"output_"+k+".csv", "a") as f:
            line = str(t)+","
            line += str(int(statistics["execs_done"])) + ","
            line += str(float(statistics["bitmap_cvg"])) + ","
            line += str(statistics["saved_crashes"]) + ","
            line += str(int(statistics["fuzz_time"])) + ","
            line += str(statistics["mpi_send_time"]) + ","
            line += str(statistics["mpi_recv_time"]) + ","
            line += str(statistics["ammuina"]) + ","
            line += str(statistics["ammuina_msg"]) + ","
            line += str(statistics["received"]) + "\n"
            f.write(line)

# def collect_metrics(t):
#     statistics = {"execs_done": 0, "bitmap_cvg": 0.0, "saved_crashes": 0, "fuzz_time": 0, "mpi_send_time": 0.0, "mpi_recv_time": 0.0}
#     content = []
# 
#     stat_paths = []
# 
#     for root, dirs, files in os.walk(FUZZ_DIR):
#         if "fuzzer_stats" in files:
#             stat_paths.append(root+"/fuzzer_stats")
# 
#     for p in stat_paths:
#         k = p.split("/")[-2]
#         with open(p, "r") as sf:
#             content = sf.read()
#         lines = content.split("\n")
#         for line in lines:
#             line = line.replace("\t", "    ")
#             line = line.split(" : ")
#             if line[0].strip() == "execs_done":
#                 print("Execs done")
#                 statistics[line[0].strip()] += int(line[1].strip())
#             elif line[0].strip() == "bitmap_cvg":
#                 print("Bitmap cvg")
#                 statistics[line[0].strip()] += float(line[1].strip("%"))
#             elif line[0].strip() == "saved_crashes":
#                 print("Crashes")
#                 statistics[line[0].strip()] += int(line[1].strip())
#             elif line[0].strip() == "fuzz_time":
#                 print("Collecting fuzz time")
#                 statistics[line[0].strip()] += int(line[1].strip())
#             elif "mpi_send_time" in line[0].strip():
#                 print("Send time")
#                 statistics[line[0].strip()] += int(line[1].strip())
#             elif line[0].strip() == "mpi_recv_time":
#                 print("Recv time")
#                 statistics[line[0].strip()] += int(line[1].strip())
# 
#     with open(data_path+"output.csv", "a") as f:
#         line = str(t)+","
#         line += str(int(statistics["execs_done"])) + ","
#         line += str(float(statistics["bitmap_cvg"]/len(stat_paths))) + ","
#         line += str(statistics["saved_crashes"]) + ","
#         line += str(int(statistics["fuzz_time"]/(len(stat_paths)))) + ","
#         line += str(statistics["mpi_send_time"]/len(stat_paths)) + ","
#         line += str(statistics["mpi_recv_time"]/len(stat_paths))
#         f.write(line + "\n")

if __name__ == "__main__":
    time.sleep(2)
    for root, dirs, files in os.walk(sys.argv[1]):
        if "fuzzer_stats" in files:
            stat_paths.append(root+"/fuzzer_stats")
    for f in stat_paths:
        print("Here")
        k = f.split("/")[-2]
        with open(data_path+"output_"+k+".csv", "w") as sf:
            sf.write("Time,Execs done,Bitmap coverage,Saved crashes,Fuzz time,MPI send time,MPI recv time,Ammuina, Ammuina number,Recevied\n")
    for t in range(SLEEP_TIME, TOTAL+SLEEP_TIME, SLEEP_TIME):
        time.sleep(SLEEP_TIME*SECONDS)  # go to sleep, so fuzzers will produce some interesting metrics
        # collect_metrics(t)
        # print("Collecting")
        collect_single_files(t)
    # print("Halting")
    # subprocess.run("pkill -9 afl-fuzz*", shell=True)  # stop the fuzzer
