#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <phidget22.h>

// If a signal is received in the parent, it will safely close the files before terminating
void sig_handler(int signal);

// Used to close files and phidget channel
void CleanupProgram();

// Will be fired off when the phidget is attached
static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx);

// Will be fired off at every data interval
void CCONV OnVoltageChangeHandler(PhidgetVoltageInputHandle voltageInput, void *ctx, double voltage);

int numberOfLogFiles = 2;
FILE * sensorLogFiles[numberOfLogFiles];
char sensorFileNames[numberOfLogFiles][128];

const int RightForceSensor = 0;
const int LeftForceSensor = 1;

pid_t pid;

// Declaring global phidget variable
PhidgetVoltageRatioInputHandle channelHandler;

int main(int argc, char ** argv)
{
    // Initialize log files to record data as a csv file
    printf("Initializing...\n Setting up %d log files to record data to.\n", numberOfLogFiles);
    pid = getpid();

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int loopCounter;
    for (loopCounter = 0; loopCounter < numberOfLogFiles; ++loopCounter)
    {
        switch (loopCounter)
        {
            case RightForceSensor:
                sprintf(sensorFileNames[loopCounter], "RightSensor_DataCollection_FrameAssembly_%d_%d_%dT%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                break;
            case LeftForceSensor:
                sprintf(leftSensorFileName, "LeftSensor_DataCollection_FrameAssembly_%d_%d_%dT%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                break;
        }

        sensorLogFiles[loopCounter] = fopen(sensorFileNames[loopCounter], "w");
        if (sensorLogFiles[lopCounter] == NULL)
        {
            printf("Failed to open log file of name: %s.\nCannot record data.\n", rightSensorFileName);
            exit(6);
        }
        printf("Opened file to save data with name: %s\n", sensorFileNames[loopCounter]);
    }

    // Setting up Signal Handlers
    printf("Setting up signal handlers in case of untimely exit...\n");

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
            exit(2);
        }
    }

    // Setup Phidget
    printf("Setting up phidget to read force data...\n");

    PhidgetReturnCode prc;
    prc = PhidgetVoltageRatioInput_create(&channelHandler);
    if (prc != EPHIDGET_OK)
    {
        fprintf(stderr, "Runtime Error -> Creating VoltageRatioInput: \n\t");
        fprintf(stderr, "Code: 0x%x\n", prc);
        kill(pid, SIGINT);
        exit(3);
    }

    prc = Phidget_setDeviceSerialNumber((PhidgetHandle)channelHandler, /* TODO: GET THE SERIAL NUMBER */66666);
    if (prc != EPHIDGET_OK)
    {
        fprintf(stderr, "Runtime Error -> Setting DeviceSerialNumber: \n\t");
        fprintf(stderr, "Code: 0x%x\n", prc);
        kill(pid, SIGINT);
        exit(3);
    }

    Phidget_setOnAttachHandler((PhidgetHandle) channelHandler, onAttachHandler, NULL);

    // Done initializing
    printf("\n\nInitialization done.\nPlease hit any kit to terminate the program...\n");
    while (1)
    {
        if (kbhit())
        {
            printf("Key has been pressed. Exiting program...\n");
        }
    }

    CleanupProgram();

    return 0;
}

void sig_handler(int signal)
{
    printf("\n\n\nEmergency Stop Activated! Received signal %d.\n\nExiting Program...\n", signal);

    CleanupProgram();

    exit(5);
}

void CleanupProgram()
{
    int loopCounter;
    for (loopCounter = 0; i < numberOfLogFiles; ++i)
    {
        if (fclose(sensorLogFiles[loopCounter]) != 0)
        {
            printf("Could not close file properly: %s\n", leftSensorFileName);
        }
    }

    PhidgetVoltageRatioInput_delete(&channelHandler);
    Phidget_close((PhidgetHandle) channelHandler);
    Phidget_release((PhidgetHandle *) &channelHandler);
}

static void CCONV OnAttachHandler(PhidgetHandle ph, void *ctx)
{
    printf("Phidget has been attached.\n");
    PhidgetReturnCode prc;
    // TODO: Set data interval and such
    // Open correct channels
    
    // Set data rate in ms
    unint32_t dataInterval = 8;
    prc = PhidgetVoltageRatioInput_setDataInterval(channelHandler, dataInterval);
    if (prc != EPHIDGET_OK)
    {
        fprintf(stderr, "Runtime Error -> Setting DataInterval: \n\t");
        fprintf(stderr, "Code: 0x%x\n", prc);
        kill(pid, SIGINT);
        exit(3);
    }

    double voltageRatioChangeTrigger = 0;
    prc = PhidgetVoltageRatioInput_setVoltageRatioChangeTrigger(channelHandler, voltageRatioChangeTrigger);
    if (prc != EPHIDGET_OK)
    {
        fprintf(stderr, "Runtime Error -> Setting VoltageRatioChangeTigger: \n\t");
        fprintf(stderr, "Code: 0x%x\n", prc);
        kill(pid, SIGINT);
        exit(3);
    }

    prc = PhidgetVoltageInput_setOnVoltageRatioChangeHandler(channelHandler, OnVoltageChangeHandler, NULL);
    if (prc != EPHIDGET_OK)
    {
        fprintf(stderr, "Runtime Error -> Setting VoltageRatioChangeHandler: \n\t");
        fprintf(stderr, "Code: 0x%x\n", prc);
        kill(pid, SIGINT);
        exit(3);
    }
}

void CCONV OnVoltageChangeHandler(PhidgetVoltageInputHandle voltageInput, void *ctx, double voltage)
{
    printf("Voltage: %lf V\n", voltage);
    // TODO: Print to file and make sure it is thread safe
}

void WriteDataToFile(float sensorData, FILE * file)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char data[128];
    sprintf(data, "%d:%d:%d,%f", tm.tm_hour, tm.tm_min, tm.tm_sec, sensorData);

    fputs(data, file);
}
