#include "ompi_config.h"

#include <stdio.h>
#include <stdlib.h>  // For malloc and free
#include <string.h>  // For snprintf

#include "ompi/mpi/c/bindings.h"
#include "ompi/runtime/params.h"
#include "ompi/communicator/communicator.h"
#include "ompi/errhandler/errhandler.h"
#include "ompi/mca/pml/pml.h"
#include "ompi/datatype/ompi_datatype.h"
#include "ompi/memchecker.h"
#include "ompi/runtime/ompi_spc.h"
#include "opal/class/opal_hash_table.h"

#include "ompi/mpiext/fmu/c/mpiext_fmu_c.h"

static const char FUNC_NAME[] = "MPI_Fmu_Write";


int MPI_Fmu_Write(void *data, int count, int src, MPI_Datatype datatype, int tag, int fmu_tag, MPI_Comm comm) {
    int rank, size;
    MPI_Status status;

    // Get the rank and size of the communicator
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    if (size < 2) {
        if (rank == 0) {
            fprintf(stderr, "MPI_Fmu_SendToLastAndStore: Requires at least 2 processes.\n");
        }
        return MPI_ERR_OTHER;
    }
    char filename[256];
    int filename_len;
    if (rank == src) {
        // Send data to the last process
        MPI_Send(data, count, datatype, size - 1, tag, comm);
    } else if (rank == size - 1) {
        // Buffer to receive data
        void *recv_data = malloc(count * sizeof(datatype));
        if (!recv_data) {
            fprintf(stderr, "Error allocating memory.\n");
            return MPI_ERR_NO_MEM;
        }
        // Receive data from process 0
        MPI_Recv(recv_data, count, datatype, src, tag, comm, &status);
        memcpy(data, recv_data, count * sizeof(datatype));
        // Generate the filename dynamically
        snprintf(filename, sizeof(filename), "fmudata_%d.txt", fmu_tag);
        // Save the data to the file
        //printf("Filename %s and rank %d \n", filename, rank);
        FILE *file = fopen(filename, "w");
        if (!file) {
            fprintf(stderr, "Error opening file: %s\n", filename);
            free(recv_data);
            return MPI_ERR_OTHER;
        }
        fwrite(recv_data, sizeof(datatype), count, file);
        fclose(file);
        opal_hash_table_set_value_ptr(comm->FMU_HASH, (void *)&fmu_tag, sizeof(fmu_tag), strdup(filename));
        free(recv_data);
        filename_len = strlen(filename) + 1; // Include null terminator
        
    } 
    MPI_Barrier(comm);
    MPI_Bcast(&filename_len, 1, MPI_INT, size - 1, comm);
    MPI_Bcast(filename, filename_len, MPI_CHAR, size - 1, comm);

    opal_hash_table_set_value_ptr(comm->FMU_HASH, (void *)&fmu_tag, sizeof(fmu_tag), strdup(filename));
    
    // Other processes do nothing
    return MPI_SUCCESS;
}

