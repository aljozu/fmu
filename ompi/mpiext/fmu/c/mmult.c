#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 1024			/* Size of matrices */

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

void fill_matrix(int m[SIZE][SIZE]) {
  static int n=0;
  int i, j;
  for (i=0; i<SIZE; i++)
    for (j=0; j<SIZE; j++)
      m[i][j] = n++;
}

void print_matrix(int m[SIZE][SIZE]) {
  int i, j = 0;
  for (i=0; i<SIZE; i++) {
    printf("\n\t| ");
    for (j=0; j<SIZE; j++)
      printf("%2d ", m[i][j]);
    printf("|");
  }
}

int main(int argc, char *argv[]) {
  int myrank, P, from, to, i, j, k;
  int tag = 0;		/* any value will do */
  MPI_Status status;
  double start_time, end_time;  /* Variables para medir tiempo */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	/* who am I */
  MPI_Comm_size(MPI_COMM_WORLD, &P); /* number of processors */

  if (SIZE % P != 0) {
    if (myrank == 0) printf("Matrix size not divisible by number of processors\n");
    MPI_Finalize();
    exit(-1);
  }

  from = myrank * SIZE / P;
  to = (myrank + 1) * SIZE / P;

  if (myrank == 0) {
    fill_matrix(A);
    fill_matrix(B);
  }

  MPI_Barrier(MPI_COMM_WORLD); /* Sincronizar antes de medir el tiempo */
  start_time = MPI_Wtime();

  MPI_Bcast(B, SIZE * SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(A, SIZE * SIZE / P, MPI_INT, A[from], SIZE * SIZE / P, MPI_INT, 0, MPI_COMM_WORLD);

  for (i = from; i < to; i++) 
    for (j = 0; j < SIZE; j++) {
      C[i][j] = 0;
      for (k = 0; k < SIZE; k++)
        C[i][j] += A[i][k] * B[k][j];
    }

  MPI_Gather(C[from], SIZE * SIZE / P, MPI_INT, C, SIZE * SIZE / P, MPI_INT, 0, MPI_COMM_WORLD);

  end_time = MPI_Wtime(); /* Medir tiempo después de terminar */
  MPI_Barrier(MPI_COMM_WORLD); /* Sincronizar después de medir */

  if (myrank == 0) {
    printf("\nElapsed time: %f seconds\n", end_time - start_time);
  }

  MPI_Finalize();
  return 0;
}
