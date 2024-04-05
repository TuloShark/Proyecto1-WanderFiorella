#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "lib/reader.h"

#define MAX_ROWS 100
#define MAX_COLS 100

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
    int MaxRows;
    int MaxCols;
};

int global_maze[MAX_ROWS][MAX_COLS][2];
pthread_mutex_t mutex;

bool verifyBoundaries(int row, int column, int MaxRows, int MaxCols);
bool verifyWall(int maze[MAX_ROWS][MAX_COLS][2], int row, int column, int MaxRows, int MaxCols);
void* move(void* arg);
void verifyAlternativePaths(struct Thread *thread);
void attemptMove(struct Thread* thread);

bool verifyBoundaries(int row, int column, int MaxRows, int MaxCols) {
    return (column < 0 || column >= MaxCols || row < 0 || row >= MaxRows);
}

bool verifyWall(int maze[MAX_ROWS][MAX_COLS][2], int row, int column, int MaxRows, int MaxCols) {
    bool outOfBoundaries = verifyBoundaries(row, column, MaxRows, MaxCols);
    return outOfBoundaries || maze[row][column][0] == 0;
}

void verifyAlternativePaths(struct Thread *thread) {
    int MaxRows = thread->MaxRows;
    int MaxCols = thread->MaxCols;
    int dirOffsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    enum Direction directions[4] = {UP, DOWN, LEFT, RIGHT};
    
    for (int i = 0; i < 4; ++i) {
        if (directions[i] == thread->direction) continue;
        
        int newRow = thread->row + dirOffsets[i][0];
        int newCol = thread->col + dirOffsets[i][1];
        
        if (!verifyBoundaries(newRow, newCol, MaxRows, MaxCols) && !verifyWall(global_maze, newRow, newCol, MaxRows, MaxCols)) {
            struct Thread *newThread = (struct Thread *)malloc(sizeof(struct Thread));
            *newThread = *thread;
            newThread->row = newRow;
            newThread->col = newCol;
            newThread->direction = directions[i];
            
            pthread_create(&newThread->thread_id, NULL, move, (void *)newThread);
            pthread_detach(newThread->thread_id);
        }
    }
}

void* move(void* arg) {
    struct Thread* thread = (struct Thread*) arg;
    attemptMove(thread);

    if (thread->success) {
        printf("Exit found by Thread [%ld] at [%d, %d].\n", (long)thread->thread_id, thread->row, thread->col);
    } else {
        printf("Thread [%ld] terminating at position [%d, %d], no further paths.\n", (long)thread->thread_id, thread->row, thread->col);
    }

    free(thread);
    pthread_exit(NULL);
}

void attemptMove(struct Thread* thread) {
    bool moved = false;
    do {
        pthread_mutex_lock(&mutex);
        if (global_maze[thread->row][thread->col][1] == 1) {
            thread->success = true;
            pthread_mutex_unlock(&mutex);
            break;
        }
        global_maze[thread->row][thread->col][0] = 0;
        pthread_mutex_unlock(&mutex);

        moved = false;

        if (thread->direction == DOWN && thread->row + 1 < thread->MaxRows && !verifyWall(global_maze, thread->row + 1, thread->col, thread->MaxRows, thread->MaxCols)) {
            thread->row += 1;
            moved = true;
        }
        if (thread->direction == UP && thread->row - 1 >= 0 && !verifyWall(global_maze, thread->row - 1, thread->col, thread->MaxRows, thread->MaxCols)) {
            thread->row -= 1;
            moved = true;
        }
        if (thread->direction == RIGHT && thread->col + 1 < thread->MaxCols && !verifyWall(global_maze, thread->row, thread->col + 1, thread->MaxRows, thread->MaxCols)) {
            thread->col += 1;
            moved = true;
        }
        if (thread->direction == LEFT && thread->col - 1 >= 0 && !verifyWall(global_maze, thread->row, thread->col - 1, thread->MaxRows, thread->MaxCols)) {
            thread->col -= 1;
            moved = true;
        }
        if (!moved) {
            verifyAlternativePaths(thread);
        }
    } while (moved);
}

void start(int maze[MAX_ROWS][MAX_COLS][2], int MaxRows, int MaxCols) {
    
    memcpy(global_maze, maze, sizeof(global_maze));
    pthread_mutex_init(&mutex, NULL);
    
    struct Thread* initialThread = malloc(sizeof(struct Thread));
    initialThread->row = 0;
    initialThread->col = 0;
    initialThread->direction = NONE;
    initialThread->success = false;
    initialThread->MaxRows = MaxRows;
    initialThread->MaxCols = MaxCols;
    
    pthread_create(&initialThread->thread_id, NULL, move, (void*)initialThread);
    pthread_join(initialThread->thread_id, NULL);
    pthread_mutex_destroy(&mutex);
}

int main() {
    
    int maze[MAX_ROWS][MAX_COLS][2];
    int rows, cols;
    char filename[100];
    
    printf("Filename: ");
    scanf("%s", filename);
    read_maze(filename, maze, &rows, &cols);
    print_matrix(maze, rows, cols);
    start(maze, rows, cols);
    
    return 0;
}
