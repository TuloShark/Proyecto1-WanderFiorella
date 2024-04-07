#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "lib/reader.h"
#include "lib/painter.h"

// Structs

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

struct Thread {
    pthread_t thread_id;
    int row;
    int col;
    int history[MAX_ROWS*MAX_COLS][2];
    enum Direction direction;
    bool success;
};

struct FunctionArgs {
    enum Direction direction;
    int initRow;
    int initCol;
    int MaxRows;
    int MaxCols;
};

struct MainThreadArgs {
    int MaxRows;
    int MaxCols;
};

// Global variables

int maze[MAX_ROWS][MAX_COLS][2]; // Matrix representing the maze

// Function to verify if the position is out of boundaries
// params: row - the row of the position
//         column - the column of the position
//         MaxRows - the maximum number of rows in the maze
//         MaxCols - the maximum number of columns in the maze
// returns: true if the position is out of boundaries, false otherwise

bool verifyBoundaries(int row, int column, int MaxRows, int MaxCols){
    return (column < 0 || column >= MaxCols || row < 0 || row >= MaxRows);
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

// Function to verify alternative paths in the maze
// params: maze - the matrix representing the maze
//         row - the row of the thread
//         column - the column of the thread
//         MaxRows - the maximum number of rows in the maze
//         MaxCols - the maximum number of columns in the maze
// returns: void

int verifyAlternativePaths(int row, int column, int MaxRows, int MaxCols, enum Direction currentDirection){

    int dirOffsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    enum Direction directions[4] = {UP, DOWN, LEFT, RIGHT};
    
    for (int i = 0; i < 4; ++i) {
        if (directions[i] == currentDirection) continue;
        
        int newRow = row + dirOffsets[i][0];
        int newCol = column + dirOffsets[i][1];  

        printf("\n");
        
        if (!verifyBoundaries(newRow, newCol, MaxRows, MaxCols) && !verifyWall(maze, newRow, newCol, MaxRows, MaxCols)) {
            
            struct FunctionArgs args = {directions[i], newRow, newCol, MaxRows, MaxCols};
            createThread(args);

            if (currentDirection == NONE) break;
        }
    }

    // Verify if the position where the thread is going to move has been visited in 
    // the same direction by another thread
    // -----------------------------------------------------------------------------
    // recorrer la lista de hilos y verificar si la posición ya ha sido visitada 
    // en la misma dirección, si es así, no llamar a la función move
    // -----------------------------------------------------------------------------

}


// Function to move the thread in the maze
// params: maze - the matrix representing the maze
//         direction - the direction to move the thread
//         initRow - the initial row of the thread
//         initCol - the initial column of the thread
//         MaxRows - the maximum number of rows in the maze
//         MaxCols - the maximum number of columns in the maze
// returns: void

void *move(void*args){

    // Extract aguments

    struct FunctionArgs *fargs = (struct FunctionArgs *)args;
    enum Direction direction = fargs->direction;
    int initRow = fargs->initRow;
    int initCol = fargs->initCol;
    int MaxRows = fargs->MaxRows;
    int MaxCols = fargs->MaxCols;

    // Create and initialize thread (struct)

    struct Thread *thread = (struct Thread*)malloc(sizeof(struct Thread));
    thread->direction = direction;
    thread->success = false;
  
    // Color of the thread

    int colorCode = setColor();

    // Initial position in the maze

    int row = initRow;
    int column = initCol;
    
    // Index for the positions array

    int i = 0;

    // Flags

    bool wall = false;
    bool success = false;

    // Offset for the direction

    int offset;
    if (direction == DOWN || direction == RIGHT) offset = 1;
    if (direction == UP || direction == LEFT) offset = -1;

    while(true){

        // Add position to history

        maze[row][column][0] = 0; // Mark the current position as visited

        thread->history[i][0] = row;
        thread->history[i][1] = column;
        i++;

        paintMovement(column, row, colorCode); // Paint the movement in the maze

        verifyAlternativePaths(row, column, MaxRows, MaxCols, direction);

        // Check if the current position is the exit

        success = maze[row][column][1] == 1;

        // If the current position is the exit, break the loop

        if (success){
            maze[row][column][0] = 1; // Unmark the current position as visited so other threads can visit it too
            thread->success = true;
            break;
        }

        if (direction == DOWN || direction == UP){
            wall = verifyWall(maze, row+offset, column, MaxRows, MaxCols); // Verify if there is a wall in the next position
            
            if(!wall){ // If there is no wall in the next position move to it
                row += offset;
            }
        }
        else if (direction == RIGHT || direction == LEFT){
            wall = verifyWall(maze, row, column+offset, MaxRows, MaxCols); // Verify if there is a wall in the next position

            if(!wall){ // If there is no wall in the next position move to it
                column += offset;
            }
        }

        // If there is a wall, explote alternative paths and destroy the thread

        if (wall){

            maze[row][column][0] = 0;

            thread->history[i][0] = row;
            thread->history[i][1] = column;
            paintMovement(column, row, colorCode); // Paint the movement in the maze

            verifyAlternativePaths(row, column, MaxRows, MaxCols, NONE);
            
            break;
        }

        
    }
    paintThreadInfo(thread->history, MaxRows, i, thread->success, colorCode); // Print thread info
    for (int j = 0; j < i; j++){
        
    }
    deactivateColor(colorCode);

    free(thread);
}

// Function to create a thread
// params: args
// returns: void

void createThread(struct FunctionArgs args){

    struct FunctionArgs *fargs = malloc(sizeof(struct FunctionArgs));;
    *fargs = args;

    int status;

    pthread_t thread;
    pthread_create(&thread, NULL, move, (void *)fargs);
}


// Function to start the maze
// params: maze - the matrix representing the maze
// returns: void

void *start(void *args){
    struct MainThreadArgs *fargs = (struct MainThreadArgs *)args;
    int MaxRows = fargs->MaxRows;
    int MaxCols = fargs->MaxCols;

    maze[0][0][0] = 0; // Mark the initial position as visited

    verifyAlternativePaths(0, 0, MaxRows, MaxCols, NONE);
}

// Main function

int main() {
    int rows, cols;

    char filename[100]; // Allocate memory for the filename
    printf("Filename: ");
    scanf("%s", filename); // Use %s to read a string

    read_maze(filename, maze, &rows, &cols);
    paintMaze(maze, cols, rows);
    
    // Main thread

    struct MainThreadArgs fargs = {rows, cols};
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, start, (void *)&fargs);
    
    while(true){
    }
    return 0;
}
