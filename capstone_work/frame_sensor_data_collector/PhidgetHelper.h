#ifndef PHIDGET_HELPER_H
#define PHIDGET_HELPER_H

/*
    Author: Alexander Infantino

    NOTE: The basis for much of these functions was taken from 
        www.phidgets.com
    where a lot of these sample functions were defined

    They have been slightly edited for personal use.
*/

#include <stdio.h>
#include <stdlib.h>
#include <phidget22.h>

/*
    Displays the error string associated with the PhidgetReturnCode

    @param returnCode: The PhidgetReturnCode caused by a function code
    @param message: The message to indicate where the code originated
*/
void DisplayError(PhidgetReturnCode returnCode, char * message);


#endif
