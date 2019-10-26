# RAID_Perfomance_Using_MPI

Simulated the behavior and the performance of RAID architecture using MPI(Message Passing Interface).  Created n+1 processes with process 0 being a coordinator that reads the requests from a file and distributes the requests to other process. The other processes simultaneously serves a specific disk, performs the read/write requests using SSTF algorithm and sends the result to the coordinator.

The test file in the repo simulates raid performance for 10 disks.

## Commands to run and test:
mpicc -o out raid_mpi.c // creating an executable

mpirun -n 11 out test10.txt // Testing 
