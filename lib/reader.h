#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 100
#define MAX_COLS 100

// Function to read the maze from a file
// params: filename - the name of the file
//         maze - the matrix to store the maze
//         rows - the number of rows in the maze
//         cols - the number of columns in the maze
// returns: void

void read_maze(char *filename, int maze[MAX_ROWS][MAX_COLS][2], int *rows, int *cols) {

    // Open the file
    FILE *file = fopen(filename, "r");

    // Check if the file was opened successfully
    if (file == NULL) {
        perror("Error abriendo archivo");
        exit(1);
    }

    // Check if file is empty
    if (fgetc(file) == EOF) {
        printf("Error: archivo vacio\n");
        exit(1);
    }

    *rows = 0;
    *cols = 0;
    bool empty = true;

    char line[MAX_COLS];
    while (fgets(line, MAX_COLS, file) != NULL) { // Read a line from the file
        
        if (*cols == 0) {
            *cols = (int) strlen(line) - 1; // -1 to remove the newline character
        }

        for (int i = 0; i < *cols; i++) { // Iterate over the characters in the line
            if (line[i] == '*') { 
                maze[*rows][i][0] = 0;
                maze[*rows][i][1] = 0;
                empty = false;
            } else if (line[i] == '/') {
                maze[*rows][i][0] = 1;
                maze[*rows][i][1] = 1;
            } else if (line[i] == ' ') {
                maze[*rows][i][0] = 1;
                maze[*rows][i][1] = 0;
            } else {
                printf("Error: formato invalido\n");
                exit(1);
            }

        }
        (*rows)++;
    }
    fclose(file);

    // Check matrix is not full or block by walls or empty
    if ((maze[1][0][0] == 0 && maze[0][1][0] == 0) || empty) {
        printf("Error: matriz no valida\n");
        exit(1);
    }
}

// Function to print the maze matrix
// params: maze - the matrix to print
//         rows - the number of rows in the matrix
//         cols - the number of columns in the matrix
// returns: void

void print_matrix(int maze[MAX_ROWS][MAX_COLS][2], int rows, int cols) {
    printf("Representacion de la matriz:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("[%d, %d] ", maze[i][j][0], maze[i][j][1]);
        }
        printf("\n");
    }
}