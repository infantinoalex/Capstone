/* Designed for Soft Robotic Suit
 * Stage: Frame Assembly
 * 
 * Work performed under Professor Yan Gu, TODO: Enter other professors here
 * 
 * Goals:
 *      Constantly read data from force sensors and write to log file.
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
#include <conio.h>
#include <unistd.h>

float GetForceSensorValue(int motor);
int EmergencyStopHit();
void WriteDataToFile(float sensorData, FILE * file);

FILE * rightSensorLogFile;
FILE * leftSensorLogFile;

const int RightForceSensor = 0;
const int LeftForceSensor = 1;

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

    while (1)
    {
        if (EmergencyStopHit())
        {
            printf("\n\n\nEMERGENCY STOP HAS BEEN HIT.\n\n\nExiting program and performing cleanup.\n");
            break;
        }

        float rightForceSensorValue = GetForceSensorValue(RightForceSensor);
        float leftForceSensorValue = GetForceSensorValue(LeftForceSensor);
        
        if (rightForceSensorValue == -1 || leftForceSensorValue == -1)
        {
            printf("Error reading force sensor.\n");
            return -1;
        }
    
        WriteDataToFile(rightForceSensorValue, rightSensorLogFile);
        WriteDataToFile(leftForceSensorValue, leftSensorLogFile);

        // Sleeps for 1 second. That might be too slow. Need to look at other sleep functions in different libraries/how to use them.
        sleep(1);
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

float GetForceSensorValue(int motor)
{
    if (motor == RightForceSensor)
    {
        // Poll Right Force Sensor
        return -1;
    }

    if (motor == LeftForceSensor)
    {
        // Poll Left Force Sensor
        return -1;
    }

    printf("Could not determine what force sensor to poll.\n")
    return -1;
}

int EmergencyStopHit()
{
    // If a key has been pressed
    if (kbhit())
    {
        return 1;
    }

    return 0;
}

void WriteDataToFile(float sensorData, FILE * file)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char data[128];
    sprintf(data, "%d:%d:%d,%f", tm.tm_hour, tm.tm_min, tm.tm_sec, sensorData);

    fputs(data, file);
}
