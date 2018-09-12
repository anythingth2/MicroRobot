#include <stdio.h>
#include <conio.h>
#define SIZE 11
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
FILE *mapFile;
char map[SIZE][SIZE + 1];
unsigned short int ONE_LEFT = 0b100;
unsigned short int ONE_FRONT = 0b010;
unsigned short int ONE_RIGHT = 0b001;
unsigned short int LEFT_FRONT = 0b110;
unsigned short int LEFT_RIGHT = 0b101;
unsigned short int RIGHT_FRONT = 0b011;
unsigned short int ALL = 0b111;

typedef struct _Block
{
    int x, y;
    int type;
} Block;
Block car;
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
        // printf("%s",map[i]);
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
    return map[block.y][block.x] == '+';
}
int tryTraverseLeft()
{
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
    }
    return isWall(nextBlock);
}

int tryTraverseFront()
{
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
    }
    return isWall(nextBlock);
}

int tryTraverseRight()
{
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
    }
    return isWall(nextBlock);
}
int main()
{
    car = createBlock(8, 8, -1);
    direction = NORTH;

    loadMap();
    showMovementOnMap(car, direction);

    do
    {
        switch (getch())
        {
        case 'a':
            tryTraverseLeft();
            break;
        case 'w':
            tryTraverseFront();
            break;
        case 'd':
            tryTraverseRight();
            break;
        case 'q':
            return 0;
        }
        showMovementOnMap(car, direction);
    } while (1);

    return 0;
}