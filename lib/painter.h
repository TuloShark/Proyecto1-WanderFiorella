#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RAINBOW/src/C/rainbow.h"

// Global variables
int colorsQuantity = 14;
int threadCount = 0;
int colors[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Function to move the cursor to a specific position
// params: x - the x coordinate of the position
//         y - the y coordinate of the position
// returns: void

void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// Function to activate a color
// params: color - the color to be activated
// returns: void

void activateColor(int color){
    colors[color] = 1;
}

// Function to deactivate a color
// params: color - the color to be deactivated
// returns: void

void deactivateColor(int color){
    colors[color] = 0;
}

// Function to set a color
// params: none
// returns: the color code

int setColor()
{
    int colorCode = rand() % colorsQuantity;

    while (colors[colorCode] == 1)
    {
        colorCode = rand() % colorsQuantity;
    }

    activateColor(colorCode);

    return colorCode;
}

// Function to paint the maze
// params: maze - the matrix representing the maze
//         width - the width of the maze
//         height - the height of the maze
// returns: void

void paintMaze(int maze[MAX_ROWS][MAX_COLS][2], int width, int height)
{
    printf("\033[H\033[2J");
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            gotoxy(j+1, i+1);
            if (maze[i][j][0] == 0)
            {
                printf("\u2588");
            }
            else
            {
                if (maze[i][j][1] == 1)
                {
                    printf("\u2591");
                }           
                else{
                    printf(" ");
                } 
            }
        }
    }
}

// Function to paint the movement of a thread
// params: x - the x coordinate of the movement
//         y - the y coordinate of the movement
//         color - the color of the movement
// returns: void

void paintMovement(int x, int y, int color)
{
    const char character[4] = "\u2588";
    gotoxy(x+1, y+1);
    //printf("x: %d  y: %d", x,y);

    switch(color){
        case 0: printf("%s\u2588%s",LGREEN,RESET); break;
        case 1: printf("%s\u2588%s",DGREEN,RESET); break;
        case 2: printf("%s\u2588%s",LRED,RESET); break;
        case 3: printf("%s\u2588%s",DRED,RESET); break;
        case 4: printf("%s\u2588%s",LWHITE,RESET); break;
        case 5: printf("%s\u2588%s",LBLUE,RESET); break;
        case 6: printf("%s\u2588%s",DBLUE,RESET); break;
        case 7: printf("%s\u2588%s",LCYAN,RESET); break;
        case 8: printf("%s\u2588%s",DCYAN,RESET); break;
        case 9: printf("%s\u2588%s",ORANGE,RESET); break;
        case 10: printf("%s\u2588%s",YELLOW,RESET); break;
        case 11: printf("%s\u2588%s",MAGENTA,RESET); break;
        case 12: printf("%s\u2588%s",LEMON,RESET); break;
        default: printf("%s\u2588%s",PINK,RESET); break;
    }
    sleep(1);
    printf("\n");
}

// Function to paint the thread info
// params: history - the history of the thread
//         y - the y coordinate of the thread
//         positionsQuantity - the quantity of positions in the history
//         success - the success of the thread
//         color - the color of the thread
// returns: void

void paintThreadInfo(int history[MAX_ROWS*MAX_COLS][2], int y, int positionsQuantity, bool success, int color)
{
    threadCount++;
    gotoxy(1, y+2+threadCount);
    switch(color){
        case 0: printf("%sThread%s",LGREEN,RESET); break;
        case 1: printf("%sThread%s",DGREEN,RESET); break;
        case 2: printf("%sThread%s",LRED,RESET); break;
        case 3: printf("%sThread%s",DRED,RESET); break;
        case 4: printf("%sThread%s",LWHITE,RESET); break;
        case 5: printf("%sThread%s",LBLUE,RESET); break;
        case 6: printf("%sThread%s",DBLUE,RESET); break;
        case 7: printf("%sThread%s",LCYAN,RESET); break;
        case 8: printf("%sThread%s",DCYAN,RESET); break;
        case 9: printf("%sThread%s",ORANGE,RESET); break;
        case 10: printf("%sThread%s",YELLOW,RESET); break;
        case 11: printf("%sThread%s",MAGENTA,RESET); break;
        case 12: printf("%sThread%s",LEMON,RESET); break;
        default: printf("%sThread%s",PINK,RESET); break;
    }
    printf(" is a %s and went throught the positions ", success ? "success" : "failure");
    for (int i = 0; i < positionsQuantity; i++)
    {
        printf("(%d, %d) ", history[i][0], history[i][1]);
    }
    
}
