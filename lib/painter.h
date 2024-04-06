#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RAINBOW/src/C/rainbow.h"

int threadCount = 0;

int colors[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void activateColor(int color){
    colors[color] = 1;
}

void deactivateColor(int color){
    colors[color] = 0;
}

int setColor()
{
    static char colorName[10];
    int colorCode = rand() % 16;

    while (colors[colorCode] == 1)
    {
        colorCode = rand() % 16;
    }

    activateColor(colorCode);

    return colorCode;
}

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

void paintMovement(int x, int y, int color)
{
    const char character[4] = "\u2588";
    gotoxy(x+1, y+1);
    //printf("x: %d  y: %d", x,y);

    switch(color){
        case 0: printf("%s\u2588%s",LGREEN,RESET); break;
        case 1: printf("%s\u2588%s",MGREEN,RESET); break;
        case 2: printf("%s\u2588%s",DGREEN,RESET); break;
        case 3: printf("%s\u2588%s",LRED,RESET); break;
        case 4: printf("%s\u2588%s",DRED,RESET); break;
        case 5: printf("%s\u2588%s",LWHITE,RESET); break;
        case 6: printf("%s\u2588%s",DWHITE,RESET); break;
        case 7: printf("%s\u2588%s",LBLUE,RESET); break;
        case 8: printf("%s\u2588%s",DBLUE,RESET); break;
        case 9: printf("%s\u2588%s",LCYAN,RESET); break;
        case 10: printf("%s\u2588%s",DCYAN,RESET); break;
        case 11: printf("%s\u2588%s",ORANGE,RESET); break;
        case 12: printf("%s\u2588%s",YELLOW,RESET); break;
        case 13: printf("%s\u2588%s",MAGENTA,RESET); break;
        case 14: printf("%s\u2588%s",LEMON,RESET); break;
        default: printf("%s\u2588%s",PINK,RESET); break;
    }
    sleep(1);
    gotoxy(20,20);
}
void paintThreadInfo(int history[MAX_ROWS*MAX_COLS][2], int y, int positionsQuantity, bool success, int color)
{
    threadCount++;
    gotoxy(1, y+2+threadCount);
    switch(color){
        case 0: printf("%sThread%s",LGREEN,RESET); break;
        case 1: printf("%sThread%s",MGREEN,RESET); break;
        case 2: printf("%sThread%s",DGREEN,RESET); break;
        case 3: printf("%sThread%s",LRED,RESET); break;
        case 4: printf("%sThread%s",DRED,RESET); break;
        case 5: printf("%sThread%s",LWHITE,RESET); break;
        case 6: printf("%sThread%s",DWHITE,RESET); break;
        case 7: printf("%sThread%s",LBLUE,RESET); break;
        case 8: printf("%sThread%s",DBLUE,RESET); break;
        case 9: printf("%sThread%s",LCYAN,RESET); break;
        case 10: printf("%sThread%s",DCYAN,RESET); break;
        case 11: printf("%sThread%s",ORANGE,RESET); break;
        case 12: printf("%sThread%s",YELLOW,RESET); break;
        case 13: printf("%sThread%s",MAGENTA,RESET); break;
        case 14: printf("%sThread%s",LEMON,RESET); break;
        default: printf("%sThread%s",PINK,RESET); break;
    }
    printf(" %d is a %s and went throught the positions ", y, success ? "success" : "failure");
    for (int i = 0; i < positionsQuantity; i++)
    {
        printf("(%d, %d) ", history[i][0], history[i][1]);
    }
    
}