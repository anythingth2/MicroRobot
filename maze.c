#pragma config(Sensor, S1,     ult,            sensorEV3_Ultrasonic)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro)
#pragma config(Sensor, S4,     button,         sensorEV3_Touch)
#pragma config(Motor,  motorA,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)
#pragma config(Motor,  motorC,          ctrlMotor,     tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorD,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//#pragma DebuggerWindows("debugStream")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#define LEFT_ULTRA_SERVO 0
#define RIGHT_ULTRA_SERVO 1
int distance = 0;
int lastGyroErr = 0;
int gyroErr = 0;
int sumGyroErr = 0;
int basePower = 30;
int defaultGyro = 0;
float KpGyro = 1.82;
float KiGyro = 0.039; // sum error
float KdGyro = 2.75;  // recent - last error
float KpUltra = 1;
int leftB = 0;
int rightB = 0;
int forwardB = 0;

int ultFound[3] = {0, 0, 0};
void turnUltraServo(int to)
{
    switch (to)
    {
    case LEFT_ULTRA_SERVO:
        moveMotorTarget(motorC, -92, -20); // ult mov Back to front
        break;
    case RIGHT_ULTRA_SERVO:
        moveMotorTarget(motorC, 92, 20);
        break;
    }
    waitUntilMotorStop(motorC);
}
void turnleftWithGyro()
{ // left

    resetGyro(gyroSensor);
    wait1Msec(500);
    float Kp = 0.75;
    int expect = -92;
    int err;

    int gyro = getGyroDegrees(gyroSensor);
    while (gyro >= expect + 5)
    {
        gyro = getGyroDegrees(gyroSensor);
        err = expect - gyro;
        motor[leftMotor] = (Kp * err);
        motor[rightMotor] = -(Kp * err);
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
    int expect = 92;
    int err;

    int gyro = getGyroDegrees(gyroSensor);
    while (gyro <= expect - 5)
    {
        gyro = getGyroDegrees(gyroSensor);
        err = expect - gyro;
        motor[leftMotor] = (Kp * err);
        motor[rightMotor] = -(Kp * err);
        wait1Msec(10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    wait1Msec(1000);

    resetGyro(gyroSensor);
}
void mov1Block()
{

    int length = 635;
    resetMotorEncoder(leftMotor);
    resetMotorEncoder(rightMotor);
    resetGyro(gyroSensor);
    int distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;

    while (distance < length)
    {
        gyroErr = getGyroDegrees(gyroSensor) + defaultGyro;
        distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;

        sumGyroErr += gyroErr;
        motor[rightMotor] = basePower + KpGyro * gyroErr + KiGyro * (sumGyroErr) + KdGyro * (lastGyroErr - gyroErr);
        motor[leftMotor] = basePower - KpGyro * gyroErr - KiGyro * (sumGyroErr)-KdGyro * (lastGyroErr - gyroErr);

        lastGyroErr = gyroErr;
        wait1Msec(10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
}
int trackingMode = 1; //mode:1 = left; mode:0 = right;

void moveWithUltra()
{
    float gap = SensorValue[ult];
    int length = 635;
    resetMotorEncoder(leftMotor);
    resetMotorEncoder(rightMotor);
    int distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;

    float outlierGap = 30.5;
    int countFindWall = 0;
    float expectedLeftGap = 7.5;
    float expectedRightGap = 12.5;
    float expectedFrontGap = 3.6;
    float gapError, sumGapError, lastGapError;
    while (distance < length)
    {
        gap = SensorValue[ult];
        if (gap > outlierGap && countFindWall < 2)
        {
            countFindWall++;

            trackingMode = !trackingMode;
            motor[leftMotor] = 0;
            motor[rightMotor] = 0;
            if (trackingMode)
            {
                turnUltraServo(LEFT_ULTRA_SERVO);
                turnUltraServo(LEFT_ULTRA_SERVO);
            }
            else
            {
                turnUltraServo(RIGHT_ULTRA_SERVO);
                turnUltraServo(RIGHT_ULTRA_SERVO);
            }
        }
        if (countFindWall < 2)
        {
            if (trackingMode)
            {
                gapError = expectedLeftGap - gap;
            }
            else
            {
                gapError = gap - expectedRightGap;
            }
        }
        else
        {
            gapError = 0;
        }
        distance = (getMotorEncoder(leftMotor) + getMotorEncoder(rightMotor)) / 2;

        motor[leftMotor] = basePower + KpUltra * gapError;
        motor[rightMotor] = basePower - KpUltra * gapError;
        sumGapError += gapError;
        //motor[leftMotor] = basePower + KpUltra * gapError;
        //motor[rightMotor] = basePower - KpUltra * gapError;

        lastGapError = gapError;
        wait1Msec(10);
    }
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
    /*
    //adjust gap
    if (trackingMode)
    {
        turnUltraServo(RIGHT_ULTRA_SERVO);
        gap = SensorValue[ult];
        if (gap < outlierGap)
            while (!((gap - expectedFrontGap) < 3 && (gap - expectedFrontGap) > -3))
            {
                int backPower = gap > expectedFrontGap ? basePower : -basePower;
                backPower /= 2;
                motor[leftMotor] = backPower;
                motor[rightMotor] = backPower;
            }
        turnUltraServo(LEFT_ULTRA_SERVO);
    }else{
  			turnUltraServo(LEFT_ULTRA_SERVO);
        gap = SensorValue[ult];
        if (gap < outlierGap)
            while (!((gap - expectedFrontGap) < 3 && (gap - expectedFrontGap) > -3))
            {
            		gap = SensorValue[ult];
                int backPower = gap > expectedFrontGap ? basePower : -basePower;
                backPower /=2;
                motor[leftMotor] = backPower;
                motor[rightMotor] = backPower;
            }
        turnUltraServo(RIGHT_ULTRA_SERVO);
    }
    */
    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
}

void chk()
{

    moveMotorTarget(motorC, -92, -20); // ult mov Back to front
    waitUntilMotorStop(motorC);
    leftB = SensorValue[S1]; // left chk
    if (leftB <= 15)
    {
        playSound(soundShortBlip);
        ultFound[0] = ultFound[0] || 1;
    }
    else if (leftB > 20)
    {
        ultFound[0] = ultFound[0] || 0;
    }
    wait1Msec(1000);
    moveMotorTarget(motorC, 92, 20);
    waitUntilMotorStop(motorC);

    forwardB = SensorValue[S1]; // forward chk
    if (forwardB <= 15)
    {
        playSound(soundShortBlip);
        ultFound[1] = ultFound[1] || 1;
    }
    else if (forwardB > 25)
    {
        ultFound[1] = ultFound[1] || 0;
    }
    wait1Msec(1000);

    moveMotorTarget(motorC, 92, 20);
    waitUntilMotorStop(motorC);
    rightB = SensorValue[S1]; // right chk
    if (rightB <= 20)
    {
        playSound(soundShortBlip);
        ultFound[2] = ultFound[2] || 1;
    }
    else if (rightB > 25)
    {
        ultFound[2] = ultFound[2] || 0;
    }
    wait1Msec(2000);

    moveMotorTarget(motorC, -92, -20); // ult mov Back to front
    waitUntilMotorStop(motorC);

    if (ultFound[0] == 1 && ultFound[1] == 0 && ultFound[2] == 0) //find left 100
    {
        mov1Block();
    }
    else if (ultFound[0] == 0 && ultFound[1] == 0 && ultFound[2] == 1) //find right 001
    {
        mov1Block();
    }
    else if (ultFound[0] == 0 && ultFound[1] == 1 && ultFound[2] == 0) //find front 010
    {
        turnleftWithGyro();
        mov1Block();
    }
    else if (ultFound[0] == 1 && ultFound[1] == 1 && ultFound[2] == 0) //find left+front 110
    {
        turnRightWithGyro();
        mov1Block();
    }
    else if (ultFound[0] == 0 && ultFound[1] == 1 && ultFound[2] == 1) //find right+front 011
    {
        turnleftWithGyro();
        mov1Block();
    }
    else if (ultFound[0] == 1 && ultFound[1] == 1 && ultFound[2] == 1) //find left+front+right 111
    {
        turnRightWithGyro();
        turnRightWithGyro();
        mov1Block();
    }
    else if (ultFound[0] == 1 && ultFound[1] == 0 && ultFound[2] == 1) //find left+right 101
    {
        mov1Block();
    }

    ultFound[0] = 0;
    ultFound[1] = 0;
    ultFound[2] = 0;
}
void displayShortP() // input position x,y or sth from shortest
{
    //	if(
    // displayStringAt(50,100,"*"); // x,y
    //displayStringAt(55,100,"*");
}
task main()
{
    playSound(soundFastUpwardTones);
    //moveWithUltra();
    while (1)
	{

		moveWithUltra();
		turnRightWithGyro();
		moveWithUltra();
		turnleftWithGyro();
		moveWithUltra();
		turnRightWithGyro();
		moveWithUltra();
		turnleftWithGyro();
		moveWithUltra();
		turnRightWithGyro();
		moveWithUltra();
    }
}