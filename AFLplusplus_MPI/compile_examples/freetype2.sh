#!/bin/bash

CC="$HOME/afl_mpi/afl-clang-lto"
CXX="$HOME/afl_mpi/afl-clang-lto++"
FREETYPE_DIR="freetype2_example"

# Test if the project has been already cloned and built. If not, clone and build it
function clone_and_build() {
        mkdir $HOME/$FREETYPE_DIR
        if [[ $? -eq 1 ]]; then
                echo "Project already checked out"
                return 1
        else
                cd $FREETYPE_DIR
                git clone git://git.sv.nongnu.org/freetype/freetype2.git
                cd freetype2/
                git checkout cd02d359a6d0455e9d16b87bf9665961c4699538
                ./autogen.sh
        fi
        return 0
}

cd $HOME
clone_and_build
cd -
./compile_freetype2_bins.sh $HOME/$FREETYPE_DIR/freetype2 freetype2_fuzz
