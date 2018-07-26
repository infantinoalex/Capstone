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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// If a signal is received in the parent, it will safely close the files before terminating
void sig_handler(int signal);

// Grabs the reading off of a force sensor
float GetForceSensorValue(int sensor_key);

// If a key is pressed, it will trigger a signal to kill the parent. Used as an emergency stop
// This will be constantly running on a child process so as to not miss any key that may be pressed.
int EmergencyStopHitViaKeyPress(pid_t parent_id);

// If the physical emergency stop device is triggered, it will send a signal to kill the parent.
// This will be constantly running on a child process so as to not miss if the stop is triggered.
int EmergencyStopHitViaPhysicalDevice(pid_t parent_id);

// Writes the given data to the given file
void WriteDataToFile(float sensorData, FILE * file);

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

    // Setting up Signal Handlers
    // When the child detects an emergency stop, it will send the signal which will be caught to properly clean up environment before terminating.
    struct sigaction new_action;
    sigset_t mask_sigs;
    int nsigs;
    int sigs[] = {SIGHUP, SIGINT, SIGQUIT, SIGBUS, SIGTERM, SIGSEGV, SIGFPE};
    nsigs = sizeof(sigs) / sizeof(int);
    sigemptyset(&mask_sigs);

    for (i = 0; i < nsigs; ++i)
    {
        new_act.sa_handler = sig_handler;
        new_act.sa_mask = mask_sigs;
        new_act.sa_flags = 0;

        if (sigaction(sigs[i], &new_act, NULL) == -1)
        {
            perror("Can't set signal handlers.");
            exit(1);
        }
    }

    // Spawning two child processes to constantly poll for emergency stop
    pid_t parent_id = getpid();
    pid_t pid;

    if ((pid = fork())== 0)
    {
        // First child which will monitor key press
        EmergencyStopHitCode(parent_id);
        exit(2);
    }

    if ((pid = fork()) ==)
    {
        // Second child which will monitor physical emergency stop
        EmergencyStopHitStrap(parent_id);
        exit(3);
    }

    // In parent code
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

    const int RightForceSensor = 0;
    const int LeftForceSensor = 1;

    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 100000000;

    while (1)
    {
        float rightForceSensorValue = GetForceSensorValue(RightForceSensor);
        float leftForceSensorValue = GetForceSensorValue(LeftForceSensor);
        
        if (rightForceSensorValue == -1 || leftForceSensorValue == -1)
        {
            printf("Error reading force sensor.\n");
            return -1;
        }
    
        WriteDataToFile(rightForceSensorValue, rightSensorLogFile);
        WriteDataToFile(leftForceSensorValue, leftSensorLogFile);

        // Sleeps for .1 second.
        nanosleep(sleepTime);
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

void sig_handler(int signal)
{
    printf("\n\n\nEmergency Stop Activated!\n\nExiting Program\n");
    if (fclose(rightSensorLogFile) != 0)
    {
        printf("Could not close file properly: %s\n", rightSensorFileName);
    }

    if (fclose(leftSensorLogFile) != 0)
    {
        printf("Could not close file properly: %s\n", leftSensorFileName);
    }

    exit(5);
}

float GetForceSensorValue(int sensor_key)
{
    if (sensor_key == RightForceSensor)
    {
        // Poll Right Force Sensor
        return -1;
    }

    if (sensor_key == LeftForceSensor)
    {
        // Poll Left Force Sensor
        return -1;
    }

    printf("Could not determine what force sensor to poll.\n")
    return -1;
}

void EmergencyStopHitViaKeyPress(pid_t parent_id)
{
    while (1)
    {
        if (kbhit())
        {
            // Send the kill signal
            kill(parent_id, SIGINT);
        }

        pid_t parent_check = getppid();
        if (parent_check == 1)
        {
            // Parent has been killed. Exit
            exit(5);
        }
    }
}

void EmergencyStopHitViaPhysicalDevice(pid_t parent_id)
{
    while (1)
    {
        // TODO: Determine how to check if physical emergency stop is hit
        if (0)
        {
            kill(parent_id, SIGINT);
        }

        pid_t parent_check = getppid();
        if (parent_check == 1)
        {
            // Parent has been killed. Exit
            exit(5);
        }
    }
}

void WriteDataToFile(float sensorData, FILE * file)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char data[128];
    sprintf(data, "%d:%d:%d,%f", tm.tm_hour, tm.tm_min, tm.tm_sec, sensorData);

    fputs(data, file);
}
