#!/bin/bash

if [[ $# -lt 1 ]]; then
        echo "usage: ./compile_afl.sh <type>"
        exit 1
fi

if [ $1 = "hash" ];
then
	echo "Compiling AFL++ MPI-hash"
	rm afl-fuzz; make afl-fuzz CC=mpicc CFLAGS="-DUSE_MPI -DHASH" -j$(nproc)
elif [ $1 = "cluster" ];
then
	echo "Compiling AFL++ MPI-cluster"
	rm afl-fuzz; make afl-fuzz CC=mpicc CFLAGS="-DUSE_MPI -DHIERARCHICAL" -j$(nproc)
elif [ $1 = "dynamic" ];
then
	echo "Compiling AFL++ MPI-dynamic"
	rm afl-fuzz; make afl-fuzz CC=mpicc CFLAGS="-DUSE_MPI -DMPI_DYNAMIC" -j$(nproc)
elif [ $1 = "hash_ammuina" ];
then
	echo "Compiling AFL++ MPI-hash with AMMUINA"
	rm afl-fuzz; make afl-fuzz CC=mpicc CFLAGS="-DUSE_MPI -DHASH -DAMMUINA" -j$(nproc)
elif [ $1 = "cluster_ammuina" ];
then
	echo "Compiling AFL++ MPI-Hierarchical with AMMUINA"
	rm afl-fuzz; make afl-fuzz CC=mpicc CFLAGS="-DUSE_MPI -DHIERARCHICAL -DAMMUINA" -j$(nproc)
elif [ $1 = "dynamic_ammuina" ];
then
	echo "Compiling AFL++ MPI-Dynamic with AMMUINA"
	rm afl-fuzz; make afl-fuzz CC=mpicc CFLAGS="-DUSE_MPI -DMPI_DYNAMIC -DAMMUINA" -j$(nproc)
elif [ $1 = "afl" ];
then
	echo "Compiling AFL++"
	rm afl-fuzz; make afl-fuzz CC=mpicc -j$(nproc)
else
	echo "Wrong type required"
	exit 1
fi
