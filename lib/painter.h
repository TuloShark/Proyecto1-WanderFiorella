#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RAINBOW/src/C/rainbow.h"

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
    gotoxy(x, y);

    switch(color){
        case 0: printf("\t\t%s%c%s\n",LGREEN,character,RESET); break;
        case 1: printf("\t\t%s%c%s\n",MGREEN,character,RESET); break;
        case 2: printf("\t\t%s%c%s\n",DGREEN,character,RESET); break;
        case 3: printf("\t\t%s%c%s\n",LRED,character,RESET); break;
        case 4: printf("\t\t%s%c%s\n",DRED,character,RESET); break;
        case 5: printf("\t\t%s%c%s\n",LWHITE,character,RESET); break;
        case 6: printf("\t\t%s%c%s\n",DWHITE,character,RESET); break;
        case 7: printf("\t\t%s%c%s\n",LBLUE,character,RESET); break;
        case 8: printf("\t\t%s%c%s\n",DBLUE,character,RESET); break;
        case 9: printf("\t\t%s%c%s\n",LCYAN,character,RESET); break;
        case 10: printf("\t\t%s%c%s\n",DCYAN,character,RESET); break;
        case 11: printf("\t\t%s%c%s\n",ORANGE,character,RESET); break;
        case 12: printf("\t\t%s%c%s\n",YELLOW,character,RESET); break;
        case 13: printf("\t\t%s%c%s\n",MAGENTA,character,RESET); break;
        case 14: printf("\t\t%s%c%s\n",LEMON,character,RESET); break;
        default: printf("\t\t%s%c%s\n",PINK,character,RESET); break;
    }

}

