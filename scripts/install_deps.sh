#!/bin/bash

cd $HOME
echo -e "[+] Bootstrapping..."

# Update packets
sudo apt-get update
sudo apt-get upgrade -y

# AFL++ dependecies
sudo apt-get install -y build-essential python3-dev automake cmake git flex bison libglib2.0-dev libpixman-1-dev python3-setuptools cargo libgtk-3-dev
# try to install llvm 14 and install the distro default if that fails
sudo apt-get install -y lld-14 llvm-14 llvm-14-dev clang-14 || sudo apt-get install -y lld llvm llvm-dev clang
sudo apt-get install -y gcc-$(gcc --version|head -n1|sed 's/\..*//'|sed 's/.* //')-plugin-dev libstdc++-$(gcc --version|head -n1|sed 's/\..*//'|sed 's/.* //')-dev

# OpenMPI
sudo apt-get install -y openmpi-bin openmpi-doc libopenmpi-dev mpich


# Change performance and cpufreq for fuzzing
echo core | sudo tee /proc/sys/kernel/core_pattern
cd /sys/devices/system/cpu
echo performance | sudo tee cpu*/cpufreq/scaling_governor
cd -
