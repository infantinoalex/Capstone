#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <phidget22.h>

/*
    Author: Alexander Infantino

    NOTE: The basis for much of these functions was taken from 
        www.phidgets.com
    where a lot of these sample functions were defined

    They have been slightly edited for personal use.
*/

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
