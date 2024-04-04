#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader.h"

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

char* setColor()
{
    static char colorName[10];
    int colorCode = rand() % 16;

    while (colors[colorCode] == 1)
    {
        colorCode = rand() % 16;
    }

    activateColor(colorCode);

    if (colorCode == 0) strcopy(colorName, "LGREEN");
    else if (colorCode == 1) strcopy(colorName, "MGREEN");
    else if (colorCode == 2) strcopy(colorName, "DGREEN");
    else if (colorCode == 3) strcopy(colorName, "LRED");
    else if (colorCode == 4) strcopy(colorName, "DRED");
    else if (colorCode == 5) strcopy(colorName, "LWHITE");
    else if (colorCode == 6) strcopy(colorName, "DWHITE");
    else if (colorCode == 7) strcopy(colorName, "LBLUE");
    else if (colorCode == 8) strcopy(colorName, "DBLUE");
    else if (colorCode == 9) strcopy(colorName, "LCYAN");
    else if (colorCode == 10) strcopy(colorName, "DCYAN");
    else if (colorCode == 11) strcopy(colorName, "ORANGE");
    else if (colorCode == 12) strcopy(colorName, "YELLOW");
    else if (colorCode == 13) strcopy(colorName, "MAGENTA");
    else if (colorCode == 14) strcopy(colorName, "LEMON");
    else strcopy(colorName, "PINK");

    return colorName;
}

void paintMaze(int maze[MAX_ROWS][MAX_COLS][2], int width, int height)
{
    int wall = 219;
    int path = 32; //o 219
    int exit = 176;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            gotoxy(j, i);
            if (maze[i][j][0] == 0)
            {
                printf("%c", wall);
            }
            else
            {
                if (maze[i][j][1] == 1)
                {
                    printf("%c", path);
                }           
                else{
                    printf("%c", exit);
                } 
            }
        }
    }
}

void paintMovement(int x, int y, char color[10])
{
    int visited = 178; // o 219
    gotoxy(x, y);
    printf("%s :\t\t%s %c %s\n",color, color, visited,"RESET");
    // printf("LGREEN:\t\t%sHello World in light green text color%s\n",LGREEN,RESET);
}

int main(){
    gotoxy(10, 10);
    printf("Hello, World!\n");

    return 0;
}

