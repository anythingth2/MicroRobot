#pragma config(Sensor, S1, rightColor, sensorEV3_Color)
#pragma config(Sensor, S2, ultSensor, sensorEV3_Ultrasonic)
#pragma config(Sensor, S3, boxColor, sensorLightActive)
#pragma config(Sensor, S4, leftColor, sensorEV3_Color)
#pragma config(Motor, motorA, rightMotor, tmotorEV3_Large, PIDControl, driveRight, encoder)
#pragma config(Motor, motorC, keepBox, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor, motorD, leftMotor, tmotorEV3_Large, PIDControl, driveLeft, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
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

float KpColor = 0.05;
float KiColor = 0.000001;
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
void moveForward(int block)
{
   // int i = 1;
		sumErr = 0;
		for(int i = 0;i< block;i++){
	    do
	    {
	        leftCol = getColorReflected(leftColor);
	        if(leftCol>40)
	        	leftCol = 40;
	        rightCol = getColorReflected(rightColor);
					if(rightCol>40)
						rightCol = 40;
	        err = leftCol - rightCol;
					sumErr += err;

	        motor[leftMotor] = basepower + KpColor*err + KiColor*sumErr;
	        motor[rightMotor] = basepower - KpColor*err - KiColor*sumErr;

	    }while(!(leftCol <= fullBlackThreshold && rightCol <= fullBlackThreshold));

	    while(leftCol <= fullBlackThreshold && rightCol <= fullBlackThreshold){
	        leftCol = getColorReflected(leftColor);
	        rightCol = getColorReflected(rightColor);
	    }
  }
}
void turn(int to){
	if(to == RIGHT_HAND){
		moveMotorTarget(leftMotor,180,basepower);
		moveMotorTarget(rightMotor,180,-basepower);

	}else{
		moveMotorTarget(leftMotor,180,-basepower);
		moveMotorTarget(rightMotor,180,basepower);

	}
		waitUntilMotorStop(leftMotor);
		waitUntilMotorStop(rightMotor);

}
void pickUp()
{
    moveMotorTarget(motorC, 650, 80);
    waitUntilMotorStop(motorC);
}
void Drop()
{
    moveMotorTarget(motorC, -650, -80);
    waitUntilMotorStop(motorC);
}
void getColor() // color = 1 is black - color = 2 is orange
{
    colorOfBox = getColorReflected(leftColor);
    if (colorOfBox <= 20)
    {
        color = 1; // black
    }
    else if (colorOfBox >= 30)
    {
        color = 2; //orange
    }
}
void isFound()
{
    frontult = SensorValue[S2]; // ultrasonic
    if (frontult <= 15)
    {
        getColor();
    }
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
}

void initCar()
{
    car.x = startX;
    car.y = startY;
    car.direction = startDirection;
}


task main()
{
	for(int i=0;i<9;i++){
		moveForward(9-1);
		turn(LEFT_HAND);
	}
}
