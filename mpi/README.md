# MPI setup

To make MPI work cross-machine, an initial setup has to be done. For the reminder of the document, it will be assumed that the cluster is composed of 3 machines.

First thing first, each machine should be present in the `known_hosts` of each other node.

Then, the .rhosts file should be copied in the root directory from which MPI jobs will be launched. Lastly, jobs can be run with the command below

```
cp .rhosts $HOME
mpirun --oversubscribe --mca btl_tcp_if_include <iface_name> --mca plm_rsh_agent "ssh -i /path/to/ssh/key" --app /path/to/mpi_app_file 1>&2 >/dev/null
```
