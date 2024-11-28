#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi-ext.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int sendcount = 1; // Number of elements each process sends
    int recvcount = 1; // Number of elements each process receives

    // Initialize send buffer
    int *sendbuf = malloc(world_size * sendcount * sizeof(int));
    for (int i = 0; i < world_size; i++) {
        sendbuf[i] = world_rank * 10 + i; // Unique data for each destination
    }

    // Allocate receive buffer
    int *recvbuf = malloc(world_size * recvcount * sizeof(int));

    // Perform All-to-All communication
    MPI_Fmu_Alltoall(sendbuf, sendcount, MPI_INT, recvbuf, recvcount, MPI_INT, MPI_COMM_WORLD);

    // Print received data
    printf("Process %d received: ", world_rank);
    for (int i = 0; i < world_size; i++) {
        printf("%d ", recvbuf[i]);
    }
    printf("\n");

    free(sendbuf);
    free(recvbuf);
    MPI_Finalize();
    return 0;
}
