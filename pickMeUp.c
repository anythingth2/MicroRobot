#include <stdio.h>
#include <conio.h>
#define SIZE 10
int simMap[SIZE][SIZE];
int map[SIZE][SIZE];
#define NOT_REACH_TYPE 0
#define EMPTY_TYPE 1
#define ORANGE_TYPE 2
#define BLACK_TYPE 3
const char SYMBOL_TYPE[] = {'-', '+', 'O', 'B'};
#define NORTH_DIRECION 0
#define EAST_DIRECION 1
#define SOUTH_DIRECION 2
#define WEST_DIRECION 3
#define LEFT_HAND -1
#define RIGHT_HAND 1
const char SYMBOL_DIRECTION[] = {'^', '>', 'v', '<'};

typedef struct _Car
{
    int x, y;
    int direction;
} Car;
Car car;
const int startX = 9, startY = 9, startDirection = NORTH_DIRECION;

void initMap()
{
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
        {
            simMap[i][j] = NOT_REACH_TYPE;
            map[i][j] = NOT_REACH_TYPE;
        }
    simMap[3][3] = ORANGE_TYPE;
    simMap[5][7] = ORANGE_TYPE;
    simMap[5][8] = ORANGE_TYPE;
    simMap[7][3] = ORANGE_TYPE;

    simMap[1][2] = BLACK_TYPE;
    simMap[4][6] = BLACK_TYPE;
    simMap[7][5] = BLACK_TYPE;
    simMap[2][5] = BLACK_TYPE;
}

void initCar()
{
    car.x = startX;
    car.y = startY;
    car.direction = startDirection;
}

void printSimMap()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            printf("%c ", SYMBOL_TYPE[simMap[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
}
void printMap()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (i == car.y && j == car.x)
                printf("%c ", SYMBOL_DIRECTION[car.direction]);
            else
                printf("%c ", SYMBOL_TYPE[map[i][j]]);
        }
        printf("\n");
    }
    printf("\n");
}
void moveForward(int block)
{
    for (int i = 0; i < block; i++)
        switch (car.direction)
        {
        case 0:
            car.y--;
            break;
        case 1:
            car.x++;
            break;
        case 2:
            car.y++;
            break;
        case 3:
            car.x--;
            break;
        }
}

void moveBack(int block)
{
    for (int i = 0; i < block; i++)
        switch (car.direction)
        {
        case 0:
            car.y++;
            break;
        case 1:
            car.x--;
            break;
        case 2:
            car.y--;
            break;
        case 3:
            car.x++;
            break;
        }
}

void turn(int to)
{
    car.direction = (car.direction + to) & 0b11;
    switch (to)
    {
    case -1:
        // car.direction =
        break;
    case 1:
        // turnRightWithGyro();
        break;
    }
}

void turnTo(int direction)
{
    while (car.direction != direction)
    {
        if (car.direction == 0b00 && direction == 0b11)
        {
            turn(RIGHT_HAND);
            continue;
        }
        if (car.direction == 0b11 && direction == 0b00)
        {
            turn(RIGHT_HAND);
            continue;
        }
        if (car.direction < direction)
            turn(RIGHT_HAND);
        else
            turn(LEFT_HAND);
    }
}

int scanLineBlock()
{
    int found = -1;

    int horizontalCommand = 0;
    int verticalCommand = 0;
    switch (car.direction)
    {
    case NORTH_DIRECION:
        verticalCommand = -1;
        break;
    case EAST_DIRECION:
        horizontalCommand = 1;
        break;
    case SOUTH_DIRECION:
        verticalCommand = 1;
        break;
    case WEST_DIRECION:
        horizontalCommand = -1;
        break;
    }
    for (int i = 1; i < SIZE / 2 + 1; i++)
    {
        if (simMap[car.y + verticalCommand * i][car.x + horizontalCommand * i] != NOT_REACH_TYPE)
        {
            found = i;
            break;
        }
    }
    return found;
}

int readBlockType()
{
    int horizontalCommand = 0;
    int verticalCommand = 0;
    switch (car.direction)
    {
    case NORTH_DIRECION:
        verticalCommand = -1;
        break;
    case EAST_DIRECION:
        horizontalCommand = 1;
        break;
    case SOUTH_DIRECION:
        verticalCommand = 1;
        break;
    case WEST_DIRECION:
        horizontalCommand = -1;
        break;
    }
    // printf("check at %d %d\n", car.x + horizontalCommand, car.y + verticalCommand);
    return simMap[car.y + verticalCommand][car.x + horizontalCommand];
}
void search()
{
    for (int i = 0; i < 4; i++)
    {

        for (int j = 0; j < SIZE - 1; j++)
        {
            if (getch() == 'q')
            {
                i = 5;
                break;
            }
            moveForward(1);

            turn(LEFT_HAND);

            int foundBlockAt = scanLineBlock();
            if (foundBlockAt != -1)
            {
                int horizontalOffset = 0;
                int verticalOffset = 0;
                switch (car.direction)
                {
                case NORTH_DIRECION:
                    verticalOffset = -1;
                    break;
                case EAST_DIRECION:
                    horizontalOffset = 1;
                    break;
                case SOUTH_DIRECION:
                    verticalOffset = 1;
                    break;
                case WEST_DIRECION:
                    horizontalOffset = -1;
                    break;
                }
                printf("found block at %d\n", foundBlockAt);
                getch();

                moveForward(foundBlockAt - 1);
                map[car.y + verticalOffset][car.x + horizontalOffset] = readBlockType();
                printMap();
                getch();
                moveBack(foundBlockAt - 1);
                printMap();
            }

            turn(RIGHT_HAND);
            printSimMap();
            printMap();
            printf("-------------------------------------------------------\n");
        }
        turn(LEFT_HAND);

        printSimMap();
        printMap();
        printf("-------------------------------------------------------\n");
    }
}

int main()
{
    initMap();
    initCar();
    printSimMap();
    search();
}