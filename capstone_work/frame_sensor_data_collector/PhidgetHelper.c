#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <phidget22.h>
#include "PhidgetHelper.h"

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
