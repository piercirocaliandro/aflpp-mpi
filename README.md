# Overview

The following directory holds the code and artifacts for the Tosem paper __When High-Performance Computing Meets Software Testing: Distributed Fuzzing using MPI__.

The code has been compiled and succesfully execeuted on __Ubuntu 22.04__ as Operating System

# Directory structure

* `mpi_apps`: MPI app files to run fuzzing campaigns
* `seeds`: Initial corpus for each fuzzing target
* `binaries`: Targets compiled with AFL++ instrumentation

# Pre-requisties

Run `scripts/install_deps.sh` to install dependencies

# Install AFL++

```
cd AFLplusplus_MPI/
LLVM_CONFIG=llvm-config-14 REAL_CC=mpicc REAL_CXX=mpic++ make all CC=mpicc CFLAGS=-DUSE_MPI -j(nproc)
```

# How to compile

See `AFLplusplus_MPI/compile_afl.sh` for compilation flags. 
