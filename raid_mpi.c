#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INPUT_LINE 1024

int shortest_seek_time_first(int current_head_pos, int num_cylinder, int *cylinder_req, int num_cylinder_req);

int main(int argc, char **argv) 
{
    int process_id, no_processes;
    int i = 0, j = 0;
    FILE *file = NULL;
    char *filename = NULL;
    char line[MAX_INPUT_LINE];
    int count = 0;
    int current_head_pos = -1;
    int num_cylinder = 5000;
    int num_cylinder_req = 0;
    int **cylinder_req;
    int SSTF = 0;
    int *size_index_array;
    int *buf_ptr;
    char s[2] = ",";
    char* token;
    int index, value;

    {
	for (i = 1; i < argc; i++)
	{
	    filename = argv[i];
	}
	// Argument error check.
	if (filename == NULL)
	{
	    printf("Error: Please Enter the File Name.\n");
	    exit(0);
	}
	file = fopen(filename, "r"); // opening a file for getting count from the file
	if (file == NULL)  
	{
	    printf("Error: Unable to open file %s\n", filename);
	    exit(0);
	}
	while(fgets(line, sizeof(line), file) != NULL)
	{
	    count++;
	}
	//printf("count %d \n",count);
	fclose(file);

	// Open the file to be read.
	file = fopen(filename, "r");
	if (file == NULL)
	{
	    printf("Error: Unable to open file %s\n", filename);
	    exit(0);
	}
	fgets(line, MAX_INPUT_LINE, file);
	// Get the position of current head
	current_head_pos = atoi(line);
	num_cylinder_req = count-1; // count for cylinder requests and iterating through array
	// Add cylinder requests from file to array.
	cylinder_req = (int **) malloc(current_head_pos * sizeof(int*));
	buf_ptr = (int *)malloc(sizeof(int) * current_head_pos);
	for(i = 0; i < current_head_pos; i++)
	{
	    cylinder_req[i] = (int*)malloc(num_cylinder_req* sizeof(int));
	}
	size_index_array = (int*) malloc(num_cylinder_req * sizeof(int));
	/* Storing the file in a double array */
	while(fgets(line, MAX_INPUT_LINE, file) != NULL) 
	{
	    /* Parsing the line */
	    token = strtok(line, s);
	    for(int i = 0; i < 2; i++)
	    {
		if( i == 0)
		{
		    index = atoi(token);
		    token = strtok(NULL, s);
		}
		else
		{
		    value = atoi(token);
		}
	    }
	    cylinder_req[index - 1][size_index_array[index - 1]] = value;
	    size_index_array[index - 1]++;
	}
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);

    if(process_id == 0) 
    {
	for(i = 1; i < no_processes; i++)
	{
	    MPI_Send(cylinder_req[i-1], size_index_array[i-1], MPI_INT, i, 0, MPI_COMM_WORLD);
	}

	for(i = 1; i < no_processes; i++)
	{
	    MPI_Recv(&SSTF, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    printf("Dist %d: traveled distance is %d \n", i, SSTF);
	}
    }
    else
    {
	MPI_Recv(buf_ptr, size_index_array[process_id-1], MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	SSTF = shortest_seek_time_first(0, num_cylinder, &buf_ptr[0], size_index_array[process_id-1]);
	MPI_Send(&SSTF, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

int shortest_seek_time_first(int current_head_pos, int num_cylinder, int *cylinder_req, int num_cylinder_req) 
{
    int i = 0;
    int j = 0;
    int * sstf_request_order = malloc(sizeof(int) * num_cylinder_req);
    int head_pos = current_head_pos;
    int closest_pos = num_cylinder;
    int closest_idx = 0;
    int sstf_total = 0;
    // Make copy of cylinder requests, so the copy can be changed.
    memcpy(sstf_request_order, cylinder_req, sizeof(int) *num_cylinder_req);
    // Go through each cylinder request, and find the closest cylinder request
    // from the current request.
    for (i = 0; i < num_cylinder_req; i++){
	for (j = 0; j < num_cylinder_req; j++){
	    // If the cylinder request has already be calculated don't use.
	    if (sstf_request_order[j] != -1){
		if (closest_pos > abs(head_pos - sstf_request_order[j])) {

		    closest_idx = j;
		    closest_pos = abs(head_pos - sstf_request_order[j]);}}}
	// Found the closest request, add it to seek time, move to next
	// request.
	head_pos = sstf_request_order[closest_idx];
	sstf_request_order[closest_idx] = -1;
	sstf_total += closest_pos;

	closest_pos = num_cylinder; }
    // Clean up
    free(sstf_request_order);
    sstf_request_order = NULL;
    return sstf_total;
}
