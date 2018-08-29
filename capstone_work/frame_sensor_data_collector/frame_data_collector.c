#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <phidget22.h>
#include <time.h>

const int numberOfForceSensors = 4;
FILE * filePointerse[numberOfForceSensors];
sem_t frontRightSem = 1;
sem_t frontLeftSem = 1;
sem_t backRightSem = 1;
sem_t backLeftSem = 1;

sem_t fileSemaphores[numberOfForceSensors] = { frontRightSem, frontLeftSem, backRightSem, backLeftSem };

static void CCONV onAttachHAndler(PhidgetHandle ph, void *ctx)
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

    prc = Phidget_getChannelClassName(ph, &channelClassName);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Channel Class Name");
        exit(3);
    }

    prc = Phidget_getDeviceClass(ph, $deviceClass);
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

        printf("\n\tChannel Class: %s\n\tSerial Number: %d\n\t Hub Port: %d\n\tChannel %d\n\t", channelClassName, serialNumber, hubPort, channel);
    }
    else
    {
        printf("\n\tChannel Class: %s\n\tSerial Number: %d\n\tChannel %d\n\t", channelClassName, serialNumber, channel);
    }

    // TODO: Determine what we want the min interval to be
    printf("\tSetting DataInterval to 1000ms (1 second)");
    prc = PhidgetVoltageInput_setDataInterval((PhidgetVoltageInputHandle) ph, 1000);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Setting DataInterval");
        exit(4);
    }

    printf("\tSetting Voltage Change to 0. This will cause a reading to occur every DataInterval\n");
    prc = PhidgetVoltageInput_setVoltageChangeTrigger((PhidgetVoltageInputHandle) ph, 0.0);
    if (prc != EPHIDGET_OK)
    {
        DisplayError("Setting Voltage Change Trigger");
        exit(4);
    }

    prc = Phidget_getChannelSubclass(ph, &channelSubclass);
    if (prc != EPHIDGET_OK)
    {
        DisplayError("Getting Channel Subclass");
        exit(5);
    }

    if (channelSubclass == PHIDCHSUBCLASS_VOLTAGEINPUT_SENSOR_PORT)
    {
        printf("Setting Voltage Sensor Type\n");
        prc = PhidgetVoltageInput_setSensorType((PhidgetVoltageInputHandle) ph, SENSOR_TYPE_VOLTAGE);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Sensor Type");
            exit(6);
        }
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

    prc = Phidget_getChannelClassName(ph, &channelClassName);
    if (prc != EPHIDGET_OK)
    {
        DisplayError(prc, "Getting Channel Class Name");
        exit(3);
    }

    prc = Phidget_getDeviceClass(ph, $deviceClass);
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

        printf("\n\tChannel Class: %s\n\tSerial Number: %d\n\t Hub Port: %d\n\tChannel %d\n\t", channelClassName, serialNumber, hubPort, channel);
    }
    else
    {
        printf("\n\tChannel Class: %s\n\tSerial Number: %d\n\tChannel %d\n\t", channelClassName, serialNumber, channel);
    }
}

static void CCONV onErrorHandler(PhidgetHandle ph, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString)
{
    fprintf(stderr, "[Phidget Error Event] -> %s (%d)\n", errorString, errorCode);
}

void CCONV onFrontRightVoltageChangeHandler(PhidgetVoltageInputHandle ph, void * ctx, double sensorValue, Phidget_UnitInfo *unitInfo)
{
    File * fileToWriteTo = filePointers[0];
    p(&fileSemaphores[0]);
    WriteToFile(fileToWriteTo, sensorValue);
    w(&fileSemaphores[0]);
}

void CCONV onFrontLeftVoltageChangeHandler(PhidgetVoltageInputHandle ph, void * ctx, double sensorValue, Phidget_UnitInfo *unitInfo)
{
    File * fileToWriteTo = filePointers[1];
    p(&fileSemaphores[1]);
    WriteToFile(fileToWriteTo, sensorValue);
    w(&fileSemaphores[1]);
}

