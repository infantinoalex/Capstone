/* Designed for Soft Robotic Suit
 * Stage: Frame Assembly
 * 
 * Work performed under Professor Yan Gu, TODO: Enter other professors here
 * 
 * Goals:
 *      Determine when a person is falling using force sensors. Turn on corresponding motor to pull
 *      rope attached to person to enable them to stay standing.
 * 
 * Safety Concerns:
 *      Turning on the motor too fast
 *      Not turning off the motor when needed
 *      Giving too much power to the motor
 *      Not having failsafes in code to ensure user doesn't get hurt
 * 
 * Design:
 *      TODO: Talk about design here
 * 
 * Intended Motor:
 *      TODO: Link and talk about motor used
 * 
 * Intended Force Sensor:
 *      TODO: Link and talk about force sensor here
 * 
 * Authors:
 *      Alexander Infantino
 * 
 * Group Members:
 *      TODO: List Group Members here
 * 
 * Copyright:
 *      University of Massachusetts Lowell
 * 
 */

#include <stdio.h>
#include <stdlib.h>

FILE * rightSensorLogFile;
FILE * leftSensorLogFile;

int main(int argc, char ** argv)
{
    // Initialize log files to record data as a csv file
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char rightSensorFileName[128];
    char leftSensorFileName[128];
    sprintf(rightSensorFileName, "RightSensor_DataCollection_FrameAssembly_%d_%d_%dT%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(leftSensorFileName, "LeftSensor_DataCollection_FrameAssembly_%d_%d_%dT%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    rightSensorLogFile = fopen(rightSensorFileName, "w");
    leftSensorLogFile = fopen(leftSensorFileName, "w");

    if (rightSensorLogFile == NULL)
    {
        printf("Failed to open log file of name: %s.\nCannot record data.\n", rightSensorFileName);
    }

    if (leftSensorLogFile == NULL)
    {
        printf("Failed to open log file of name: %s.\nCannot record data.\n", leftSensorFileName);
    }

    printf("Successfully open log file to record data at: %s\n", rightSensorFileName);
    printf("Successfully open log file to record data at: %s\n", leftSensorFileName);

    printf("Finding max force sensor value used to determine when to turn on a motor.");
    float maxValueOfRightForceSensor = GetMinValueOfForceSensor(/* indicate right sensor here */);
    float maxValueOfLeftForceSensor = GetMinValueOfForceSensor(/* indicate left sensor here */);

    printf("Max force sensor values found.\n\tLeft Force Sensor Threshold:\t%f\n\tRight Force Sensor Threshold:\t%f\n", maxValueOfLeftForceSensor, maxValueOfRightForceSensor);

    while (1)
    {
        if (EmergencyStopHit())
        {
            printf("\n\n\nEMERGENCY STOP HAS BEEN HIT.\n\n\nExiting program and performing cleanup.\n");
            break;
        }

        float rightForceSensor = GetForceSensorValue(/* indicate right sensor here */);
        float leftForceSensor = GetForceSensorValue(/* indicate left sensor here */);
        int motorOn = 0;

        WriteDataToFile(rightForceSensor, rightSensorLogFile);
        WriteDataToFile(leftForceSensor, leftSensorLogFile);

        if (rightForceSensor > maxValueOfRightForceSensor)
        {
            printf("Right force sensor above threshold. Turning on motor.\n");
            TurnOnMotor(/* indicate right motor */, rightForceSensor, maxValueOfRightForceSensor);
            motorOn = 1;
        }
        if (leftForceSensor > maxValueOfLeftForceSensor)
        {
            printf("Left force sensor above threshold. Turning on motor.\n");
            TurnOnMotor(/* indicate left motor */, leftForceSensor, maxValueOfLeftForceSensor);
            motorOn = 1;
        }

        if (motorOn)
        {
            printf("Motor(s) are on. Slowly turning off motor while ensuring participant stays steady.\n");
            MonitorForceSensorsAndTurnOffMotors();
        }
    }

    if (fclose(rightSensorLogFile) != 0)
    {
        printf("Could not close file properly: %s\n", rightSensorFileName);
    }

    if (fclose(leftSensorLogFile) != 0)
    {
        printf("Could not close file properly: %s\n", leftSensorFileName);
    }

    return 0;
}

float GetMinValueOfForceSensor(/* pass in motor */)
{
    // poll force sensor for time period and find average. Add specified value and this will be threshold to turn on motor
}

void TurnOnMotor(int motor, float sensor, float maxValue)
{
    // Turn on motor at speed in reguard to difference/percentage of sensor value to threshold
}

void MonitorForceSensorsAndTurnOffMotors(float maxValueOfRightForceSensor, float maxValueOfLeftForceSensor)
{
    // Constantly polling both force sensors, slowly turn off the motor with respect to the threshold and current values
}

void WriteDataToFile(float sensorData, FILE * file)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char data[128];
    sprintf(data, "%d:%d:%d,%f", tm.tm_hour, tm.tm_min, tm.tm_sec, sensorData);

    fputs(data, file);
}

int EmergencyStopHit()
{
    return 1;
}
