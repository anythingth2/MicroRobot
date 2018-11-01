#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#define SIZE 12
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
int debug = 1;
typedef struct _Car
{
    int x, y;
    int direction;
    int isPicking;
    int orangePickingIndex;
} Car;
Car car;
const int startX = 10, startY = 10, startDirection = NORTH_DIRECION;

typedef struct _Box
{
    int x, y;
    int type;
} Box;
Box blackBoxs[4], orangeBoxs[2], doubleOrangeBox;
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

    // simMap[4][4] = ORANGE_TYPE;
    // simMap[6][8] = ORANGE_TYPE;
    // simMap[6][7] = ORANGE_TYPE;
    // simMap[8][6] = ORANGE_TYPE;

    // simMap[5][3] = BLACK_TYPE;
    // simMap[5][5] = BLACK_TYPE;
    // simMap[8][4] = BLACK_TYPE;
    // simMap[4][3] = BLACK_TYPE;

    destinationBox[0].x = 8;
    destinationBox[0].y = 2;

    destinationBox[1].x = 3;
    destinationBox[1].y = 8;

    destinationDoubleBox.x = 7;
    destinationDoubleBox.y = 6;
    destinationDoubleBox.type = DOUBLE_ORANGE_VERTICAL_TYPE;
}

void randomMap()
{
    srand(time(NULL));
    int start = 2;
    int stop = SIZE - 5;
    int x = (rand()) % (stop) + start;
    int y = (rand()) % (stop) + start;
    for (int i = 0; i < 2; i++)
    {
        x = (rand()) % (stop) + start;
        y = (rand()) % (stop) + start;
        simMap[y][x] = ORANGE_TYPE;
        printf("ORANGE\t[%d,%d]\n", x, y);
    }
    for (int i = 0; i < 4; i++)
    {
        x = (rand()) % (stop) + start;
        y = (rand()) % (stop) + start;
        simMap[y][x] = BLACK_TYPE;

        printf("Black\t[%d,%d]\n", x, y);
    }

    x = (rand()) % (stop) + start;
    y = (rand()) % (stop) + start;
    simMap[y][x] = ORANGE_TYPE;
    if (rand() % 2)
        simMap[y][x + 1] = ORANGE_TYPE;
    else
        simMap[y + 1][x] = ORANGE_TYPE;
    printf("DOUBLE ORANGE \t[%d,%d]\n", x, y);
}
void initCar()
{
    car.x = startX;
    car.y = startY;
    car.direction = startDirection;
    car.isPicking = 0;
    car.orangePickingIndex = -1;
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
    printf("car [%d,%d]\n", car.x, car.y);
    printf("\n");
}
Box nextBox;
void getNextBox(int direction)
{
    nextBox.x = car.x;
    nextBox.y = car.y;
    switch (direction)
    {
    case NORTH_DIRECION:
        nextBox.y--;
        break;
    case EAST_DIRECION:
        nextBox.x++;
        break;
    case SOUTH_DIRECION:
        nextBox.y++;
        break;
    case WEST_DIRECION:
        nextBox.x--;
        break;
    }
}
void moveForward(int block)
{
    for (int i = 0; i < block; i++)
    {
        int horizontalCommand = 0, verticalCommand = 0;

        switch (car.direction)
        {
        case 0:
            // car.y--;
            verticalCommand = -1;
            break;
        case 1:
            // car.x++;
            horizontalCommand = 1;
            break;
        case 2:
            // car.y++;
            verticalCommand = 1;
            break;
        case 3:
            // car.x--;
            horizontalCommand = -1;
            break;
        }
        car.x += horizontalCommand;
        car.y += verticalCommand;
        if (car.isPicking)
        {
            map[orangeBoxs[car.orangePickingIndex].y][orangeBoxs[car.orangePickingIndex].x] = NOT_REACH_TYPE;
            orangeBoxs[car.orangePickingIndex].x += horizontalCommand;
            orangeBoxs[car.orangePickingIndex].y += verticalCommand;
            map[orangeBoxs[car.orangePickingIndex].y][orangeBoxs[car.orangePickingIndex].x] = ORANGE_TYPE;
        }
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
    if (car.isPicking)
    {
        int x = car.x, y = car.y;
        map[orangeBoxs[car.orangePickingIndex].y][orangeBoxs[car.orangePickingIndex].x] = NOT_REACH_TYPE;
        switch (car.direction)
        {
        case NORTH_DIRECION:
            y++;
            break;
        case EAST_DIRECION:
            x--;
            break;
        case SOUTH_DIRECION:
            y--;
            break;
        case WEST_DIRECION:
            x++;
            break;
        }

        orangeBoxs[car.orangePickingIndex].x = x;
        orangeBoxs[car.orangePickingIndex].y = y;

        map[orangeBoxs[car.orangePickingIndex].y][orangeBoxs[car.orangePickingIndex].x] = ORANGE_TYPE;
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

void pick(int orangeIndex)
{
    car.isPicking = 1;
    car.orangePickingIndex = orangeIndex;

    printf("picked\n");
    Box box = orangeBoxs[orangeIndex];

    map[box.y][box.x] = NOT_REACH_TYPE;
}

void unPick()
{
    car.isPicking = 0;
    car.orangePickingIndex = -1;
}

float calculateDistance(Box fromBox, Box toBox)
{
    return (fromBox.x - toBox.x) * (fromBox.x - toBox.x) + (fromBox.y - toBox.y) * (fromBox.y - toBox.y);
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
void moveJook(int x, int y)
{
    while (car.x != x || car.y != y)
    {
        if (!debug && getch() == 'q')
            return;
        getNextBox(car.direction);
        Box nextBlock = nextBox;
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
                if (!debug && getch() == 'q')
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
                getNextBox((car.direction - 1) & 0b11);
                Box leftBox = nextBox;
                getNextBox(car.direction);
                Box frontBox = nextBox;
                getNextBox((car.direction + 1) & 0b11);
                Box rightBox = nextBox;

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
        // printMap();
    }
}
void search()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < SIZE - 1; j++)
        {
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
int isFound()
{
    return readBlockType() != NOT_REACH_TYPE;
}
int readColor()
{
    return readBlockType();
}
void searchNew()
{

    moveForward(1);
    turn(LEFT_HAND);
    // printMap();
    int isLeft = 1;
    int isOut = 0;
    int lastX = -1;
    int lastY = -1;
    Box tempBox;
    for (int i = 2; i < SIZE - 2; i++)
    {
        while (!((isLeft && car.x == 1) || (!isLeft && car.x == SIZE - 2)) || isOut)
        {
            if (!debug && getch() == 'q')
                return;
            if (isOut)
            {
                // printf("out\n");

                turn(LEFT_HAND);
                if (isFound())
                {
                    getNextBox(car.direction);
                    tempBox = nextBox;
                    map[tempBox.y][tempBox.x] = readColor();
                    turn(RIGHT_HAND);
                    if (isFound())
                    {
                        getNextBox(car.direction);
                        tempBox = nextBox;
                        map[tempBox.y][tempBox.x] = readColor();
                        turn(RIGHT_HAND);
                        if (isFound())
                        {
                            getNextBox(car.direction);
                            tempBox = nextBox;
                            map[tempBox.y][tempBox.x] = readColor();
                        }
                        else
                        {
                            moveForward(1);
                        }
                    }
                    else
                    {
                        moveForward(1);
                    }
                }
                else
                {
                    moveForward(1);
                }
            }
            else if (isFound())
            {
                isOut = 1;
                lastX = car.x;
                lastY = car.y;
                turnTo(isLeft ? NORTH_DIRECION : SOUTH_DIRECION);
            }
            else
            {
                moveForward(1);
            }

            if (car.y == lastY && ((isLeft && car.x < lastX) || (!isLeft && car.x > lastX)))
            {
                turnTo(isLeft ? WEST_DIRECION : EAST_DIRECION);
                isOut = 0;
                // printf("come back\n");
            }
            // printf("==========\n");
            // printSimMap();
            // printMap();
        }
        if (!debug && getch() == 'q')
            return;

        if (isLeft)
        {
            turn(RIGHT_HAND);
            moveForward(1);
            turn(RIGHT_HAND);
        }
        else
        {

            turn(LEFT_HAND);
            moveForward(1);
            turn(LEFT_HAND);
        }

        isLeft = !isLeft;
        // printf("==========\n");
        // printSimMap();
        // printMap();
    }
    turnTo(SOUTH_DIRECION);
    moveForward(SIZE - 3);
    turnTo(NORTH_DIRECION);
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
                blackBoxs[countBlackBox].x = x;
                blackBoxs[countBlackBox++].y = y;
                // printf("blackBoxs %d %d\n", x, y);
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

                orangeBoxs[countOrangeBox].x = x;
                orangeBoxs[countOrangeBox++].y = y;
                // printf("orangeBoxs %d %d\n", x, y);
            }
        }
    }
}

int isClearPath(int x, int y, int direction)
{
    int isClear = 1;
    while (x > 0 && x < SIZE && y > 0 && y < SIZE)
    {
        switch (direction)
        {
        case NORTH_DIRECION:
            y--;
            break;
        case EAST_DIRECION:
            x++;
            break;
        case SOUTH_DIRECION:
            y++;
            break;
        case WEST_DIRECION:
            x--;
            break;
        }
        if (map[y][x] != NOT_REACH_TYPE)
        {
            isClear = 0;

            // printf("%c [%d %d]\n",SYMBOL_DIRECTION[direction],x,y);
            break;
        }
    }
    return isClear;
}
void goPushBox()
{
    int pair[2][2];

    if (calculateDistance(orangeBoxs[0], destinationBox[0]) < calculateDistance(orangeBoxs[1], destinationBox[0]))
    {
        pair[0][0] = 0;
        pair[0][1] = 0;
        pair[1][0] = 1;
        pair[1][1] = 1;
    }
    else
    {

        pair[0][0] = 0;
        pair[0][1] = 1;
        pair[1][0] = 1;
        pair[1][1] = 0;
    }

    for (int i = 0; i < 2; i++)
    {

        int orangeBoxIndex = pair[i][0];
        int destinationBoxIndex = pair[i][1];
        Box orange = orangeBoxs[orangeBoxIndex];
        Box destination = destinationBox[destinationBoxIndex];
        int borderSide;
        printf("from [%d,%d] to [%d,%d]\n", orange.x, orange.y, destination.x, destination.y);
        //move block to border
        if (isClearPath(orange.x, orange.y, EAST_DIRECION))
        {
            printf("EAST\n");
            moveForward(SIZE - orange.y - 1);
            turn(LEFT_HAND);
            moveForward(SIZE - orange.x - 2);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            pick(orangeBoxIndex);
            moveForward(SIZE - car.x - 1);
            turn(LEFT_HAND);
            turn(LEFT_HAND);
            // moveForward(1);
            borderSide = EAST_DIRECION;
        }
        else if (isClearPath(orange.x, orange.y, SOUTH_DIRECION))
        {
            printf("SOUTH\n");
            turn(LEFT_HAND);
            moveForward(SIZE - orange.x - 2);
            turn(RIGHT_HAND);
            moveForward(SIZE - orange.y - 2);
            turn(LEFT_HAND);
            turn(LEFT_HAND);
            pick(orangeBoxIndex);
            moveForward(SIZE - car.y - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            // moveForward(1);

            borderSide = SOUTH_DIRECION;
        }
        else if (isClearPath(orange.x, orange.y, NORTH_DIRECION))
        {
            printf("NORTH\n");
            moveForward(SIZE - 2);
            turn(LEFT_HAND);
            moveForward(SIZE - orange.x - 2);
            turn(LEFT_HAND);
            moveForward(orange.y - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            pick(orangeBoxIndex);
            moveForward(orange.y - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            // moveForward(1);
            borderSide = NORTH_DIRECION;
        }
        else if (isClearPath(orange.x, orange.y, WEST_DIRECION))
        {
            printf("WEST\n");
            turn(LEFT_HAND);
            moveForward(SIZE - 1);
            turn(RIGHT_HAND);
            moveForward(SIZE - orange.y - 1);
            turn(RIGHT_HAND);
            moveForward(orange.x - 1);
            turn(LEFT_HAND);
            turn(LEFT_HAND);
            pick(orangeBoxIndex);
            moveForward(orange.x - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            // moveForward(1);
            borderSide = WEST_DIRECION;
        }
        else
        {
            printf("fialed\n");
            borderSide = -1;
        }
        printf("move to border\n");
        printMap();
        //find shortest door
        int isHasDoors[4];

        int destinationDoors[4][2] = {{destination.x, 1},
                                      {SIZE - 2, destination.y},
                                      {destination.x, SIZE - 2},
                                      {1, destination.y}};
        float distanceEachDoor[4];

        for (int i = 0; i < 4; i++)
        {
            isHasDoors[i] = isClearPath(destination.x, destination.y, i);
            Box fromBox;
            fromBox.x = car.x;
            fromBox.y = car.y;
            Box toBox;
            toBox.x = destinationDoors[i][0];
            toBox.y = destinationDoors[i][1];
            distanceEachDoor[i] = isHasDoors[i] ? calculateDistance(fromBox, toBox) : 9999.0;
            printf("%d %f\n", isHasDoors[i], distanceEachDoor[i]);
        }
        for (int i = 0; i < 4; i++)
        {
            for (int j = i; j < 4; j++)
            {
                if (distanceEachDoor[i] > distanceEachDoor[j])
                {
                    int temp = destinationDoors[i][0];
                    destinationDoors[i][0] = destinationDoors[j][0];
                    destinationDoors[j][0] = temp;

                    temp = destinationDoors[i][1];
                    destinationDoors[i][1] = destinationDoors[j][1];
                    destinationDoors[j][1] = temp;
                }
            }
        }

        int diffHorizontal = destination.x - car.x;
        int diffVertical = destination.y - car.y;

        //go to shortest door
        if (borderSide == NORTH_DIRECION || borderSide == SOUTH_DIRECION)
        {
            int lastDirection = car.direction;
            if (diffHorizontal > 0)
                turnTo(EAST_DIRECION);
            else if (diffHorizontal < 0)
                turnTo(WEST_DIRECION);
            moveForward(_abs(diffHorizontal));
            if (diffHorizontal > 0)
                turnTo(lastDirection);
            else if (diffHorizontal < 0)
                turnTo(lastDirection);
            moveForward(_abs(diffVertical) + 1);
        }
        else
        {
            int lastDirection = car.direction;
            if (diffVertical > 0)
                turnTo(SOUTH_DIRECION);
            else if (diffVertical < 0)
                turnTo(NORTH_DIRECION);
            moveForward(_abs(diffVertical));
            if (diffVertical > 0)
                turnTo(lastDirection);
            else if (diffVertical < 0)
                turnTo(lastDirection);
            moveForward(_abs(diffHorizontal) + 1);
        }
        //drop
        unPick();

        printMap();

        turnTo(EAST_DIRECION);
        moveJook(SIZE - 2, car.y);
        turnTo(SOUTH_DIRECION);
        moveJook(SIZE - 2, SIZE - 2);
        turnTo(NORTH_DIRECION);
        printMap();
    }
}

int main()
{
    debug = 1;
    initMap();
    randomMap();
    initCar();

    printSimMap();
    printMap();
    searchNew();

    findBlock();
    goPushBox();

    printf("*********************\n\n\n");
    printSimMap();
    printMap();
    return 0;
}
