#!/bin/bash

INSTRUMENTED_BINS="inst/"
CC="$HOME/afl_mpi/afl-clang-lto"
CXX="$HOME/afl_mpi/afl-clang-lto++"

compile_binary()
{
        cd $1
        make clean
        CC=$CC CXX=$CXX ./configure --disable-shared --prefix="$1/../build"
        make -j$(nproc)
        make install
        cd ..
        $CXX ./target.cc $1/.libs/libcares.a -I build/include -o $INSTRUMENTED_BINS$2 $HOME/afl_mpi/libAFLDriver.a
        if [[ $3 ]]; then
                mv $INSTRUMENTED_BINS$2 $INSTRUMENTED_BINS$2.$3
        fi
}

compile_laf()
{
        export AFL_LLVM_LAF_ALL=1
        compile_binary $1 $2 laf
        unset AFL_LLVM_LAF_ALL
}

compile_cmplog()
{
        compile_binary $1 $2 afl
        export AFL_LLVM_CMPLOG=1
        compile_binary $1 $2 cmplog
        unset AFL_LLVM_CMPLOG
}

compile_asan()
{
        export AFL_USE_ASAN=1 ; export AFL_USE_UBSAN=1 ; export AFL_USE_CFISAN=1
        compile_binary $1 $2 asan
        unset AFL_USE_ASAN; unset AFL_USE_UBSAN; unset AFL_USE_CFISAN
}

compile_bins ()
{
        mkdir $1/../$INSTRUMENTED_BINS
        compile_asan $1 $2
        compile_laf $1 $2
        compile_cmplog $1 $2
        compile_binary $1 $2
}

if [[ $# -lt 2 ]]; then
        echo "Usage: ./copmile_bins <path> <bin_name>"
fi
compile_bins $1 $2
