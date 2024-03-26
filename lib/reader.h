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
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *rows = 0;
    *cols = 0;
    char line[MAX_COLS];
    while (fgets(line, MAX_COLS, file) != NULL) { // Read a line from the file
        
        if (*cols == 0) {
            *cols = (int) strlen(line) - 1; // -1 to remove the newline character
        }
        for (int i = 0; i < *cols; i++) { // Iterate over the characters in the line
            if (line[i] == '*') { 
                maze[*rows][i][0] = 0;
                maze[*rows][i][1] = 0;
                
            } else if (line[i] == '/') {
                maze[*rows][i][0] = 1;
                maze[*rows][i][1] = 1;
            } else {
                maze[*rows][i][0] = 1;
                maze[*rows][i][1] = 0;
            }
        }
        (*rows)++;
    }
    fclose(file);
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