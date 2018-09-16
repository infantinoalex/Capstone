#define _BSD_SOURCE

/* TODO: Delete K and Offset from Code
   TODO: Change datarate to 8ms */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <phidget22.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>

const int numberOfForceSensors = 4;
FILE * filePointers[4];
sem_t frontRightSem;
sem_t frontLeftSem;
sem_t backRightSem;
sem_t backLeftSem;

struct timeval tval_start;

/*
    Displays the error string associated with the PhidgetReturnCode

    @param returnCode: The PhidgetReturnCode caused by a function code
    @param message: The message to indicate where the code originated
*/

void DisplayError(PhidgetReturnCode returnCode, char * message)
{
    PhidgetReturnCode prc;
    const char * error;

    fprintf(stderr, "Runtime Error -> %s\n\t", message);

    prc = Phidget_getErrorDescription(returnCode, &error);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting ErrorDescription");
        return;
    }

    fprintf(stderr, "Desc: %s\n", error);
}

static void CCONV onErrorHandler(PhidgetHandle ph, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString)
{
    fprintf(stderr, "[Phidget Error Event] -> %s (%d)\n", errorString, errorCode);
}

static void CCONV onAttachHandler(PhidgetHandle ph, void *ctx)
{
    PhidgetReturnCode prc;
    Phidget_ChannelSubclass channelSubclass;
    Phidget_DeviceClass deviceClass;
    char * channelClassName;
    int32_t serialNumber;
    int32_t hubPort;
    int32_t channel;

    printf("Attach Event\n");

    prc = Phidget_getDeviceSerialNumber(ph, &serialNumber);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Device Serial Number");
        exit(3);
    }

    prc = Phidget_getChannel(ph, &channel);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Channel");
        exit(3);
    }

    prc = Phidget_getDeviceClass(ph, &deviceClass);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Device Class");
        exit(3);
    }

    if (deviceClass == PHIDCLASS_VINT)
    {
        prc = Phidget_getHubPort(ph, &hubPort);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Getting Hub Port");
            exit(3);
        }

        printf("\n\ttSerial Number: %d\n\t Hub Port: %d\n\tChannel %d\n\t", serialNumber, hubPort, channel);
    }
    else
    {
        printf("\n\t\n\tSerial Number: %d\n\tChannel %d\n\t", serialNumber, channel);
    }

    /* TODO: Determine what we want the min interval to be */
    printf("\tSetting DataInterval to 1000ms (1 second)");
    prc = PhidgetVoltageRatioInput_setDataInterval((PhidgetVoltageRatioInputHandle) ph, 1000);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Setting DataInterval");
        exit(4);
    }

    printf("\tSetting Voltage Change to 0. This will cause a reading to occur every DataInterval\n");
    prc = PhidgetVoltageRatioInput_setVoltageRatioChangeTrigger((PhidgetVoltageRatioInputHandle) ph, 0.0);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Setting Voltage Change Trigger");
        exit(4);
    }

    prc = Phidget_getChannelSubclass(ph, &channelSubclass);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Channel Subclass");
        exit(5);
    }

    if (channelSubclass == PHIDCHSUBCLASS_VOLTAGEINPUT_SENSOR_PORT)
    {
        printf("Setting Voltage Sensor Type\n");
        prc = PhidgetVoltageRatioInput_setSensorType((PhidgetVoltageRatioInputHandle) ph, SENSOR_TYPE_VOLTAGE);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Sensor Type");
            exit(6);
        }
    }
    
    printf("\n\nSetting OnErrorHandler...\n");
    prc = Phidget_setOnErrorHandler((PhidgetHandle) ph, onErrorHandler, NULL);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Setting OnErrorHandler");
        exit(2);
    }
}

static void CCONV onDetachHandler(PhidgetHandle ph, void *ctx)
{
    PhidgetReturnCode prc;
    Phidget_ChannelSubclass channelSubclass;
    Phidget_DeviceClass deviceClass;
    char * channelClassName;
    int32_t serialNumber;
    int32_t hubPort;
    int32_t channel;

    printf("Detach Event\n");

    prc = Phidget_getDeviceSerialNumber(ph, &serialNumber);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Device Serial Number");
        exit(3);
    }

    prc = Phidget_getChannel(ph, &channel);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Channel");
        exit(3);
    }

    prc = Phidget_getDeviceClass(ph, &deviceClass);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Device Class");
        exit(3);
    }

    if (deviceClass == PHIDCLASS_VINT)
    {
        prc = Phidget_getHubPort(ph, &hubPort);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Getting Hub Port");
            exit(3);
        }

        printf("\n\tSerial Number: %d\n\t Hub Port: %d\n\tChannel %d\n\t", serialNumber, hubPort, channel);
    }
    else
    {
        printf("\n\tSerial Number: %d\n\tChannel %d\n\t", serialNumber, channel);
    }
}

