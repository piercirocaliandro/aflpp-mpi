#!/usr/bin/python3
import time
import os

fuzz_dir = "/home/cr0/xpdf_example/output_mpi/"  # output directory for fuzzing
interesting_stats = ["execs_done", "bitmap_cvg", "saved_crashes"]
content = []

# SLEEP_TIME is the time in minutes for which this script will sleep
# Effective sleep time will be SLEEP_TIME*SECONDS, while total exec time
# will be TOTAL*SECONDS.
SLEEP_TIME = 2
SECONDS = 60
TOTAL = 5
data_path = "data/"

statistics = {}  # dict of dicts to gather metrics
stat_paths = []

for root, dirs, files in os.walk(fuzz_dir):
    if "fuzzer_stats" in files:
        statistics[root.split("/")[-1]] = {}
        stat_paths.append(root+"/fuzzer_stats")

for _ in range(0, TOTAL, SLEEP_TIME):
    #time.sleep(SLEEP_TIME*SECONDS)  # go to sleep, so fuzzers will produce some interesting metrics
    for p in stat_paths:
            k = p.split("/")[-2]
            with open(p, "r") as sf:
                content = sf.read()
            lines = content.split("\n")
            for line in lines:
                line = line.split(" : ")
                if line[0].strip() in interesting_stats:
                    if line[0].strip() not in statistics[k]:
                        statistics[k][line[0].strip()] = []

                    if "." in line[1]:
                        statistics[k][line[0].strip()].append(float(line[1].strip("%")))
                    else:
                        statistics[k][line[0].strip()].append(int(line[1]))

# Finally, write the statistics in .dat format
avg = {"execs_done": [], "bitmap_cvg": [], "saved_crashes": []}
for k in statistics:
        for cat in statistics[k]:
            if len(avg[cat]) == 0:
                avg[cat] = statistics[k][cat]
            else:
                avg[cat] = [a+b for a, b in zip(avg[cat], statistics[k][cat])]

print(avg)

with open("output.csv", "w") as f:
    f.write("Time,Execs done,Bitmap coverage,Saved crashes\n")
    t = SLEEP_TIME
    for x in zip(*avg.values()):
        print(x)
        line = str(t) + "," 
        for elem in x:
            line += str(elem) + ","
        f.write(line[:len(line)-1] + "\n")
