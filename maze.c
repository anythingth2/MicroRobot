#include <stdio.h>
#include <conio.h>
#define SIZE 11
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
FILE *mapFile;
char map[SIZE][SIZE + 1];

typedef struct _Block
{
    int x, y;
    int type;
} Block;
Block car;
Block goal;
int direction = 0;

Block createBlock(int x, int y, int type)
{
    Block block;
    block.x = x;
    block.y = y;
    block.type = type;
    return block;
}

void loadMap()
{
    mapFile = fopen("testmap.txt", "r");
    int i = 0;
    while (fgets(map[i], SIZE + 1, mapFile) != NULL)
    {
        if (map[i][0] != '\n')
            i++;
    }
    fclose(mapFile);
}
void showMap()
{
    printf("\n\n\n Map!\n");
    int i = 0;
    for (i = 0; i < SIZE; i++)
    {
        printf("%s\n", map[i]);
    }
    printf("\n");
}

char getDirectionChar(int dir)
{
    switch (dir)
    {
    case NORTH:
        return 'N';
    case EAST:
        return 'E';
    case SOUTH:
        return 'S';
    case WEST:
        return 'W';
    default:
        return 'X';
    }
}
void showMovementOnMap(Block block, int direction)
{
    char localMap[SIZE][SIZE + 1];

    int i = 0;
    int j = 0;
    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE + 1; j++)
        {
            localMap[i][j] = map[i][j];
        }
    }

    localMap[block.y][block.x] = getDirectionChar(direction);
    for (i = 0; i < SIZE; i++)
    {
        printf("%s\n", localMap[i]);
    }
    printf("\n");
}
void setPosMap(Block block, char ch)
{
    map[block.y][block.x] = ch;
}

Block stack[100];
int topOfStack = 0;

void pushStack(Block block)
{
    stack[topOfStack++] = block;
}
Block popStack()
{
    return stack[topOfStack--];
}
void printStack()
{
    printf("============Stack==========\n");
    for (int i = 0; i < topOfStack; i++)
        printf("x=%d y=%d\n", stack[topOfStack - i - 1].x, stack[topOfStack - i - 1].y);
}

int isWall(Block block)
{
    return map[block.y][block.x] == '+' ? 1 : 0;
}
int tryTraverseLeft()
{
    printf("try traverse left\t");
    Block nextBlock = car;
    int nextDirection;
    switch (direction)
    {
    case NORTH:
        nextDirection = WEST;
        nextBlock.x--;
        break;
    case EAST:
        nextDirection = NORTH;
        nextBlock.y--;
        break;
    case SOUTH:
        nextDirection = EAST;
        nextBlock.x++;
        break;
    case WEST:
        nextDirection = SOUTH;
        nextBlock.y++;
        break;
    }
    // printf("%d\n", nextDirection);
    if (!isWall(nextBlock))
    {
        car = nextBlock;
        direction = nextDirection;
        printf("done");
    }
    else
    {
        printf("failed");
    }

    printf("\n");
    return !isWall(nextBlock);
}

int tryTraverseFront()
{
    printf("try traverse front\t");
    Block nextBlock = car;
    int nextDirection;
    switch (direction)
    {
    case NORTH:
        nextDirection = NORTH;
        nextBlock.y--;
        break;
    case EAST:
        nextDirection = EAST;
        nextBlock.x++;
        break;
    case SOUTH:
        nextDirection = SOUTH;
        nextBlock.y++;
        break;
    case WEST:
        nextDirection = WEST;
        nextBlock.x--;
        break;
    }
    // printf("%d\n", nextDirection);
    if (!isWall(nextBlock))
    {
        car = nextBlock;
        direction = nextDirection;
        printf("done");
    }
    else
    {
        printf("failed");
    }
    printf("\n");
    return !isWall(nextBlock);
}

int tryTraverseRight()
{
    printf("try traverse right\t");
    Block nextBlock = car;
    int nextDirection;
    switch (direction)
    {
    case NORTH:
        nextDirection = EAST;
        nextBlock.x++;
        break;
    case EAST:
        nextDirection = SOUTH;
        nextBlock.y++;
        break;
    case SOUTH:
        nextDirection = WEST;
        nextBlock.x--;
        break;
    case WEST:
        nextDirection = NORTH;
        nextBlock.y--;
        break;
    }
    // printf("%d\n", nextDirection);
    if (!isWall(nextBlock))
    {
        car = nextBlock;
        direction = nextDirection;
        printf("done");
    }
    else
    {
        printf("failed");
    }

    printf("\n");
    return !isWall(nextBlock);
}
int tryTraverseBack()
{
    printf("try traverse BACK!\t");
    Block nextBlock = car;
    int nextDirection;
    switch (direction)
    {
    case NORTH:
        nextDirection = SOUTH;
        nextBlock.y++;
        break;
    case EAST:
        nextDirection = WEST;
        nextBlock.x--;
        break;
    case SOUTH:
        nextDirection = NORTH;
        nextBlock.y--;
        break;
    case WEST:
        nextDirection = EAST;
        nextBlock.x++;
        break;
    }
    // printf("%d\n", nextDirection);
    if (!isWall(nextBlock))
    {
        car = nextBlock;
        direction = nextDirection;
        printf("done");
    }
    else
    {
        printf("failed");
    }

    printf("\n");
    return !isWall(nextBlock);
}

int isGoal(Block block) { return block.x == goal.x && block.y == goal.y; }

int main()
{
    car = createBlock(9, 9, -1);
    goal = createBlock(5, 5, -1);
    direction = NORTH;

    loadMap();
    showMovementOnMap(car, direction);
    int nextState = 1;
    int state = 0;
    while (getch() != 'q')
    {
        switch (state)
        {
        case -1:
            
            nextState = 0;
            break;
        case 0:
            if (tryTraverseLeft())
                nextState = -1;
            else
                nextState = 1;
            break;
        case 1:
            if (tryTraverseFront())
                nextState = -1;
            else
                nextState = 2;
            break;
        case 2:
            if (tryTraverseRight())
                nextState = -1;
            else
                nextState = 3;
            break;
        case 3:
            if (tryTraverseBack())
                nextState = -1;
            else
                nextState = 4;
            break;
        }
        state = nextState;
        showMovementOnMap(car, direction);
        if (isGoal(car))
        {
            printf("GOAL!\n\n\n");
            return 0;
        }
    }

    return 0;
}