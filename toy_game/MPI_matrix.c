#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>

int main(int argc, char** argv) {

    size_t array_size = 17;

	int a[array_size];
    
    for (int i = 0; i < array_size; i++) {
        a[i] = i ;
    }





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



    size_t send_size = ceil(double(array_size) / world_size) ;
    size_t actual_send_size = std::min(send_size, array_size - send_size * world_rank) ;
    std::vector<int>  number(actual_send_size);    



    if (world_rank == 0) {        

        for (int i = 1; i < world_size; i ++ ) {
            size_t actual_send_size = std::min(send_size, array_size - send_size * i) ;
            MPI_Send(&a[send_size * i], actual_send_size, MPI_INT, i, 0, MPI_COMM_WORLD);
            std::cout << "Sending matrix from processor 0 to " << i << std::endl;            
        }

        memcpy( &number[0], a, actual_send_size*sizeof(int)) ;

    } else {  
        
        

        MPI_Recv(&number[0], actual_send_size, MPI_INT, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        std::cout << "Process " << world_rank << " received matrix from process 0 " << std::endl;

    }    

    for (auto ite = number.begin(); ite != number.end(); ite++) {
        std::cout << "In processor rank "<< world_rank <<" vector has number " << *ite << std::endl;
    }


    int rank = 0;
    int num_threads = 1;
    
    #ifdef _OPENMP
    #pragma omp parallel 
    {
        num_threads = omp_get_num_threads();
    }
    # endif

    std::vector<std::vector<int>> result(num_threads, std::vector<int>(actual_send_size));

	#ifdef _OPENMP
	#pragma omp parallel for private(rank)
	#endif  // _OPENMP
	    for (size_t istart = 0; istart < actual_send_size; istart += 1) {
       
	#ifdef _OPENMP
		    rank = omp_get_thread_num(); 
	#endif // _OPENMP
            result[rank][istart] = rank* 100 +  number[istart] ;
	    }


    for (auto ite = result.begin(); ite != result.end(); ite++) {
        for (auto ite2 = (*ite).begin(); ite2 != (*ite).end(); ite2++) {
            std::cout << "In processor rank "<< world_rank << " and rank " << rank << " vector has number " << *ite2 << std::endl;
        }
    }



    // Finalize the MPI environment.
    MPI_Finalize();
}

