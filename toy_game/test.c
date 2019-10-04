#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <iostream>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processorS
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);


	#ifdef _OPENMP
	#pragma omp parallel for 
	#endif  // _OPENMP
	    for (size_t istart = 0; istart < 10; istart += 1) {
                int rank = 999;
	#ifdef _OPENMP
		rank = omp_get_thread_num();
	#endif // _OPENMP
                std::cout << "Test the thread number:" << rank << std::endl;
	    }

    // Finalize the MPI environment.
    MPI_Finalize();
}

