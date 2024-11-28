#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to multiply two matrices
void matrix_multiply(double *A, double *B, double *C, int rowsA, int colsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            C[i * colsB + j] = 0;
            for (int k = 0; k < colsA; k++) {
                C[i * colsB + j] += A[i * colsA + k] * B[k * colsB + j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int N = 4; // Size of the matrix (NxN)
    int chunk_size = N / world_size; // Number of rows assigned to each process
    if (N % world_size != 0 && world_rank == 0) {
        fprintf(stderr, "Matrix size must be divisible by the number of processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    double *A = NULL, *B = NULL, *C = NULL; // Matrices
    double *A_chunk = malloc(chunk_size * N * sizeof(double)); // Local chunk of A
    double *C_chunk = malloc(chunk_size * N * sizeof(double)); // Local chunk of C

    // Step 1: Initialize data (on root process only)
    if (world_rank == 0) {
        A = malloc(N * N * sizeof(double));
        B = malloc(N * N * sizeof(double));
        C = malloc(N * N * sizeof(double));

        // Initialize matrices A and B
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i * N + j] = i + j; // Example data
                B[i * N + j] = i - j; // Example data
            }
        }

        // Write A chunks to FMU
        for (int i = 0; i < world_size; i++) {
            int tag = i; // Tag identifies the process
            int fmu_tag = 0; // Identifier for matrix A
            MPI_Fmu_Write(&A[i * chunk_size * N], chunk_size * N, MPI_DOUBLE, tag, fmu_tag, MPI_COMM_WORLD);
        }

        // Write B to FMU (shared across all processes)
        MPI_Fmu_Write(B, N * N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD); // tag=0, fmu_tag=1
    }

    // Step 2: Read data chunks from FMU
    int fmu_tag = 0; // Matrix A chunks
    MPI_Fmu_Read(A_chunk, chunk_size * N, world_rank, MPI_DOUBLE, world_rank, fmu_tag, MPI_COMM_WORLD);

    fmu_tag = 1; // Matrix B
    B = malloc(N * N * sizeof(double));
    MPI_Fmu_Read(B, N * N, world_rank, MPI_DOUBLE, 0, fmu_tag, MPI_COMM_WORLD);

    // Step 3: Perform local matrix multiplication
    matrix_multiply(A_chunk, B, C_chunk, chunk_size, N, N);

    // Step 4: Write results to FMU
    fmu_tag = 2; // Result matrix C
    MPI_Fmu_Write(C_chunk, chunk_size * N, MPI_DOUBLE, world_rank, fmu_tag, MPI_COMM_WORLD);

    // Step 5: Gather results at root
    if (world_rank == 0) {
        for (int i = 0; i < world_size; i++) {
            MPI_Fmu_Read(&C[i * chunk_size * N], chunk_size * N, i, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
        }

        // Print result matrix C
        printf("Result matrix C:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%6.2f ", C[i * N + j]);
            }
            printf("\n");
        }

        // Free root buffers
        free(A);
        free(B);
        free(C);
    }

    // Free local buffers
    free(A_chunk);
    free(B);
    free(C_chunk);

    MPI_Finalize();
    return 0;
}
