#include "ompi_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ompi/communicator/communicator.h"
#include "ompi/mca/pml/pml.h"
#include "ompi/datatype/ompi_datatype.h"

#include "ompi/mpiext/fmu/c/mpiext_fmu_c.h"

static const char FUNC_NAME[] = "MPI_Fmu_Alltoall";

int MPI_Fmu_Alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int world_rank, world_size;
    
    MPI_Comm_rank(comm, &world_rank);
    MPI_Comm_size(comm, &world_size);

    int sendtype_size, recvtype_size;
    MPI_Type_size(sendtype, &sendtype_size);
    MPI_Type_size(recvtype, &recvtype_size);

    // Step 1: Write Phase
    for (int dest = 0; dest < world_size; dest++) {
        if (dest == world_rank) continue; // Skip self

        // Use a unique tag for the communication
        int tag = dest;      // Identifies the receiver
        int fmu_tag = world_rank; // Identifies the sender (optional for debugging/organization)

        // Pointer to the data chunk for the current destination
        void *data = (char *)sendbuf + dest * sendcount * sendtype_size;
        printf("dest \n");
        // Write the data to the FMU node
        MPI_Fmu_Write(data, sendcount, sendtype, tag, fmu_tag, comm);
        printf("asd \n");
    }
    MPI_Barrier(comm);
    // Step 2: Read Phase
    for (int src = 0; src < world_size; src++) {
        if (src == world_rank) {
            // Self data copy directly
            void *self_data = (char *)sendbuf + src * sendcount * sendtype_size;
            memcpy((char *)recvbuf + src * recvcount * recvtype_size, self_data, sendcount * sendtype_size);
            continue;
        }

        // Use a unique tag for the communication
        int tag = world_rank; // Identifies the receiver
        int fmu_tag = src;    // Identifies the sender

        // Pointer to the destination chunk in the receive buffer
        void *recv_data = (char *)recvbuf + src * recvcount * recvtype_size;

        // Read the data from the FMU node
        MPI_Fmu_Read(recv_data, recvcount, src, recvtype, tag, fmu_tag, comm);
    }
    return MPI_SUCCESS;
}
