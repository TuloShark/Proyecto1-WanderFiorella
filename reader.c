#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX_ROWS 100
#define MAX_COLS 100

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Thread {
    pthread_t thread_id;
    int row;
    int col;
    int history[MAX_ROWS*MAX_COLS][2];
    enum Direction direction;
    bool success;
};

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

// Function to verify if the position is out of boundaries
// params: row - the row of the position
//         column - the column of the position
//         MaxRows - the maximum number of rows in the maze
//         MaxCols - the maximum number of columns in the maze
// returns: true if the position is out of boundaries, false otherwise
bool verifyBoundaries(int row, int column, int MaxRows, int MaxCols){
    // // left
    // if (column < 0) return true;
    // // right
    // if (column >= MaxCols) return true;
    // // up
    // if (row < 0) return true;
    // // down
    // if (row >= MaxRows) return true;
    // return false;

    return (column < 0 || column >= MaxCols || row < 0 || row >= MaxRows);
}

// Function to verify alternative paths in the maze
// params: maze - the matrix representing the maze
//         row - the row of the thread
//         column - the column of the thread
//         MaxRows - the maximum number of rows in the maze
//         MaxCols - the maximum number of columns in the maze
// returns: void
int verifyAlternativePaths(int maze[MAX_ROWS][MAX_COLS][2], int row, int column, int MaxRows, int MaxCols){
    // Check all directions and create threads for each one
    enum Direction dir;

    if (!verifyBoundaries(row+1, column, MaxRows, MaxCols) && maze[row+1][column][0] == 1){
        dir = DOWN;
    }
    if (!verifyBoundaries(row-1, column, MaxRows, MaxCols) && maze[row-1][column][0] == 1){
       dir = UP;
    }
    if (!verifyBoundaries(row, column+1, MaxRows, MaxCols) && maze[row][column+1][0] == 1){
        dir = RIGHT;
    }
    if (!verifyBoundaries(row, column-1, MaxRows, MaxCols) && maze[row][column-1][0] == 1){
        dir = LEFT;
    }

    // Create thread
    // pthread_create(&thread->thread_id, NULL, move, (void *)thread);
}

// Function to verify if there is a wall in the position
// params: maze - the matrix representing the maze
//         row - the row of the position
//         column - the column of the position
//         MaxRows - the maximum number of rows in the maze
//         MaxCols - the maximum number of columns in the maze
// returns: true if there is a wall in the position, false otherwise
bool verifyWall(int maze[MAX_ROWS][MAX_COLS][2], int row, int column, int MaxRows, int MaxCols){
    // Verify if there is a wall in the next position or if it is out of boundaries
    bool outOfBoundaries = verifyBoundaries(row, column, MaxRows, MaxCols);
    return outOfBoundaries || maze[row][column][0] == 0;
}


// Function to move the thread in the maze
// params: maze - the matrix representing the maze
//         direction - the direction to move the thread
//         initRow - the initial row of the thread
//         initCol - the initial column of the thread
//         MaxRows - the maximum number of rows in the maze
//         MaxCols - the maximum number of columns in the maze
// returns: void
void *move(int maze[MAX_ROWS][MAX_COLS][2], enum Direction direction, int initRow, int initCol, int MaxRows, int MaxCols){
    // -------------------------------------------------------------------------------------
    // Esta es una opción más corta y eficiente en teoría (o esa es la intención jaja) 
    // La otra opción está comentada abajo
    // -------------------------------------------------------------------------------------

    // Create and initialize thread (struct)
    struct Thread *thread = (struct Thread*)malloc(sizeof(struct Thread));
     //(struct *Thread)arg;// malloc(sizeof(struct Thread));
    int positions[MAX_ROWS*MAX_COLS][2]; // [row, column]

    // Initial position in the maze
    int row = initRow;
    int column = initCol;
    
    // Index for the positions array
    int i = 1;

    // Flags
    bool wall = false;
    bool success = false;

    // Offset for the direction
    int offset;
    if (direction == DOWN || direction == RIGHT) offset = 1;
    if (direction == UP || direction == LEFT) offset = -1;

    while(true){
        // Add position to history
        // positions[i][0] = row;
        // positions[i][1] = column;
        thread->history[i][0] = row;
        thread->history[i][1] = column;
        i++;

        if (direction == DOWN || direction == UP){
            wall = verifyWall(maze, row+offset, column, MaxRows, MaxCols); // Verify if there is a wall in the next position
            
            if(!wall){ // If there is no wall in the next position move to it
                success = maze[row+offset][column][1] == 1; // Verify if the next position is the exit
                maze[row][column][0] == 0; // Mark the current position as visited
                row += offset;
            }
        }
        else if (direction == RIGHT || direction == LEFT){
            wall = verifyWall(maze, row, column+offset, MaxRows, MaxCols); // Verify if there is a wall in the next position

            if(!wall){ // If there is no wall in the next position move to it
                success = maze[row][column+offset][1] == 1; // Verify if the next position is the exit
                maze[row][column][0] == 0; // Mark the current position as visited
                column += offset;
            }
        }

        // If there is a wall, explote alternative paths and destroy the thread
        if (wall){
            verifyAlternativePaths(maze, row, column, MaxRows, MaxCols);
            // terminar hilo
            break;
        }

        // If the next position is the exit, break the loop
        if (success){
            thread->success = true;
            break;
        }
    }

    // thread->history = positions;
    
    // -------------------------------------------------------------------------------------
    // Esta es otra opción sin el offset, pero es más larga y redundante
    // Solo hice el ejemplo con el caso de ir hacia abajo, pero es lo mismo para los demás casos
    // Esta incompleta obvio, pero el resto es muy parecido al anterior
    // -------------------------------------------------------------------------------------

    // while(true){
    //     // Verify next position is not a wall
    //     switch (direction) {
    //         case DOWN:
    //             if(maze[row+1][column][0] == 1){
    //                 if (maze[row+1][column][1] == 1){
    //                     success = true;
    //                 }
    //                 row++;
    //             }
    //             else {
    //                 wall = true;
    //             }
    //             break;
    //         case LEFT:
    //             break;
    //         case RIGHT:
    //             break;
    //     }

    // // Verify alternative paths in the new position
    // verifyAlternativePaths(maze, row, column);
    // }
    
}


// Function to start the maze
// params: maze - the matrix representing the maze
// returns: void
void start(int maze[MAX_ROWS][MAX_COLS][2], int MaxRows, int MaxCols){
    verifyAlternativePaths(maze, 0, 0, MaxRows, MaxCols);
}

// Main function
int main() {

    int maze[MAX_ROWS][MAX_COLS][2];
    int rows, cols;

    char filename[100]; // Allocate memory for the filename
    printf("Filename: ");
    scanf("%s", filename); // Use %s to read a string

    read_maze(filename, maze, &rows, &cols);
    print_matrix(maze, rows, cols);

    return 0;
}