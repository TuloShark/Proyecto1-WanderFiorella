#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "lib/reader.h"
#include "lib/RAINBOW/src/C/rainbow.h"


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

struct ThreadLinkedList {
    struct Thread thread;
    struct ThreadLinkedList *next;
};

// Global variables
struct ThreadLinkedList threads[MAX_ROWS*MAX_COLS]; // Array of threads0
int maze[MAX_ROWS][MAX_COLS][2]; // Matrix representing the maze

// Function to add a thread to the linked list
// params: thread - the thread to add
// returns: void
void addThread(struct Thread thread){
    struct ThreadLinkedList *newThread = (struct ThreadLinkedList*)malloc(sizeof(struct ThreadLinkedList));
    newThread->thread = thread;
    newThread->next = NULL;

    struct ThreadLinkedList *current = threads;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = newThread;
}

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
int verifyAlternativePaths(int maze[MAX_ROWS][MAX_COLS][2], int row, int column, int MaxRows, int MaxCols, enum Direction currentDirection){
    // Check all directions and create threads for each one
    enum Direction dir;
    bool noPath = true;

    int rowOffset = 0;
    int colOffset = 0;

    //printf("Row: %d, Column: %d\n", row, column);

    if (currentDirection != DOWN && !verifyBoundaries(row+1, column, MaxRows, MaxCols) && maze[row+1][column][0] == 1){
        dir = DOWN;
        noPath = false;
        rowOffset = 1;
        //printf("DOWN\n");
    }
    if (currentDirection != UP && !verifyBoundaries(row-1, column, MaxRows, MaxCols) && maze[row-1][column][0] == 1){
       dir = UP;
        noPath = false;
        rowOffset = -1;
       //printf("UP\n");
    }
    if (currentDirection != RIGHT && !verifyBoundaries(row, column+1, MaxRows, MaxCols) && maze[row][column+1][0] == 1){
        dir = RIGHT;
        noPath = false;
        colOffset = 1;
        //printf("RIGHT\n");
    }
    if (currentDirection != LEFT && !verifyBoundaries(row, column-1, MaxRows, MaxCols) && maze[row][column-1][0] == 1){
        dir = LEFT;
        noPath = false;
        colOffset = -1;
        //printf("LEFT\n");
    }
    if (currentDirection == NONE && row == 0 && column == 0){
        rowOffset = 0;
        colOffset = 0;
    }

    // Verify if the position where the thread is going to move has been visited in 
    // the same direction by another thread
    // -----------------------------------------------------------------------------
    // recorrer la lista de hilos y verificar si la posición ya ha sido visitada 
    // en la misma dirección, si es así, no llamar a la función move
    // -----------------------------------------------------------------------------

    if (noPath){
        //printf("NO PATH\n");
    }
    else{
        // Create args
        struct FunctionArgs args = {dir, row+rowOffset, column+colOffset, MaxRows, MaxCols};
        // Create a thread for the new direction
        createThread((void *)&args);
        // printf("---------------------------------------------------\nNew thread!\n Direction: %d  Row: %d  Column: %d \n---------------------------------------------------\n ", dir, row+rowOffset, column+colOffset);
        // move(maze, dir, row+rowOffset, column+colOffset, MaxRows, MaxCols);
        
    }
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
    // Extract arguments
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
    // int colorStr = setColor();


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
        thread->history[i][0] = row;
        thread->history[i][1] = column;
        i++;

        maze[row][column][0] = 0; // Mark the current position as visited
        // paintMovement(column, row, colorStr); // Paint the movement in the maze

        verifyAlternativePaths(maze, row, column, MaxRows, MaxCols, direction);

        printf("Row: %d, Column: %d ", row, column);

        // Check if the current position is the exit
        success = maze[row][column][1] == 1;
        // If the current position is the exit, break the loop
        if (success){
            printf("SUCCESS!!! YOU FOUND IT!!!\n");
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

        printf("Wall: %d\n", wall);

        // If there is a wall, explote alternative paths and destroy the thread
        if (wall){
            //print_matrix(maze, MaxRows, MaxCols);
            printf("2 - Row: %d, Column: %d \n", row, column);
            thread->history[i][0] = row;
            thread->history[i][1] = column;
            // paintMovement(column, row, colorStr); // Paint the movement in the maze

            verifyAlternativePaths(maze, row, column, MaxRows, MaxCols, NONE);
            
            // terminar hilo
            break;
        }

        
    }

    // Print thread info all in one line
    printf("Thread: ");
    for (int j = 0; j < i; j++){
        printf("[%d, %d] ", thread->history[j][0], thread->history[j][1]);
    }
    printf(" -> Success: %d\n", thread->success);

    // Add thread to the linked list
    //addThread(*thread);

    // Free memory
    //free(thread);
}

// Function to create a thread
// params: args
// returns: void
void createThread(void *args){
    struct FunctionArgs *fargs = (struct FunctionArgs *)args;
    pthread_t threadId;
    pthread_create(&threadId, NULL, move, (void *)fargs);
}


// Function to start the maze
// params: maze - the matrix representing the maze
// returns: void
void start(int maze[MAX_ROWS][MAX_COLS][2], int MaxRows, int MaxCols){
    verifyAlternativePaths(maze, 0, 0, MaxRows, MaxCols, NONE);
    // move(maze, DOWN, 0, 0, MaxRows, MaxCols);
}

// Main function
int main() {

    // int maze[MAX_ROWS][MAX_COLS][2];
    int rows, cols;

    char filename[100]; // Allocate memory for the filename
    printf("Filename: ");
    scanf("%s", filename); // Use %s to read a string

    read_maze(filename, maze, &rows, &cols);
    print_matrix(maze, rows, cols);
    // paintMaze(maze, cols, rows);

    start(maze, rows, cols);

    return 0;
}