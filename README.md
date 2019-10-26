# RAID_Perfomance_Using_MPI

Simulated the behavior and the performance of RAID architecture using MPI(Message Passing Interface).  Created n+1 processes with process 0 being a coordinator that reads the requests from a file and distributes the requests to other process. The other processes simultaneously serves a specific disk, performs the read/write requests using SSTF algorithm and sends the result to the coordinator.