void WriteToFile(FILE * file, double measuredForce)
{
    time_t t = time(NULL);
    struct tm * timeinfo = localtime(&t);

    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    
    /* Gets usec since it started for a unique number */
    struct timeval timeDifference;
    timersub(&currentTime, &tval_start, &timeDifference);

    char test[48];
    strftime(test, 48, "%H:%M:%S", timeinfo);

    char data[128];
    sprintf(data, "%s.%06ld,%f\n", test, (long int)timeDifference.tv_usec, measuredForce);

    fputs(data, file);
}

void CCONV onFrontRightVoltageChangeHandler(PhidgetVoltageRatioInputHandle ph, void * ctx, double voltageRatio)
{
    FILE * fileToWriteTo = filePointers[0];
    sem_wait(&frontRightSem);
    WriteToFile(fileToWriteTo, voltageRatio);
    sem_post(&frontRightSem);
}

void CCONV onFrontLeftVoltageChangeHandler(PhidgetVoltageRatioInputHandle ph, void * ctx, double voltageRatio)
{
    FILE * fileToWriteTo = filePointers[1];
    sem_wait(&frontLeftSem);
    WriteToFile(fileToWriteTo, voltageRatio);
    sem_post(&frontLeftSem);
}

void CCONV onBackRightVoltageChangeHandler(PhidgetVoltageRatioInputHandle ph, void * ctx, double voltageRatio)
{
    FILE * fileToWriteTo = filePointers[2];
    sem_wait(&backRightSem);
    WriteToFile(fileToWriteTo, voltageRatio);
    sem_post(&backRightSem);
}

void CCONV onBackLeftVoltageChangeHandler(PhidgetVoltageRatioInputHandle ph, void * ctx, double voltageRatio)
{
    FILE * fileToWriteTo = filePointers[3];
    sem_wait(&backLeftSem);
    WriteToFile(fileToWriteTo, voltageRatio);
    sem_post(&backLeftSem);
}

