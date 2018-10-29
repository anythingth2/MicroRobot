#pragma config(Sensor, S1,     leftUlt,        sensorEV3_Ultrasonic)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro)
#pragma config(Sensor, S3,     frontUlt,       sensorSONAR)
#pragma config(Sensor, S4,     rightUlt,       sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)
#pragma config(Motor,  motorC,          ctrlMotor,     tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorD,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define LEFT_ULTRA_SERVO 0
#define RIGHT_ULTRA_SERVO 1
int currentUltraServo = 0; //left = 0; front = 1; right = 2;
int distance = 0;
int lastGyroErr = 0;
int gyroErr = 0;
int sumGyroErr = 0;
int basePower = 20;
int defaultGyro = 0;
float KpGyro = 1.82;
float KiGyro = 0.039; // sum error
float KdGyro = 2.75;  // recent - last error
float KpUltra = 0.21;
float KiUltra = 0.000007;
float outlierGap = 20;
float expectedGap = 6;
float gapErrorLeft, gapErrorRight, sumGapErrorLeft, sumGapErrorRight, lastGapError = 0;
float gapLeft, gapRight;
float gap;
float gapError;
int leftB = 0;
int rightB = 0;
int forwardB = 0;
int carX, carY, carDirection = 0;
int startX = 8, startY = 8, startDirection = 0;

int endX = 4, endY = 4;
int wayStack[81];
int topOfStack = 0;
int countAvaliableBlock = 0;
void moveWithUltra();
void moveBackWithUltra();
void moveNormal();
void moveBackNormal();
void turnleftWithGyro();
void turnRightWithGyro();
void turn(int to);
void turnTo(int direction);
void pushWayStack(int direction)
{
    wayStack[topOfStack++] = direction;
}
int popWayStack()
{
    return wayStack[--topOfStack];
}
int absolute(int v)
{
    return v >= 0 ? v : -v;
}

// SHORTEST PATH================================
#define CHAR_WIDTH 15
#define CHAR_HEIGHT 5
#define LCD_HEIGHT 125
#define SIZE 9
int map[SIZE][SIZE];
typedef struct _Block
{
    int _type;
    int direction;
    int fromX, fromY;
    int isNewEdge;
    int isTraveled;
    int isUpdating;
    int carWentedDirection;
} Block;

Block pathMap[SIZE][SIZE];
Block searchMap[SIZE][SIZE];

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

void initSearchMap()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            searchMap[i][j].direction = -1;
            searchMap[i][j].carWentedDirection = -1;
            searchMap[i][j]._type = 0;
        }
    }
    searchMap[startY][startX]._type |= 0b1000;
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
                displayStringAt(j * CHAR_WIDTH, LCD_HEIGHT - i * CHAR_HEIGHT, "%c", getDirectionSymbol(pathMap[i][j].direction));
            }
            else
            {
                displayStringAt(j * CHAR_WIDTH, LCD_HEIGHT - i * CHAR_HEIGHT, " ");
            }
        }
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

                displayStringAt(0, LCD_HEIGHT - 70, "[%d,%d %c %d,%d] ", pathMap[i][j].fromX, pathMap[i][j].fromY, ch, j, i);
            }
            else
            {

                displayStringAt(0, LCD_HEIGHT - 70, "                ");
            }
        }
    }
}
void showSearchStep()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (searchMap[i][j].carWentedDirection != -1)
            {
                displayStringAt(j * CHAR_WIDTH, LCD_HEIGHT - (SIZE - i) * CHAR_HEIGHT, " %d", searchMap[i][j].carWentedDirection);
            }
            else
            {
                displayStringAt(j * CHAR_WIDTH, LCD_HEIGHT - (SIZE - i) * CHAR_HEIGHT, "%d", -1);
            }
        }
    }
    displayStringAt(0, LCD_HEIGHT - 62, "direction %d\t%c\n", carDirection, getDirectionSymbol(carDirection));
}
void showSearchDirectionStep()
{

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            displayStringAt(j * CHAR_WIDTH, LCD_HEIGHT - i * CHAR_HEIGHT, "%c", getDirectionSymbol(searchMap[i][j].direction));
        }
        // printf("\n");
    }
}

