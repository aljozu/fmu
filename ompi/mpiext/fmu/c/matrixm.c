#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi-ext.h"

#define SIZE 8 /* Size of matrices */

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

void fill_matrix(int m[SIZE][SIZE]) {
    static int n = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            m[i][j] = n++;
}

void print_matrix(int rank, int m[SIZE][SIZE]) {
    printf("Rank %d - Local Matrix:\n", rank);
    for (int i = 0; i < SIZE; i++) {
        printf("\t| ");
        for (int j = 0; j < SIZE; j++) {
            printf("%2d ", m[i][j]);
        }
        printf("|\n");
    }
}

void print_local_matrix(int rank, int local_matrix[SIZE / 2][SIZE]) {
    printf("Rank %d - Local Matrix:\n", rank);
    for (int i = 0; i < SIZE / 2; i++) {
        printf("\t| ");
        for (int j = 0; j < SIZE; j++) {
            printf("%2d ", local_matrix[i][j]);
        }
        printf("|\n");
    }
}


int main(int argc, char *argv[]) {
    int myrank, P, from, to, i, j, k;
    int tag_A1 = 11, tag_A2 = 12, tag_B = 75, tag_C = 3, tag_r1 = 21, tag_r2=22; /* FMU tags for A, B, and C */
    MPI_Status status;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); /* Get rank */
    MPI_Comm_size(MPI_COMM_WORLD, &P);      /* Get number of processes */
    int avp = P -1;

    if (SIZE % avp != 0) {
        if (myrank == 0) printf("Matrix size not divisible by P-1 (workers).\n");
        MPI_Finalize();
        exit(-1);
    }
    int rows_per_process = SIZE / avp; /* Dividir la matriz en 2 partes iguales */
    from = myrank * SIZE / avp;
    to = (myrank + 1) * SIZE / avp;

    if (myrank == 0) {
        /* Process 0 fills matrices and writes them to the last node */
        fill_matrix(A);
        fill_matrix(B);
    }
    start_time = MPI_Wtime();
    /* Write the first avp rows of matrix A to the last node */
    //MPI_Fmu_Write(A, SIZE * rows_per_process, 0, MPI_INT, 0, tag_A1, MPI_COMM_WORLD);
    
    /* Write the remaining rows (rows 4 to 7) of matrix A to the last node */

    for(int j = 0, t = 11; j < SIZE; j+=rows_per_process,t++){
        MPI_Fmu_Write(*(A + j), SIZE * rows_per_process, 0, MPI_INT, 0, t, MPI_COMM_WORLD);
    }

    MPI_Fmu_Write(B, SIZE * SIZE, 0, MPI_INT, 0, tag_B, MPI_COMM_WORLD);

    /* Cada proceso lee su parte de la matriz A desde el nodo final */
    int local_A[SIZE][SIZE]; /* Parte local de la matriz A */

    for(int j = 1; j < avp; j++) {
        MPI_Fmu_Read(B, SIZE * SIZE, j, MPI_INT, 0, tag_B, MPI_COMM_WORLD);
    }

    for (int j = 0, t = 11; j < avp; j++,t++){
        MPI_Fmu_Read(local_A, rows_per_process * SIZE, j, MPI_INT, 0, t, MPI_COMM_WORLD);
    }
    

    //MPI_Fmu_Read(local_A, rows_per_process * SIZE, 1, MPI_INT, 0, tag_A2, MPI_COMM_WORLD);
        
    if(myrank<=1){
    for (i=0; i<rows_per_process; i++) 
        for (j=0; j<SIZE; j++) {
        C[i][j]=0;
            for (k=0; k<SIZE; k++)
                C[i][j] += local_A[i][k]*B[k][j];
    }
    }   

    for(int j = 0, t = 21; j < avp; j++, t++){
        MPI_Fmu_Write(C, SIZE * rows_per_process, j, MPI_INT, 0, t, MPI_COMM_WORLD);
    }

    
    //MPI_Fmu_Write(C, SIZE * rows_per_process, 1, MPI_INT, 0, tag_r2, MPI_COMM_WORLD);
    for(int j = 0, t = 21; j < SIZE; j+=rows_per_process,t++){
        MPI_Fmu_Read(*(C + j), rows_per_process * SIZE, 0, MPI_INT, 0, t, MPI_COMM_WORLD);
    }
    //MPI_Fmu_Read(C, rows_per_process * SIZE, 0, MPI_INT, 0, tag_r1, MPI_COMM_WORLD);
    
    end_time = MPI_Wtime();
    if (myrank == 0) {
        printf("Total program execution time: %f seconds\n", end_time - start_time);
    }
    // if (myrank==0) {
    //     printf("\n\n");
    //     print_matrix(myrank, A);
    //     printf("\n\n\t       * \n");
    //     print_matrix(myrank, B);
    //     printf("\n\n\t       = \n");
    //     print_matrix(myrank, C);
    //     printf("\n\n");
    // }
    MPI_Finalize();
    return 0;
}

    // MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes

    // for (int rank = 0; rank < P; rank++) {
    //     if (myrank == rank) {
    //         print_matrix(rank, local_A);
    //         fflush(stdout); // Ensure output is displayed immediately
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD); // Synchronize before moving to the next rank
    // }
