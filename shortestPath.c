#include <stdio.h>
#include <conio.h>
#define SIZE 9
int map[SIZE][SIZE];
typedef struct _Block
{
    int direction;
    int fromX, fromY;
    int isNewEdge;
    int isTraveled;
    int isUpdating;
} Block;
void loadMap()
{

    int i = 0;
    char tmpMap[9][9];
    FILE *mapFile = fopen("map.txt", "r");
    while (fgets(tmpMap[i], SIZE + 1, mapFile) != NULL)
    {
        if (tmpMap[i][0] != '\n')
            i++;
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            map[i][j] = tmpMap[i][j] - 'A';
        }
        // printf("\n");
    }

    fclose(mapFile);
}

Block pathMap[SIZE][SIZE];
void initPathMap()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            pathMap[i][j].isTraveled = 0;
            pathMap[i][j].isUpdating = 0;
            pathMap[i][j].isNewEdge = 0;
        }
    }
}
char getDirectionSymbol(int direction)
{
    switch (direction)
    {
    case 0:
        return '^';
    case 1:
        return '>';
    case 2:
        return 'v';
    case 3:
        return '<';
    default:
        return 'x';
    }
}
void showStep()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {

            if (pathMap[i][j].isTraveled)
            {
                printf("%c",getDirectionSymbol(pathMap[i][j].direction));
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}
void showStepInfo()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (pathMap[i][j].isTraveled)
            {
                char ch;
                switch (pathMap[i][j].direction)
                {
                case 0:
                    ch = '^';
                    break;
                case 1:
                    ch = '>';
                    break;
                case 2:
                    ch = 'v';
                    break;
                case 3:
                    ch = '<';
                    break;
                }
                printf("[%d,%d %c %d,%d] ", pathMap[i][j].fromX, pathMap[i][j].fromY, ch, j, i);
            }
            else
            {
                printf("                ");
            }
        }
        printf("\n");
    }
}
void flood()
{

    for (int y = 0; y < SIZE; y++)
    {
        for (int x = 0; x < SIZE; x++)
        {

            if (pathMap[y][x].isNewEdge && !pathMap[y][x].isTraveled)
            {

                if (!(map[y][x] & 0b0010) && !pathMap[y - 1][x].isUpdating && !pathMap[y - 1][x].isTraveled)
                {
                    pathMap[y - 1][x].isNewEdge = 1;
                    pathMap[y - 1][x].fromX = x;
                    pathMap[y - 1][x].fromY = y;
                    pathMap[y - 1][x].isUpdating = 1;
                    pathMap[y - 1][x].direction = 0;
                    pathMap[y][x].isTraveled = 1;
                }
                if (!(map[y][x] & 0b0001) && !pathMap[y][x + 1].isUpdating && !pathMap[y][x + 1].isTraveled)
                {
                    pathMap[y][x + 1].isNewEdge = 1;
                    pathMap[y][x + 1].fromX = x;
                    pathMap[y][x + 1].fromY = y;
                    pathMap[y][x + 1].isUpdating = 1;
                    pathMap[y][x + 1].direction = 1;
                    pathMap[y][x].isTraveled = 1;
                }
                if (!(map[y][x] & 0b1000) && !pathMap[y + 1][x].isUpdating && !pathMap[y + 1][x].isTraveled)
                {
                    pathMap[y + 1][x].isNewEdge = 1;
                    pathMap[y + 1][x].fromX = x;
                    pathMap[y + 1][x].fromY = y;
                    pathMap[y + 1][x].isUpdating = 1;
                    pathMap[y + 1][x].direction = 2;
                    pathMap[y][x].isTraveled = 1;
                }
                if (!(map[y][x] & 0b0100) && !pathMap[y][x - 1].isUpdating && !pathMap[y][x - 1].isTraveled)
                {
                    pathMap[y][x - 1].isNewEdge = 1;
                    pathMap[y][x - 1].fromX = x;
                    pathMap[y][x - 1].fromY = y;
                    pathMap[y][x - 1].isUpdating = 1;
                    pathMap[y][x - 1].direction = 3;
                    pathMap[y][x].isTraveled = 1;
                }
            }
        }
    }
    for (int p = 0; p < SIZE; p++)
    {
        for (int q = 0; q < SIZE; q++)
        {
            pathMap[q][p].isUpdating = 0;
        }
    }
}

int startX = 8, startY = 8;
int endX = 4, endY = 4;
int wayStack[81];
int topOfStack = 0;
void pushWayStack(int direction)
{
    wayStack[topOfStack++] = direction;
}
int popWayStack()
{
    return wayStack[--topOfStack];
}
void makeWay()
{
    int x = endX, y = endY;
    while (1)
    {
        pushWayStack(pathMap[y][x].direction);
        int nextX = pathMap[y][x].fromX;
        int nextY = pathMap[y][x].fromY;
        printf("%c", getDirectionSymbol(pathMap[y][x].direction));
        if (x == startX && y == startY)
        {
            break;
        }
        x = nextX;
        y = nextY;
    }
}
int main()
{
    loadMap();

    // pathMap[startY][startX].isTraveled = 1;
    pathMap[startY][startX].isNewEdge = 1;

    while (getch() != 'q')
    {

        flood();
        if (pathMap[endY][endX].isNewEdge || pathMap[endY][endX].isTraveled)
            break;
        showStep();
    }
    makeWay();
    printf("\n\n");
    for (int i = 0; i < topOfStack; i++)
        printf("%c", getDirectionSymbol(wayStack[i]));
    return 1;
}