void showSearchCar()
{

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (i == carY && j == carX)
            {
                displayStringAt(j * CHAR_WIDTH, LCD_HEIGHT - i * CHAR_HEIGHT, "%c", getDirectionSymbol(carDirection));
            }
            else
            {
            }
        }
        // printf("\n");
    }
}

void moveForward()
{
    switch (carDirection)
    {
    case 0:
        carY--;
        break;
    case 1:
        carX++;
        break;
    case 2:
        carY++;
        break;
    case 3:
        carX--;
        break;
    }
    moveWithUltra();
}

void moveBack()
{
    switch (carDirection)
    {
    case 0:
        carY++;
        break;
    case 1:
        carX--;
        break;
    case 2:
        carY--;
        break;
    case 3:
        carX++;
        break;
    }
    moveBackWithUltra();
}
void turn(int to)
{
    carDirection = (carDirection + to) & 0b11;
    switch (to)
    {
    case -1:
        turnleftWithGyro();
        break;
    case 1:
        turnRightWithGyro();
        break;
    }
    displayStringAt(60, LCD_HEIGHT - 92, "turn %d\n", to);
}

void turnTo(int direction)
{
    displayStringAt(0, LCD_HEIGHT - 100, "turnTo from %d to %d\n", carDirection, direction);
    while (carDirection != direction)
    {
        if (carDirection == 0b00 && direction == 0b11)
        {
            turn(-1);
            continue;
        }
        if (carDirection == 0b11 && direction == 0b00)
        {
            turn(1);
            continue;
        }
        if (carDirection < direction)
            turn(1);
        else
            turn(-1);
    }
    displayStringAt(80, LCD_HEIGHT - 100, "result %d -> %d\n", carDirection, direction);
}
int checkWall(int turn)
{
    float gap;
    switch (turn)
    {
    case -1:
        gap = SensorValue[leftUlt];
        break;
    case 0:
        gap = SensorValue[frontUlt];
        break;
    case 1:
        gap = SensorValue[rightUlt];
        break;
    }

    return gap < outlierGap;
}

