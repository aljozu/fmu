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

static const char FUNC_NAME[] = "MPI_Fmu_Read";

int MPI_Fmu_Read(void *data, int count, int dest, MPI_Datatype datatype, int tag, int fmu_tag, MPI_Comm comm) {
    int rank, size, datatype_size;
    MPI_Status status;

    // Get rank and size
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Get size of the datatype
    MPI_Type_size(datatype, &datatype_size);

    if (rank == size - 1) {
        // FMU Node
        char *filename = NULL;

        // Retrieve filename from hash table
        if (OPAL_SUCCESS != opal_hash_table_get_value_ptr(comm->FMU_HASH, &fmu_tag, sizeof(fmu_tag), (void **)&filename)) {
            fprintf(stderr, "Rank %d: Error retrieving filename for fmu_tag %d.\n", rank, fmu_tag);
            return MPI_ERR_OTHER;
        }

        printf("Rank %d: Filename retrieved = %s\n", rank, filename);

        // Open file
        FILE *file = fopen(filename, "rb");
        if (!file) {
            fprintf(stderr, "Rank %d: Error opening file: %s\n", rank, filename);
            return MPI_ERR_OTHER;
        }

        // Allocate buffer to read file data
        void *file_data = malloc(count * datatype_size);
        if (!file_data) {
            fclose(file);
            fprintf(stderr, "Rank %d: Error allocating memory for file data.\n", rank);
            return MPI_ERR_NO_MEM;
        }

        // Read data from file
        size_t elements_read = fread(file_data, datatype_size, count, file);
        if (elements_read != count) {
            fprintf(stderr, "Rank %d: Error reading data from file. Expected %d, got %zu.\n", rank, count, elements_read);
            free(file_data);
            fclose(file);
            return MPI_ERR_OTHER;
        }

        fclose(file);

        // Send data to destination rank
        MPI_Send(file_data, count, datatype, dest, 0, comm);
        free(file_data);

    } else if (rank == dest) {
        // Destination Rank
        printf("Rank %d: Preparing to receive data from FMU node.\n", rank);

        // Allocate buffer for received data
        void *recv_data = malloc(count * datatype_size);
        if (!recv_data) {
            fprintf(stderr, "Rank %d: Error allocating memory for received data.\n", rank);
            return MPI_ERR_NO_MEM;
        }

        // Receive data from FMU node
        MPI_Recv(recv_data, count, datatype, size - 1, 0, comm, &status);

        // Copy received data into user-provided buffer
        memcpy(data, recv_data, count * datatype_size);
        free(recv_data);

        printf("Rank %d: Data successfully received and stored.\n", rank);
    }

    return MPI_SUCCESS;
}

