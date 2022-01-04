#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

/**
Creates 2d array with rowsCount rows and colsCount columns, filled with 0.0 values.
(works like zeros function in numpy)
**/
float** zeros(int rowsCount, int colsCount) {
    float* values = calloc(rowsCount*colsCount, sizeof(float));
    float** rows = malloc(rowsCount*sizeof(float*));
    for (int i = 0; i < rowsCount; i++) {
        rows[i] = values + i * colsCount;
    }
    return rows;
}

/**
Prints 2d array in convenient way
**/
void print(float **array, int rowsCount, int colsCount) {
    for(int y = 0; y < rowsCount; y++) {
        for(int x = 0; x < colsCount; x++) {
            if(array[y][x] >= 0) printf(" ");
            printf("%f    ", array[y][x]);
        }
        printf("\n");
    }
}

void printSubMatrix(float **array, int startRowIdx, int rowsCount, int colsCount) {
    for(int y = startRowIdx; y < rowsCount; y++) {
        for(int x = 0; x < colsCount; x++) {
            if(array[y][x] >= 0) printf(" ");
            printf("%f    ", array[y][x]);
        }
        printf("\n");
    }
}

/**
To use only for testing
Prints out the matrix part that belongs to process. Uses sleep to plrint parts in order.
**/
void printResult(float **array, int rank, int rowsCount, int colsCount) {
    int startRowIdx = rank == 0 ? 0 : 1;
    int printedRowsCount = rank == size - 1 ? rowsCount : rowsCount - 1;
    sleep(rank);
    printSubMatrix(matrix, startRowIdx, printedRowsCount, colsCount);
}

/**
Each process has extra rows, on which it does not make calculations - they are received from another process
and needed for the formula.
**/
int getRowsCount(int n, int rank, int size, int stripHeight) {
    int rowsCount = rank < size - 1 ? stripHeight : n - (size - 1) * stripHeight;
    if(rank == 0 || rank == size - 1) {
        rowsCount += 1;
    } else {
        rowsCount += 2;
    }
    return rowsCount;
}

/**
Each process has a full strip to make calculations, so number of columns is equal to size of a matrix.
**/
int getColsCount(int n) {
    return n;
}

int main (int argc, char * argv[])
{
    int rank, size;

    MPI_Init (&argc, &argv);                // starts MPI
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);  // get current process id
    MPI_Comm_size (MPI_COMM_WORLD, &size);  // get number of processes 

    int a = 1;                  // size of a plate in meters
    int n = 100;                  // size of the matrix describing the plate
    int simulationLength = 300;
    double p = 10.0;            // constant from the formula
    double T = 10.0;            // constant from the formula
    double k = (double)a/n;     // distance between points for which we measure values
    int pointsPerProcess = (int)ceil((double)n / size);  // the width of the strip which gets the process to calculate

    int rowsCount = getRowsCount(n, rank, size, pointsPerProcess);
    int colsCount = getColsCount(n);
    float** matrix = zeros(rowsCount, colsCount);


    double calculation_time_start, calculation_time_end, communication_time_start, communication_time_end;
    double calculation_time, communication_time;

    /**
    We have to make calculations on points like on a chessboard, once on dark and once on light fields. It is needed to
    get right values in makeing calculations in place.
    **/
    int processShift = (pointsPerProcess % 2 == 0 && rank % 2 == 1) ? 1 : 0;
    for(int counter = 0; counter < simulationLength; counter++) {
        int counterShift = counter % 2;
        bool shift_indicator = (processShift == 0 && counterShift != 0) || (processShift != 0 && counterShift == 0);

        calculation_time_start = MPI_Wtime();
        for(int y = 1; y < rowsCount - 1; y++) {
            int shift = y % 2 == shift_indicator ? 1 : 0;
            for(int x = 1 + shift; x < colsCount - 1; x += 2) {
                matrix[y][x] = 0.25 * (matrix[y-1][x] + matrix[y+1][x] + matrix[y][x+1] + matrix[y][x-1] + -p/T * pow(k, 2));
            }
        }
        calculation_time_end = MPI_Wtime();

        // We send data only to processes with ranks: rank - 1 and rank + 1
        if(rank > 0) {
            MPI_Send(matrix[1], colsCount, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD);
        }
        if(rank < size - 1) {
            MPI_Send(matrix[rowsCount - 2], colsCount, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD);
        }

        MPI_Status status;
        if(rank > 0) {
            MPI_Recv(matrix[0], colsCount, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, &status);
        }
        if(rank < size - 1) {
            MPI_Recv(matrix[rowsCount - 1], colsCount, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD, &status);
        }
    }

    // printResult(matrix, rank, rowsCount, colsCount);

    MPI_Finalize();
    return 0;
}