void search()
{

    int step = 0;
    int isStarted = 0;
    searchMap[carY][carX].direction = carDirection;
    while (1)
    {
        // printf("step %d \t  ==========================================\n", step);
        //while(!SensorValue[button]);

        if (searchMap[carY][carX].carWentedDirection < 2)
        {
        		if(searchMap[carY][carY].carWentedDirection == -1){
        				countAvaliableBlock++;
        		}
            searchMap[carY][carX].carWentedDirection++;

            if (checkWall(searchMap[carY][carX].carWentedDirection - 1))
            {
                int wall;
                switch ((carDirection + (searchMap[carY][carX].carWentedDirection - 1)) & 0b11)
                {
                case 0:
                    wall = 0b0010;
                    break;
                case 1:
                    wall = 0b0001;
                    break;
                case 2:
                    wall = 0b1000;
                    break;
                case 3:
                    wall = 0b0100;
                    break;
                }
                searchMap[carY][carX]._type |= wall;
                eraseDisplay();
                displayStringAt(0, LCD_HEIGHT - 80, "check %d not move\n", searchMap[carY][carX].carWentedDirection);
                showSearchStep();
                // showSearchDirectionStep();
                // showSearchCar();
            }
            else
            {
                if (carX == startX && carY == startY && searchMap[carY][carX].carWentedDirection == -1)
                {
                    isStarted = 1;
                }
                if (searchMap[carY][carX].carWentedDirection - 1 != 0)
                    turn(searchMap[carY][carX].carWentedDirection - 1);

                moveForward();
                if (searchMap[carY][carX].carWentedDirection != -1)
                {
                    moveBack();
                    turnTo(searchMap[carY][carX].direction);
                    //searchMap[carY][carX].carWentedDirection++;
                }
                searchMap[carY][carX].direction = carDirection;
                eraseDisplay();
                showSearchStep();
                // showSearchDirectionStep();
                // showSearchCar();
                // printf("check %d move to %d\n", searchMap[carY][carX].carWentedDirection);
                displayStringAt(0, LCD_HEIGHT - 80, "check %d move to %d\n", searchMap[carY][carX].carWentedDirection);
            }
        }
        else
        {
            if (carX == startX && carY == startY)
            {
                isStarted = 0;
                break;
            }
            moveBack();
            turnTo(searchMap[carY][carX].direction);
            eraseDisplay();
            showSearchStep();
            // showSearchDirectionStep();
            // showSearchCar();
            // printf("moveBack!\n");
            displayStringAt(0, LCD_HEIGHT - 80, "moveBack!\n");
        }
        // printf("carX:%d\tcarY:%d\tcarDirection:%d\n", carX, carY, carDirection);
        displayStringAt(0, LCD_HEIGHT - 90, "carX:%d\tcarY:%d\tcarDirection:%d\n", carX, carY, carDirection);
        // printf("end step %d \t  ==========================================\n", step++);
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

                if (!(searchMap[y][x]._type & 0b0010) && !pathMap[y - 1][x].isUpdating && !pathMap[y - 1][x].isTraveled)
                {
                    pathMap[y - 1][x].isNewEdge = 1;
                    pathMap[y - 1][x].fromX = x;
                    pathMap[y - 1][x].fromY = y;
                    pathMap[y - 1][x].isUpdating = 1;
                    pathMap[y - 1][x].direction = 0;
                    pathMap[y][x].isTraveled = 1;
                }
                if (!(searchMap[y][x]._type & 0b0001) && !pathMap[y][x + 1].isUpdating && !pathMap[y][x + 1].isTraveled)
                {
                    pathMap[y][x + 1].isNewEdge = 1;
                    pathMap[y][x + 1].fromX = x;
                    pathMap[y][x + 1].fromY = y;
                    pathMap[y][x + 1].isUpdating = 1;
                    pathMap[y][x + 1].direction = 1;
                    pathMap[y][x].isTraveled = 1;
                }
                if (!(searchMap[y][x]._type & 0b1000) && !pathMap[y + 1][x].isUpdating && !pathMap[y + 1][x].isTraveled)
                {
                    pathMap[y + 1][x].isNewEdge = 1;
                    pathMap[y + 1][x].fromX = x;
                    pathMap[y + 1][x].fromY = y;
                    pathMap[y + 1][x].isUpdating = 1;
                    pathMap[y + 1][x].direction = 2;
                    pathMap[y][x].isTraveled = 1;
                }
                if (!(searchMap[y][x]._type & 0b0100) && !pathMap[y][x - 1].isUpdating && !pathMap[y][x - 1].isTraveled)
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

void makeWay()
{
    int x = endX, y = endY;
    eraseDisplay();
    int i = 0;
    while (1)
    {
        pushWayStack(pathMap[y][x].direction);
        int nextX = pathMap[y][x].fromX;
        int nextY = pathMap[y][x].fromY;
        displayStringAt(i++*(CHAR_WIDTH),50,"%c",getDirectionSymbol(pathMap[y][x].direction));
        if (x == startX && y == startY)
        {
            break;
        }
        x = nextX;
        y = nextY;
    }
}

void findShortestPath()
{
    pathMap[startY][startX].isNewEdge = 1;

    while (1)
    {
        flood();
        if (pathMap[endY][endX].isNewEdge || pathMap[endY][endX].isTraveled)
            break;
        // showStep();
    }
    makeWay();
}

void goShortestPath()
{
    int targetDirection;
		playSound(soundDownwardTones);
		popWayStack();
		int count = 0;
    while (topOfStack > 0)
    {
        targetDirection = popWayStack();
        turnTo(targetDirection);
        moveForward();
        eraseDisplay();
        displayStringAt(0, 50, "passed %d blocks", ++count);
    }

    playSound(soundFastUpwardTones);
    displayStringAt(0,80,"avaliable block = ",countAvaliableBlock);
}
// end SHORTEST PATH================================

void turnleftWithGyro()
{ // left

    resetGyro(gyroSensor);
    wait1Msec(500);
    float Kp = 0.75;
    int expect = -90;
    int err;

    int gyro = getGyroDegrees(gyroSensor);
    while (gyro >= expect + 5)
    {
        gyro = getGyroDegrees(gyroSensor);
        err = expect - gyro;
        motor[leftMotor] = (Kp * err)/2;
        motor[rightMotor] = -(Kp * err)/2;
        wait1Msec(10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    wait1Msec(1000);

    resetGyro(gyroSensor);
}
void turnRightWithGyro()
{ // right

    resetGyro(gyroSensor);
    wait1Msec(500);
    float Kp = 0.75;
    int expect = 90;
    int err;

    int gyro = getGyroDegrees(gyroSensor);
    while (gyro <= expect - 5)
    {
        gyro = getGyroDegrees(gyroSensor);
        err = expect - gyro;
        motor[leftMotor] = (Kp * err)/2;
        motor[rightMotor] = -(Kp * err)/2;
        wait1Msec(10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    wait1Msec(1000);

    resetGyro(gyroSensor);
}

int trackingMode = 0; //mode:1 = left; mode:0 = right;
void moveWithUltra()
{
    int length = 600;
    resetMotorEncoder(leftMotor);
    resetMotorEncoder(rightMotor);
    distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;

    int countFindWall = 0;
    sumGapErrorleft = 0;
    sumGapErrorRight = 0;
    while (distance < length)
    {

        gapLeft = SensorValue[leftUlt];
        gapRight = SensorValue[rightUlt];

        if (gapLeft < outlierGap && gapRight < outlierGap)
        {
            gapErrorLeft = expectedGap - SensorValue[leftUlt];
            gapErrorRight = SensorValue[rightUlt] - expectedGap;
        }
        else if (gapLeft < outlierGap)
        {
            gapErrorLeft = expectedGap - SensorValue[leftUlt];
            gapErrorRight = gapErrorLeft;
        }
        else if (gapRight < outlierGap)
        {
            gapErrorRight = SensorValue[rightUlt] - expectedGap;
            gapErrorLeft = gapErrorRight;
        }
        else
        {
            gapErrorLeft = 0;
            gapErrorRight = 0;
        }

        distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;
        sumGapErrorLeft += gapErrorLeft;
        sumGapErrorRight += gapErrorRight;

        motor[leftMotor] = basePower + KpUltra * ((gapErrorLeft + gapErrorRight) / 2) - KiUltra/10 * ((sumGapErrorLeft + sumGapErrorRight) / 2);
        motor[rightMotor] = basePower - KpUltra * ((gapErrorLeft + gapErrorRight) / 2) + KiUltra/10 * ((sumGapErrorLeft + sumGapErrorRight) / 2);

        wait1Msec(10*10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    resetGyro(gyroSensor);
}
void moveBackWithUltra()
{
    int length = -600;
    resetMotorEncoder(leftMotor);
    resetMotorEncoder(rightMotor);
    distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;

    int countFindWall = 0;
    sumGapErrorleft = 0;
    sumGapErrorRight = 0;
    while (distance >= length)
    {

        gapLeft = SensorValue[leftUlt];
        gapRight = SensorValue[rightUlt];

        if (gapLeft < outlierGap && gapRight < outlierGap)
        {
            gapErrorLeft = expectedGap - SensorValue[leftUlt];
            gapErrorRight = SensorValue[rightUlt] - expectedGap;
        }
        else if (gapLeft < outlierGap)
        {
            gapErrorLeft = expectedGap - SensorValue[leftUlt];
            gapErrorRight = gapErrorLeft;
        }
        else if (gapRight < outlierGap)
        {
            gapErrorRight = SensorValue[rightUlt] - expectedGap;
            gapErrorLeft = gapErrorRight;
        }
        else
        {
            gapErrorLeft = 0;
            gapErrorRight = 0;
        }

        distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;
        sumGapErrorLeft += gapErrorLeft;
        sumGapErrorRight += gapErrorRight;

        motor[leftMotor] = -(basePower + KpUltra * ((gapErrorLeft + gapErrorRight) / 2) - KiUltra /10* ((sumGapErrorLeft + sumGapErrorRight) / 2));
        motor[rightMotor] = -(basePower - KpUltra * ((gapErrorLeft + gapErrorRight) / 2) + KiUltra/10 * ((sumGapErrorLeft + sumGapErrorRight) / 2));

        wait1Msec(10*10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    resetGyro(gyroSensor);
}
task main()
{
    playSound(soundFastUpwardTones);
    initPathMap();
    initSearchMap();
    resetAllSensorAutoID();
    carX = startX;
    carY = startY;
    carDirection = startDirection;

		search();
    findShortestPath();
    goShortestPath();
    wait10Msec(1000000);
}