int main(int argc, char ** argv)
{
    PhidgetVoltageRatioInputHandle channelHandlers[4];
    PhidgetReturnCode prc;
    
    int loopCounter;
    int channelUsed[4] = { 0, 0, 0, 0 };
    int scanfResult;

    const int FrontRightForceSensor = 0;
    const int FrontLeftForceSensor = 1;
    const int BackRightForceSensor = 2;
    const int BackLeftForceSensor = 3;

    char *sensorNames[] = { "Front Right Force Sensor", "Front Left Force Sensor", "Back Right Force Sensor", "Back Left Force Sensor" };
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char fileNames[4][128];

    gettimeofday(&tval_start, NULL);

    sem_init(&frontRightSem, 0, 1);
    sem_init(&frontLeftSem, 0, 1);
    sem_init(&backRightSem, 0, 1);
    sem_init(&backLeftSem, 0, 1);
    
    /* Create new channel objects for each channel */
    for (loopCounter = 0; loopCounter < numberOfForceSensors; ++loopCounter)
    {
        /* Open up the file for writing */
        switch (loopCounter)
        {
            case 0:
                /* Front Right Sensor */
                sprintf(fileNames[loopCounter], "/home/pi/Capstone_Data/Front_Right_Sensor/Front_Right_Sensor_%d%d%dT_%d_%d_%d_%06ld.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (long int) tval_start.tv_usec);
                break;

            case 1:
                /* Front Left Sensor */
                sprintf(fileNames[loopCounter], "/home/pi/Capstone_Data/Front_Left_Sensor/Front_Left_Sensor_%d%d%dT_%d_%d_%d_%06ld.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (long int) tval_start.tv_usec);
                break;

            case 2:
                /* Back Right Sensor */
                sprintf(fileNames[loopCounter], "/home/pi/Capstone_Data/Back_Right_Sensor/Back_Right_Sensor_%d%d%dT_%d_%d_%d_%06ld.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (long int) tval_start.tv_usec);
                break;

            case 3:
                /* Back Left Sensor */
                sprintf(fileNames[loopCounter], "/home/pi/Capstone_Data/Back_Left_Sensor/Back_Left_Sensor_%d%d%dT_%d_%d_%d_%06ld.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (long int) tval_start.tv_usec);
                break;
        }

        int channelToUse = -1;
        printf("Please enter the Channel ID for the %s:\n\tIf you wish to not setup this sensor, enter the value [ -1 ] to skip.\n\nValue: ", sensorNames[loopCounter]);
        scanfResult = scanf("%d", &channelToUse);
        while ((getchar()) != '\n');
        printf("\n\n");

        if (scanfResult == EOF || scanfResult == 0)
        {
            fprintf(stderr, "Error reading in value. Terminating program.\n");
            exit(8);
        }

        if (channelToUse < 0 || channelToUse > 3)
        {
            if (channelToUse == -1)
            {
                printf("Not registering channel\n");
                continue;
            }
            else
            {
                fprintf(stderr, "Channel must be between 0 - 3 as it is only configued for FOUR sensors.\n\nExiting program.\n");
                exit(8);
            }
        }

        printf("Using channel %d\n", channelToUse);

        channelUsed[loopCounter] = 1;
        printf("Opening %d file: %s\n", loopCounter, fileNames[loopCounter]);
        filePointers[loopCounter] = fopen(fileNames[loopCounter], "w+");
        if (filePointers[loopCounter] == NULL)
        {
            fprintf(stderr, "Could not open file with name %s\n", fileNames[loopCounter]);
            exit(7);
        }

        /* Allocate new phidget channel object */
        prc = PhidgetVoltageRatioInput_create(&channelHandlers[loopCounter]);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Creating Channel");
            exit(2);
        }

        /* Open up the channel. Will get the user to enter the correct channel for the name of the sensor */
        prc = Phidget_setDeviceSerialNumber((PhidgetHandle) channelHandlers[loopCounter], 533044);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Device Serial Number");
            exit(2);
        }

        prc = Phidget_setChannel((PhidgetHandle) channelHandlers[loopCounter], channelToUse);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Channel");
            exit(2);
        }

        /* Add the Event Handlers before the device is opened */
        printf("\n\nSetting OnAttachHandler...\n");
        prc = Phidget_setOnAttachHandler((PhidgetHandle) channelHandlers[loopCounter], onAttachHandler, NULL);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting OnAttachHandler");
            exit(2);
        }

        printf("\n\nSetting OnDetachHandler...\n");
        prc = Phidget_setOnDetachHandler((PhidgetHandle) channelHandlers[loopCounter], onDetachHandler, NULL);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting OnDetachHAndler");
            exit(2);
        }

        /* Each sensor will have a different event handler when the sensor or voltage changes  so that it grabs the correct file pointer to write to */
        switch (loopCounter)
        {
            case 0:
                printf("Setting OnVoltageChangeHandler for Front Right Force Sensor\n");
                prc = PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(channelHandlers[loopCounter], onFrontRightVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Front Right Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;

            case 1:
                printf("Setting OnVoltageChangeHandler for Front Left Force Sensor\n");
                prc = PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(channelHandlers[loopCounter], onFrontLeftVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Front Left Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;

            case 2:
                printf("Setting OnVoltageChangeHandler for Back Right Force Sensor\n");
                prc = PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(channelHandlers[loopCounter], onBackRightVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Back Right Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;

            case 3:
                printf("Setting OnVoltageChangeHandler for Back Left Force Sensor\n");
                prc = PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(channelHandlers[loopCounter], onBackLeftVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Back Left Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;
        }

        /* Open the channel with a timeout */
        printf("Opening and Waiting for Attachment\n");
        prc = Phidget_openWaitForAttachment((PhidgetHandle) channelHandlers[loopCounter], 5000);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Waiting for channel to open");
            exit(2);
        }
    }

    
    printf("All Channels Open and Recording. Waiting until key is pressed for exit\n");
    printf("Press any key to exit...\n");
    getchar();
    printf("Key received. Cleaning up and Exiting\n");
    for (loopCounter = 0; loopCounter < numberOfForceSensors; ++loopCounter)
    {
        if (channelUsed[loopCounter])
        {
            printf("Closing Phidget for %s\n", sensorNames[loopCounter]);
            prc = Phidget_close((PhidgetHandle) channelHandlers[loopCounter]);
            if (prc != EPHIDGET_OK)
            {
                DisplayError(prc, "Closing channel");
            }

            printf("Deleting Phidget for %s\n", sensorNames[loopCounter]);
            prc = PhidgetVoltageRatioInput_delete(&channelHandlers[loopCounter]);
            if (prc != EPHIDGET_OK)
            {
                DisplayError(prc, "Deleting ChannelHandler");
            }

            printf("Closing file %s for %s\n", fileNames[loopCounter], sensorNames[loopCounter]);
            if (fclose(filePointers[loopCounter]) != 0)
            {
                printf("Error closing file name %s\n", fileNames[loopCounter]);
            }
        }
    }

    printf("Exiting program\n");
    exit(0);
}
