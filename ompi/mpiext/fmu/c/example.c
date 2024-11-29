#include <mpi.h>
#include <stdio.h>
#include "mpi-ext.h"

int main(int argc, char *argv[]) {
    int rank, size;
    int data = 0;  // Data to send/receive
    int tag = 0, fmu_tag = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) {
        data = 42;
    }
    // Call the custom MPI function
    MPI_Fmu_Write(&data, 1, 0, MPI_INT, tag, fmu_tag, MPI_COMM_WORLD);

    //MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Fmu_Read(&data, 1, 1, MPI_INT, tag, fmu_tag, MPI_COMM_WORLD);
    MPI_Fmu_Read(&data, 1, 2, MPI_INT, tag, fmu_tag, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Process %d sent data %d to the last process.\n", rank, data);
    } else if (rank == size - 1) {
        printf("Process %d received data %d from Process 0.\n", rank, data);
    } else if (rank == 1) {
        printf("Process %d received data %d from Process 0.\n", rank, data);
    } else {
        printf("Process %d received data %d from Process 0.\n", rank, data);
    }
    
    MPI_Finalize();
    return 0;
}
