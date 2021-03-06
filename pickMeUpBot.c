#pragma config(Sensor, S1, rightColor, sensorEV3_Color)
#pragma config(Sensor, S2, ultSensor, sensorEV3_Ultrasonic)
#pragma config(Sensor, S3, boxColor, sensorLightActive)
#pragma config(Sensor, S4, leftColor, sensorEV3_Color)
#pragma config(Motor, motorA, rightMotor, tmotorEV3_Large, PIDControl, driveRight, encoder)
#pragma config(Motor, motorC, keepBox, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor, motorD, leftMotor, tmotorEV3_Large, PIDControl, driveLeft, encoder)

#define true 1
#define false 0
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#define SIZE 12

typedef struct _Car
{
    int x, y;
    int direction;
    int isPicking;
    int orangePickingIndex;
} Car;
Car car;
typedef struct _Box
{
    int x, y;
    int type;
} Box;

Box nextBox;

Box blackBoxs[4], orangeBoxs[2], doubleOrangeBox;
Box destinationBox[2], destinationDoubleBox;
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
#define BLOCK_SIZE 26.7
const char SYMBOL_DIRECTION[] = {'^', '>', 'v', '<'};

const int startX = 10, startY = 10, startDirection = NORTH_DIRECION;

float KpColor = 0.095;
float KiColor = 0.0;
//float KiColor = -0.00000001;
float err;
float sumErr;
int leftCol = 0;
int rightCol = 0;
int boxCol = 0;
int basepower = 50;
int frontult = 0;
int colorOfBox;
int color = 0;
int fullBlackThreshold = 10;

int map[SIZE][SIZE];

