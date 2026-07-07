#!/usr/bin/python3
import time
import os

fuzz_dir = "/home/packer/xpdf-3.02/output/"  # output directory for fuzzing
interesting_stats = ["run_time", "execs_done", "bitmap_cvg", "saved_crashes"]
content = []

SLEEP_TIME = 5
SECONDS = 60
TOTAL = 60
data_path = "data/"

statistics = {}  # dict of dicts to gather metrics

for root, dirs, files in os.walk(fuzz_dir):
    if "fuzzer_stats" in files:
        statistics[root.split("/")[-1]] = {}

# print(statistics)

for _ in range(0, TOTAL, SLEEP_TIME):
    time.sleep(SLEEP_TIME*SECONDS)
    for root, dirs, files in os.walk(fuzz_dir):
        if "fuzzer_stats" in files:
            stats = root+"/fuzzer_stats"
            k = root.split("/")[-1]

            with open(stats, "r") as sf:
                content = sf.read()  # read the whole file in mem

            lines = content.split("\n")
            for line in lines:
                line = line.split(" : ")
                if line[0].strip() in interesting_stats:
                    if line[0].strip() not in statistics[k]:
                        statistics[k][line[0].strip()] = []
                    statistics[k][line[0].strip()].append(line[1].strip("%"))
    print(statistics)

# Finally, write the statistics in .dat format
for k in statistics:
        for cat in statistics[k]:
            with open(data_path+k+"_"+cat, "w") as f:
                t = SLEEP_TIME
                f.write("Time   " + cat + "\n")
                for val in statistics[k][cat]:
                    f.write(str(t)+"     " + val + "\n")
                    t += SLEEP_TIME
