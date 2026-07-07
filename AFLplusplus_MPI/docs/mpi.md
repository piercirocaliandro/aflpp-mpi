# AFL++ with OpenMPI

## How to compile

Supposing that one has a `mpi` installation on the system (weather it is compiled by source or using distro binaries).<br>

To compile it, run
```
make distrib CC=mpicc
```

## How to run with `mpirun`
`mpirun` makes allows to specify the number of processes to run for a single istance.
This creates problems when running with afl, so one solution is to run mpi as this

```
mpirun -n 1 ./alf-fuzz -M <mainame> <args> : \
    -n 1 ./alf-fuzz -S <name_2> <args> : \
    ...
    -n 1 ./alf-fuzz -S <name_n> <args>
```

# Tested platform
Linux debian 6.1.0-25-amd64 (Debian 12)
