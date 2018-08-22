#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <phidget22.h>

const int numberOfForceSensors = 4;

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
    
    // TODO: Initialize Signal Handler
    
    // Create new channel objects for each channel
    for (loopCounter = 0; loopCounter < numberOfForceSensors; ++loopCounter)
    {
        // Open up specific file

        // Allocate new phidget channel object
        prc = PhidgetVoltageInput_create(&channelHandlers[loopCounter]);
        if (prc != EPHIDGET_OK)
        {
            DisplayError(prc, "Creating Channel");
            exit(2);
        }

        // Open up the channel. Will get the user to enter the correct channel for the name of the sensor

    }
}
