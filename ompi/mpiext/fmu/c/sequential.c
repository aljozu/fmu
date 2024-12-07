#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1024 /* Size of matrices */

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

void fill_matrix(int m[SIZE][SIZE]) {
    static int n = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            m[i][j] = n++;
}

void print_matrix(int m[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        printf("\n\t| ");
        for (int j = 0; j < SIZE; j++)
            printf("%2d ", m[i][j]);
        printf("|");
    }
    printf("\n");
}

void matrix_multiply(int A[SIZE][SIZE], int B[SIZE][SIZE], int C[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            C[i][j] = 0;
            for (int k = 0; k < SIZE; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    clock_t start, end;
    double cpu_time_used;

    // Fill matrices A and B
    fill_matrix(A);
    fill_matrix(B);

    // Start timing
    start = clock();

    // Perform matrix multiplication
    matrix_multiply(A, B, C);

    // Stop timing
    end = clock();

    // Calculate time taken
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nTime taken for matrix multiplication: %.6f seconds\n", cpu_time_used);

    return 0;
}