void CCONV onBackRightVoltageChangeHandler(PhidgetVoltageInputHandle ph, void * ctx, double sensorValue, Phidget_UnitInfo *unitInfo)
{
    File * fileToWriteTo = filePointers[2];
    p(&fileSemaphores[2]);
    WriteToFile(fileToWriteTo, sensorValue);
    w(&fileSemaphores[2]);
}

void CCONV onBackLeftVoltageChangeHandler(PhidgetVoltageInputHandle ph, void * ctx, double sensorValue, Phidget_UnitInfo *unitInfo)
{
    File * fileToWriteTo = filePointers[3];
    p(&fileSemaphores[3]);
    WriteToFile(fileToWriteTo, sensorValue);
    w(&fileSemaphores[3]);
}

void WriteToFile(File * file, double sensorData)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // TODO: Figure out how to get milliseconds
    // TODO: Figure out how to convert Sensor to Newtons
    char data[128];
    sprintf(data, "%d:%d:%d,%f\n", tm.tm_hour, tm.tm_min, tm.tm_sec, sensorData);

    fputs(data, file);
}


int main(int argc, char ** argv)
{
    PhidgetVoltageInputHandle channelHandlers[numberOfForceSensors] = NULL;
    PhidgetReturnCode prc;
    
    int loopCounter;

    const int FrontRightForceSensor = 0;
    const int FrontLeftForceSensor = 1;
    const int BackRightForceSensor = 2;
    const int BackLeftForceSensor = 3;

    char ** sensorNames = { "Front Right Force Sensor", "Front Left Force Sensor", "Back Right Force Sensor", "Back Left Force Sensor" };
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char fileNames[numberOfForceSensors][128];

    // TODO: Initialize Signal Handler?
    // Is this still neccessary
    
    // Create new channel objects for each channel
    for (loopCounter = 0; loopCounter < numberOfForceSensors; ++loopCounter)
    {
        // Open up the file for writing
        switch (loopCounter)
        {
            case 0:
                // Front Right Sensor
                sprintf(fileNames[loopCounter], "Front_Right_Sensor_%d_%d_%dT_%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_day, tm.tm_hour, tm.tm_min, tm.tm_sec);
                break;

            case 1:
                // Front Left Sensor
                sprintf(fileNames[loopCounter], "Front_Left_Sensor_%d_%d_%dT_%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_day, tm.tm_hour, tm.tm_min, tm.tm_sec);
                break;

            case 2:
                // Back Right Sensor
                sprintf(fileNames[loopCounter], "Back_Right_Sensor_%d_%d_%dT_%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_day, tm.tm_hour, tm.tm_min, tm.tm_sec);
                break;

            case 3:
                // Back Left Sensor
                sprintf(fileNames[loopCounter], "Back_Left_Sensor_%d_%d_%dT_%d_%d_%d.csv", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_day, tm.tm_hour, tm.tm_min, tm.tm_sec);
                break;
        }

        filePointers[loopCounter] = fopen(fileName, "w");
        if (filePointers[loopCounter] == NULL)
        {
            fprintf(stderr, "Could not open file with name %s\n", fileName);
            exit(7);
        }

        // TODO: Ask user which ID corresponds to the sensorNames[loopCount]
        // This is so at runtime, user can configure what ID represents what force sensor

        // Allocate new phidget channel object
        prc = PhidgetVoltageInput_create(&channelHandlers[loopCounter]);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Creating Channel");
            exit(2);
        }

        // Open up the channel. Will get the user to enter the correct channel for the name of the sensor
        prc = Phidget_setDeviceSerialNumber((PhidgetHandle) channelHandlers[loopCounter], /* TODO: Get Serial Number */);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Device Serial Number");
            exit(2);
        }

        prc = Phidget_setHubPort((PhidgetHandle) channelHandlers[loopCounter], /* TODO: Get Hub Port */);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Hub Port");
            exit(2);
        }

        prc = Phidget_setIsHubPortDevice((PhidgetHandle) channelHandlers[loopCounter], /* TODO: Determine if Is Hub Port */);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Is Hub Port");
            exit(2);
        }

        prc = Phidget_setChannel((PhidgetHandle) channelHandlers[loopCounter], /* TODO: Get Channel Number */);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting Channel");
            exit(2);
        }

        // Add the Event Handlers before the device is opened
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

        printf("\n\nSetting OnErrorHandler...\n");
        prc = Phidget_setOnErrorHandler((PhidgetHandle) channelHandlers[loopCounter], onErrorHandler, NULL);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Setting OnErrorHandler");
            exit(2);
        }

        // Each sensor will have a different event handler when the sensor or voltage changes  so that it grabs the correct file pointer to write to
        switch (loopCounter)
        {
            case 0:
                // Front Right Force Sensor
                // Think we only need sensor change handler, not voltage change
                // TODO: Determine comment above
                printf("Setting OnVoltageChangeHandler for Front Right Force Sensor\n");
                prc = PhidgetVoltageInput_setOnVoltageChangeHandler(channelHandlers[loopCounter], onFrontRightVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Front Right Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;

            case 1:
                // Front Left Force Sensor
                // Think we only need sensor change handler, not voltage change
                // TODO: Determine comment above
                printf("Setting OnVoltageChangeHandler for Front Left Force Sensor\n");
                prc = PhidgetVoltageInput_setOnVoltageChangeHandler(channelHandlers[loopCounter], onFrontLeftVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Front Left Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;

            case 2:
                // Back Right Force Sensor
                // Think we only need sensor change handler, not voltage change
                // TODO: Determine comment above
                printf("Setting OnVoltageChangeHandler for Back Right Force Sensor\n");
                prc = PhidgetVoltageInput_setOnVoltageChangeHandler(channelHandlers[loopCounter], onBackRightVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Back Right Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;

            case 3:
                // Back Left Force Sensor
                // Think we only need sensor change handler, not voltage change
                // TODO: Determine comment above
                printf("Setting OnVoltageChangeHandler for Back Left Force Sensor\n");
                prc = PhidgetVoltageInput_setOnVoltageChangeHandler(channelHandlers[loopCounter], onBackLeftVoltageChangeHandler, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError(prc, "Setting Back Left Force Sensor OnVoltageChangeHandler");
                    exit(2);
                }
                break;
            }

            // Open the channel with a timeout
            printf("Opening and Waiting for Attachment\n");
            prc = Phidget_openWaitForAttachment((PhidgetHandle) channelHandlers[loopCounter], 5000);
            if (prc != EPHIDGET_OK)
            {
                DisplayError(prc, "Waiting for channel to open");
                exit(2);
            }
        }
    }

    printf("All Channels Open and Recording. Waiting until key is pressed for exit\n");
    printf("Press any key to exit...\n");
    while (1)
    {
        if (kbhit())
        {
            printf("Key received. Cleaning up and Exiting\n");
            for (loopCounter = 0; loopCounter < numberOfForceSensors; ++loopCounter)
            {
                prc = PhidgetVoltageInput_setOnSensorChangeHandler(channelHandlers[loopCounter], NULL, NULL);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError("Clearing on SensorChangeHandlers");
                }

                prc = Phidget_close((PhidgetHandler) channelHandlers[loopCounter]);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError("Closing channel");
                }

                prc = PhidgetVoltageInput_delete(&channelHandlers[loopCounter]);
                if (prc != EPHIDGET_OK)
                {
                    DisplayError("Deleting ChannelHandler");
                }

                if (fclose(filePointers[loopCounter]) != 0)
                {
                    printf("Error closing file name %s\n", fileNames[loopCounter]);
                }
            }

            printf("Exiting program\n");
            exit(0);
        }
    }
}
