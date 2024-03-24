#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 100
#define MAX_COLS 100

void read_maze(char *filename, int maze[MAX_ROWS][MAX_COLS][2], int *rows, int *cols) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *rows = 0;
    *cols = 0;
    char line[MAX_COLS];
    while (fgets(line, MAX_COLS, file) != NULL) {
        (*rows)++;
        if (*cols == 0) {
            *cols = (int) strlen(line);
        }
        for (int i = 0; i < *cols; i++) {
            if (line[i] == '*') {
                maze[*rows - 1][i][0] = 0;
                maze[*rows - 1][i][1] = 0;
                
            } else if (line[i] == '/') {
                maze[*rows - 1][i][0] = 1;
                maze[*rows - 1][i][1] = 1;
            } else {
                maze[*rows - 1][i][0] = 1;
                maze[*rows - 1][i][1] = 0;
            }
        }
    }
    fclose(file);
}

void print_matrix(int maze[MAX_ROWS][MAX_COLS][2], int rows, int cols) {
    printf("Representacion de la matriz:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("[%d, %d] ", maze[i][j][0], maze[i][j][1]);
        }
        printf("\n");
    }
}

int main() {

    int maze[MAX_ROWS][MAX_COLS][2];
    int rows, cols;

    read_maze("laberinto.txt", maze, &rows, &cols);

    print_matrix(maze, rows, cols);

    return 0;
}