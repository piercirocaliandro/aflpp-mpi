#!/bin/bash

run_bench() {

	# Setup and run MPI
	cd /home/caliandro/afl_mpi
	./compile_afl.sh mpi
	/home/caliandro/$1_example/mpi.sh &
	cd /home/caliandro/afl_mpi/stats
	python3 collect.py "/home/caliandro/$1_example/output_mpi"
	# pkill -9 mpirun
	# pkill -9 afl-fuzz*
	pkill -9 $1_fuzz*
	
	# Tar mpi data
	cd /home/caliandro/afl_mpi
	tar -cf $1_logs.tar log*
	# rm -f log*
	cd /home/caliandro/afl_mpi/stats/data
	tar -cf $1_mpi.tar output*
	rm -f output*

	# Setup and run AFL
	cd /home/caliandro/afl_mpi
	./compile_afl.sh afl
	/home/caliandro/$1_example/afl.sh &
	cd /home/caliandro/afl_mpi/stats
	python3 collect.py "/home/caliandro/$1_example/output_afl/"
	# pkill -9 afl-fuzz*
	pkill -9 $1_fuzz*
	
	# Tar afl data
	cd /home/caliandro/afl_mpi/stats/data
	tar -cf $1_afl.tar output*
	# rm -f output*
	
	rm -rf /home/caliandro/$1_example/output_mpi /home/caliandro/$1_example/output_afl
	# ipcrm -ashm
}

# for name in boringssl cares freetype2 guetzli harfbuzz json lcms libjpeg libxml proj4 re2 woff2; do
# export AFL_NO_UI=1
# export AFL_QUIET=1
# for name in boringssl cares freetype2 harfbuzz json lcms libjpeg libxml proj4 re2 woff2; do
for name in boringssl freetype2 harfbuzz json lcms libjpeg proj4 re2 woff2; do
	run_bench $name
done