int _abs(int v)
{
    return v > 0 ? v : -v;
}
void printMap()
{
    int offsetY = 10;
    int offsetX = 15;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (i == car.y && j == car.x)
                displayStringAt(10 + offsetX * j, 150 - offsetY * i, "%c", SYMBOL_DIRECTION[car.direction]);
            else
                displayStringAt(10 + offsetX * j, 150 - offsetY * i, "%c", SYMBOL_TYPE[map[i][j]]);
        }
    }
}
void turn(int to)
{
    car.direction = (car.direction + to) & 0b11;
    // 336 or 350
    int angle = car.isPicking ? 350 : 340;
    //int offset = 50;
    int offset = 0;
    int _power = 50;
    if (to == RIGHT_HAND)
    {
        moveMotorTarget(leftMotor, angle, -_power);
        moveMotorTarget(rightMotor, angle, _power);
    }
    else
    {
        moveMotorTarget(leftMotor, angle, _power);
        moveMotorTarget(rightMotor, angle, -_power);
    }
    waitUntilMotorStop(leftMotor);
    waitUntilMotorStop(rightMotor);

    moveMotorTarget(leftMotor, offset, -_power + 2);
    moveMotorTarget(rightMotor, offset, -_power);

    waitUntilMotorStop(leftMotor);
    waitUntilMotorStop(rightMotor);


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
float calculateDistance(Box fromBox, Box toBox)
{
    return (fromBox.x - toBox.x) * (fromBox.x - toBox.x) + (fromBox.y - toBox.y) * (fromBox.y - toBox.y);
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
void moveForward(int block)
{
    int offset = 180;
    sumErr = 0;
    for (int i = 0; i < block; i++)
    {
        resetMotorEncoder(leftMotor);
        resetMotorEncoder(rightMotor);
        int distance = 0;
        do
        {
            leftCol = getColorReflected(leftColor);
            rightCol = getColorReflected(rightColor);
            err = leftCol - rightCol;
            sumErr += err;
            motor[leftMotor] = -(basepower + KpColor * err + KiColor * sumErr);
            motor[rightMotor] = -(basepower - KpColor * err - KiColor * sumErr);
            wait1Msec(10);
            //          distance = -((getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2);

        } while (!(leftCol <= fullBlackThreshold && rightCol <= fullBlackThreshold));
        while (leftCol <= fullBlackThreshold && rightCol <= fullBlackThreshold)
        {
            leftCol = getColorReflected(leftColor);
            rightCol = getColorReflected(rightColor);
        }
        resetMotorEncoder(leftMotor);
        resetMotorEncoder(rightMotor);
        do
        {
            leftCol = getColorReflected(leftColor);
            rightCol = getColorReflected(rightColor);
            err = leftCol - rightCol;
            sumErr += err;
            motor[leftMotor] = -(basepower + KpColor * err + KiColor * sumErr);
            motor[rightMotor] = -(basepower - KpColor * err - KiColor * sumErr);
            wait1Msec(10);
            distance = -((getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2);

        } while (distance < offset);

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

        playTone(500, 50);
    }
}

void moveBack(int block)
{
    if (block <= 0)
        return;
    turn(LEFT_HAND);
    turn(LEFT_HAND);
    moveForward(block);
    turn(RIGHT_HAND);
    turn(RIGHT_HAND);
}

void moveJook(int x, int y)
{
    while (car.x != x || car.y != y)
    {
       // if (!debug && getch() == 'q')
       //     return;
        getNextBox(car.direction);

        if (map[nextBox.y][nextBox.x] == NOT_REACH_TYPE)
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
               // if (!debug && getch() == 'q')
             //       return;
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
                Box leftBox;
                leftBox.x = nextBox.x;
                leftBox.y = nextBox.y;
                getNextBox(car.direction);
                Box frontBox;
                frontBox.x = nextBox.x;
                frontBox.y = nextBox.y;
                getNextBox((car.direction + 1) & 0b11);
                Box rightBox;
                rightBox.x = nextBox.x;
                rightBox.y = nextBox.y;

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
    return SensorValue[ultSensor] < BLOCK_SIZE * 5 + 7 ? (int)((SensorValue[ultSensor] - 7) / BLOCK_SIZE) : -1;
}
int readBlockType()
{
    return ORANGE_TYPE;
}


void getColor() // color = 1 is black - color = 2 is orange
{
    colorOfBox = SensorValue[S3];
    if (colorOfBox >= 15 && colorOfBox <= 30)
    {
        color = 1; // black
    }
    else if (colorOfBox >= 45 && colorOfBox <= 50)
    {
        color = 2; //orange
    }
}

int readColor()
{
		int initLength = SensorValue[S2];
		float k = 16;
    while (SensorValue[S2] >= 4)
    {
        leftCol = getColorReflected(leftColor);
        rightCol = getColorReflected(rightColor);
        err = leftCol - rightCol;
        sumErr += err;
        motor[leftMotor] = -(basepower - 30 + KpColor * err + KiColor * sumErr);
        motor[rightMotor] = -(basepower - 30 - KpColor * err - KiColor * sumErr);
        wait1Msec(10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    wait1Msec(1000);

    colorOfBox = SensorValue[S3];

    moveMotorTarget(leftMotor, initLength*k, basepower - 30 + 2);
    moveMotorTarget(rightMotor, initLength*k, basepower - 30);
   	waitUntilMotorStop(leftMotor);
   	waitUntilMotorStop(rightMotor);

    return colorOfBox < 33 ? BLACK_TYPE : ORANGE_TYPE;
}
int isFound()
{
    return SensorValue[S2] < 19; // ultrasonic
}

void searchNew()
{
    moveForward(1);
    turn(LEFT_HAND);
    printMap();
    int isLeft = 1;
    int isOut = 0;
    int lastX = -1;
    int lastY = -1;
    Box tempBox;
    for (int i = 2; i < SIZE - 2; i++)
    {
        while (!((isLeft && car.x == 1) || (!isLeft && car.x == SIZE - 2)) || isOut)
        {

            if (isOut)
            {
                turn(LEFT_HAND);
                if (isFound())
                {
                    getNextBox(car.direction);
                    tempBox = nextBox;
                    map[nextBox.y][nextBox.x] = readColor();
                    turn(RIGHT_HAND);
                    if (isFound())
                    {
                        getNextBox(car.direction);
                        tempBox = nextBox;
                        map[nextBox.y][nextBox.x] = readColor();
                        turn(RIGHT_HAND);
                        if (isFound())
                        {
                            getNextBox(car.direction);
                            tempBox = nextBox;
                            map[nextBox.y][nextBox.x] = readColor();
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
            }
            // printf("==========\n");
            // printSimMap();
            printMap();
        }

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
}


void pickUp(int orangeIndex)
{
    motor[leftMotor] = -20;
    motor[rightMotor] = -20;
    wait1Msec(700);

    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    wait1Msec(700);

    moveMotorTarget(motorC, 2400, 80);
    waitUntilMotorStop(motorC);

    motor[leftMotor] = 20;
    motor[rightMotor] = 20;
    wait1Msec(1800);

    moveMotorTarget(motorC, -700, -80);
    waitUntilMotorStop(motorC);

    car.isPicking = 1;
    car.orangePickingIndex = orangeIndex;
}
void drop()
{
    motor[leftMotor] = 20;
    motor[rightMotor] = 20;
    wait1Msec(700);

    moveMotorTarget(motorC, 700, 80);
    waitUntilMotorStop(motorC);

    motor[leftMotor] = -20;
    motor[rightMotor] = -20;
    wait1Msec(500);

    moveMotorTarget(motorC, -2400, -80);
    waitUntilMotorStop(motorC);

       car.isPicking = 0;
    car.orangePickingIndex = -1;
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
        Box orange;
        orange.x = orangeBoxs[orangeBoxIndex].x;
        orange.y = orangeBoxs[orangeBoxIndex].y;
        Box destination;
        destination.x = destinationBox[destinationBoxIndex].x;
        destination.y = destinationBox[destinationBoxIndex].y;

        int borderSide;
        //printf("from [%d,%d] to [%d,%d]\n", orange.x, orange.y, destination.x, destination.y);
        //move block to border
        if (isClearPath(orange.x, orange.y, EAST_DIRECION))
        {
           // printf("EAST\n");
            moveForward(SIZE - orange.y - 1);
            turn(LEFT_HAND);
            moveForward(SIZE - orange.x - 2);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            pickUp(orangeBoxIndex);
            moveForward(SIZE - car.x - 1);
            turn(LEFT_HAND);
            turn(LEFT_HAND);
            // moveForward(1);
            borderSide = EAST_DIRECION;
        }
        else if (isClearPath(orange.x, orange.y, SOUTH_DIRECION))
        {
           // printf("SOUTH\n");
            turn(LEFT_HAND);
            moveForward(SIZE - orange.x - 2);
            turn(RIGHT_HAND);
            moveForward(SIZE - orange.y - 2);
            turn(LEFT_HAND);
            turn(LEFT_HAND);
            pickUp(orangeBoxIndex);
            moveForward(SIZE - car.y - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            // moveForward(1);

            borderSide = SOUTH_DIRECION;
        }
        else if (isClearPath(orange.x, orange.y, NORTH_DIRECION))
        {
           // printf("NORTH\n");
            moveForward(SIZE - 2);
            turn(LEFT_HAND);
            moveForward(SIZE - orange.x - 2);
            turn(LEFT_HAND);
            moveForward(orange.y - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            pickUp(orangeBoxIndex);
            moveForward(orange.y - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            // moveForward(1);
            borderSide = NORTH_DIRECION;
        }
        else if (isClearPath(orange.x, orange.y, WEST_DIRECION))
        {
          //  printf("WEST\n");
            turn(LEFT_HAND);
            moveForward(SIZE - 1);
            turn(RIGHT_HAND);
            moveForward(SIZE - orange.y - 1);
            turn(RIGHT_HAND);
            moveForward(orange.x - 1);
            turn(LEFT_HAND);
            turn(LEFT_HAND);
            pickUp(orangeBoxIndex);
            moveForward(orange.x - 1);
            turn(RIGHT_HAND);
            turn(RIGHT_HAND);
            // moveForward(1);
            borderSide = WEST_DIRECION;
        }
        else
        {
           // printf("fialed\n");
            borderSide = -1;
        }
       // printf("move to border\n");
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
         //   printf("%d %f\n", isHasDoors[i], distanceEachDoor[i]);
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
        drop();

        printMap();

        turnTo(EAST_DIRECION);
        moveJook(SIZE - 2, car.y);
        turnTo(SOUTH_DIRECION);
        moveJook(SIZE - 2, SIZE - 2);
        turnTo(NORTH_DIRECION);
        printMap();
    }
}
void initMap()
{
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
        {
            map[i][j] = NOT_REACH_TYPE;
        }

    destinationBox[0].x = 8;
    destinationBox[0].y = 2;

    destinationBox[1].x = 3;
    destinationBox[1].y = 8;

    destinationDoubleBox.x = 7;
    destinationDoubleBox.y = 6;
    destinationDoubleBox.type = DOUBLE_ORANGE_VERTICAL_TYPE;
}
void mock(){

	map[3][3] = ORANGE_TYPE;
	map[2][6] = ORANGE_TYPE;

	map[7][6] = ORANGE_TYPE;
	map[8][6] = ORANGE_TYPE;

	map[2][7] = BLACK_TYPE;
	map[4][5] = BLACK_TYPE;
	map[6][8] = BLACK_TYPE;
	map[7][3] = BLACK_TYPE;
}
void initCar()
{
    car.x = startX;
    car.y = startY;
    car.direction = startDirection;
}

task main()
{

    initMap();
    initCar();
    turn(LEFT_HAND);
    turn(LEFT_HAND);
    pickUp(0);
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    wait1Msec(1000);
    drop();
    //searchNew();

    //findBlock();
    //goPushBox();
}
