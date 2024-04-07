#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "lib/reader.h"
#include "lib/painter.h"

// Structs & Enums

// Enum for the directions

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

// Struct for the thread

struct Thread {
    pthread_t thread_id;
    int row;
    int col;
    int history[MAX_ROWS*MAX_COLS][2];
    enum Direction direction;
    bool success;
};

// Struct for the move function arguments

struct FunctionArgs {
    enum Direction direction;
    int initRow;
    int initCol;
    int MaxRows;
    int MaxCols;
};

// Struct for the main thread arguments

struct MainThreadArgs {
    int MaxRows;
    int MaxCols;
};

// Global variables
pthread_mutex_t mazeLock = PTHREAD_MUTEX_INITIALIZER; // Mutex for the maze
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutex for the activeThreads
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // Condition variable to signal the end of the program
bool threadActive = false; // Flag to indicate if the threads are active
int activeThreads = 0; // Counter for the active threads
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

    int dirOffsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Offsets for the directions
    enum Direction directions[4] = {UP, DOWN, LEFT, RIGHT}; // Directions
    
    for (int i = 0; i < 4; ++i) { // Iterate over the directions
        if (directions[i] == currentDirection) continue; // Skip the current direction
        
        // Calculate the new position
        int newRow = row + dirOffsets[i][0]; 
        int newCol = column + dirOffsets[i][1];  

        printf("\n");
        
        // Verify if the new position is out of boundaries or if there is a wall
        if (!verifyBoundaries(newRow, newCol, MaxRows, MaxCols) && !verifyWall(maze, newRow, newCol, MaxRows, MaxCols)) {
            
            struct FunctionArgs args = {directions[i], newRow, newCol, MaxRows, MaxCols};
            createThread(args); // Create a thread to move to the new position

            if (currentDirection == NONE) break; // If the current direction is NONE, break the loop (initial thread)
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
        pthread_mutex_lock(&mazeLock);
        maze[row][column][0] = 0; // Mark the current position as visited
        pthread_mutex_unlock(&mazeLock);

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
            // Mark the current position as visited
            pthread_mutex_lock(&mazeLock);
            maze[row][column][0] = 0;
            pthread_mutex_unlock(&mazeLock);
            
            // Add position to history
            thread->history[i][0] = row;
            thread->history[i][1] = column;
            
            paintMovement(column, row, colorCode); // Paint the movement in the maze
            
            verifyAlternativePaths(row, column, MaxRows, MaxCols, NONE);
            
            break;
        }

        
    }
    // Print thread info
    paintThreadInfo(thread->history, MaxRows, i, thread->success, colorCode); // Print thread info
    deactivateColor(colorCode);
    
    // Decrement the activeThreads counter
    pthread_mutex_lock(&lock);
    activeThreads--;
    if (activeThreads == 0){ // If there are no active threads signal the condition variable to end the program
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&lock);

    // Free the thread
    free(thread);
}

// Function to create a thread
// params: args
// returns: void

void createThread(struct FunctionArgs args){
    // Increment the activeThreads counter
    pthread_mutex_lock(&lock);
    activeThreads++;
    pthread_mutex_unlock(&lock);

    struct FunctionArgs *fargs = malloc(sizeof(struct FunctionArgs));;
    *fargs = args;

    int status;

    pthread_t thread;
    pthread_create(&thread, NULL, move, (void *)fargs); // Create the thread
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
    

    // Initialize mutex and condition variable
    pthread_mutex_init(&mazeLock, NULL); // Mutex for the maze
    pthread_mutex_init(&lock, NULL); // Mutex for the activeThreads
    pthread_cond_init(&cond, NULL); // Condition variable to signal the end of the program
    
    // Wait for the threads to finish
    pthread_mutex_lock(&lock);
    while(activeThreads > 0 || threadActive == false){
       pthread_cond_wait(&cond, &lock); // Wait for the condition variable to be signaled
       threadActive = true; // Set the threadActive flag to true to break the loop when all threads are finished
    }
    pthread_mutex_unlock(&lock);
    
    printf("\nLaberinto resuelto! \n");
    
    
    return 0;
}
