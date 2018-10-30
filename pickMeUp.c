#include <stdio.h>
#include <conio.h>
#define SIZE 10
int simMap[SIZE][SIZE];
int map[SIZE][SIZE];
#define NOT_REACH_TYPE 0
#define EMPTY_TYPE 1
#define BLACK_TYPE 2
#define ORANGE_TYPE 3
#define DOUBLE_ORANGE_HORIZONTAL_TYPE 4
#define DOUBLE_ORANGE_VERTICAL_TYPE 5
const char SYMBOL_TYPE[] = {'-', '+', 'B', 'O'};
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

typedef struct _Box
{
    int x, y;
    int type;
} Box;
Box blackBox[4], orangeBox[2], doubleOrangeBox;
Box destinationBox[2], destinationDoubleBox;

int _abs(int v)
{
    return v > 0 ? v : -v;
}
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

    destinationBox[0].x = 7;
    destinationBox[0].y = 1;

    destinationBox[1].x = 2;
    destinationBox[1].y = 7;

    destinationDoubleBox.x = 6;
    destinationDoubleBox.y = 5;
    destinationDoubleBox.type = DOUBLE_ORANGE_VERTICAL_TYPE;
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

Box getNextBox(int direction)
{
    Box box;
    box.x = car.x;
    box.y = car.y;
    switch (direction)
    {
    case NORTH_DIRECION:
        box.y--;
        break;
    case EAST_DIRECION:
        box.x++;
        break;
    case SOUTH_DIRECION:
        box.y++;
        break;
    case WEST_DIRECION:
        box.x--;
        break;
    }
    return box;
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
        break;
    case 1:
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
float calculateDistance(Box fromBox, Box toBox)
{
    return (fromBox.x - toBox.x) * (fromBox.x - toBox.x) + (fromBox.y - toBox.y) * (fromBox.y - fromBox.y);
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
            // if (getch() == 'q')
            // {
            //     i = 5;
            //     break;
            // }
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
                // printf("found block at %d\n", foundBlockAt);
                // getch();

                moveForward(foundBlockAt - 1);
                map[car.y + verticalOffset][car.x + horizontalOffset] = readBlockType();
                // printMap();
                // getch();
                moveBack(foundBlockAt - 1);
                // printMap();
            }

            turn(RIGHT_HAND);
            // printSimMap();
            // printMap();
            // printf("-------------------------------------------------------\n");
        }
        turn(LEFT_HAND);

        // printSimMap();
        // printMap();
        // printf("-------------------------------------------------------\n");
    }
}
void findBlock()
{
    int countBlackBox = 0;
    int countOrangeBox = 0;

    int testMap[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            testMap[i][j] = map[i][j];

    for (int y = 0; y < SIZE; y++)
    {
        for (int x = 0; x < SIZE; x++)
        {
            if (testMap[y][x] == BLACK_TYPE)
            {
                blackBox[countBlackBox].x = x;
                blackBox[countBlackBox++].y = y;
                // printf("blackBox %d %d\n", x, y);
            }
            else if (testMap[y][x] == ORANGE_TYPE)
            {

                if (x < SIZE - 2 && y < SIZE - 2)
                {
                    if (testMap[y][x + 1] == ORANGE_TYPE)
                    {
                        testMap[y][x + 1] = NOT_REACH_TYPE;
                        doubleOrangeBox.x = x;
                        doubleOrangeBox.y = y;
                        doubleOrangeBox.type = DOUBLE_ORANGE_HORIZONTAL_TYPE;
                        // printf("double horizontal %d %d\n", x, y);
                        continue;
                    }
                    else if (testMap[y + 1][x] == ORANGE_TYPE)
                    {
                        testMap[y + 1][x] = NOT_REACH_TYPE;
                        doubleOrangeBox.x = x;
                        doubleOrangeBox.y = y;
                        doubleOrangeBox.type = DOUBLE_ORANGE_VERTICAL_TYPE;
                        // printf("double vertical %d %d\n", x, y);
                        continue;
                    }
                }

                orangeBox[countOrangeBox].x = x;
                orangeBox[countOrangeBox++].y = y;
                // printf("orangeBox %d %d\n", x, y);
            }
        }
    }
}

void moveJook(Box box)
{
    while (car.x != box.x || car.y != box.y)
    {
        if (getch() == 'q')
            return;
        Box nextBlock = getNextBox(car.direction);
        if (map[nextBlock.y][nextBlock.x] == NOT_REACH_TYPE)
        {
            moveForward(1);
        }
        else
        {
            int isHorizontal = car.direction == WEST_DIRECION || car.direction == EAST_DIRECION;
            int isStart = 0;
            int savedX = car.x, savedY = car.y;
            turn(RIGHT_HAND);
            while (1)
            {
                if (getch() == 'q')
                    return;
                if (isStart)
                {
                    // printf("[%d %d] [%d %d]\n", car.x, car.y, savedX, savedY);
                    if (isHorizontal)
                    {
                        if (car.y == savedY)
                            break;
                    }
                    else
                    {
                        if (car.x == savedX)
                            break;
                    }
                }
                if (!isStart)
                    isStart = 1;

                Box leftBox = getNextBox((car.direction - 1) & 0b11);
                Box frontBox = getNextBox(car.direction);
                Box rightBox = getNextBox((car.direction + 1) & 0b11);

                if (map[leftBox.y][leftBox.x] == NOT_REACH_TYPE)
                {
                    turn(LEFT_HAND);
                    moveForward(1);
                    printMap();
                    continue;
                }
                if (map[frontBox.y][frontBox.x] == NOT_REACH_TYPE)
                {

                    moveForward(1);
                    printMap();

                    continue;
                }
                if (map[rightBox.y][rightBox.x] == NOT_REACH_TYPE)
                {
                    turn(RIGHT_HAND);
                    moveForward(1);
                    printMap();
                    continue;
                }
            }
            turn(RIGHT_HAND);
        }
        printMap();
    }
}
void goPushBox()
{

    if (calculateDistance(orangeBox[0], destinationBox[0]) < calculateDistance(orangeBox[1], destinationBox[0]))
    {
        int pair[][2] = {{0, 0}, {1, 1}};
    }
    else
    {
        int pair[][2] = {{0, 1}, {1, 0}};
    }

    for (int i = 0; i < 2; i++)
    {
    }
}

int main()
{
    initMap();
    initCar();
    printSimMap();
    search();
    // findBlock();
    moveForward(4);
    turn(LEFT_HAND);
    Box box;
    box.x = 2;
    box.y = 5;
    moveJook(box);
}
