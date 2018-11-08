#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BufferSize 1024
#define PortNumber 50000

union
{
    char chars[4];
    float f;
} floatUnion;

float ConvertBytesToFloat(char * buffer, int offset);
float ConvertStringToFloat(char * buffer);

int main(int argc, char ** argv)
{
    int sockfd;
    char buffer[BufferSize];

    time_t t = time(NULL);
    struct tm * timeinfo = localtime(&t);

    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PortNumber); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    int isAccelerationPastThreshold = 0;
    int isAngularVelocityPastThreshold = 0;

    const float accelerationThreshold = 4.6;
    const float angularVelocityThreshold = 3.6;

    int shouldSleep = 0;

    while (1)
    {
        int len, n; 
        n = recvfrom(sockfd, 
                    (char *)buffer,
                    BufferSize,  
                    MSG_WAITALL,
                    ( struct sockaddr *) &cliaddr, 
                    &len);

        buffer[n] = '\0'; 

        if (buffer[0] == 'V')
        {
            float measuredAngularVelocity = ConvertStringToFloat(buffer);

            if (measuredAngularVelocity > angularVelocityThreshold)
            {
                if (isAngularVelocityPastThreshold)
                {
                    printf("Fall detected due to Angular Velocity\n\n");
                    usleep(500 * 1000);
                }
                else
                {
                    printf("Angular Velocity past threshold with reading: %f\n", measuredAngularVelocity);
                    isAngularVelocityPastThreshold = 1;
                    shouldSleep = 1;
                }
            }
            else
            {
                isAngularVelocityPastThreshold = 0;
            }
        }
        else if (buffer[0] == 'A')
        {
            float measuredAcceleration = ConvertStringToFloat(buffer);

            if (measuredAcceleration < accelerationThreshold)
            {
                if (isAccelerationPastThreshold)
                {
                    printf("Fall detected due to Acceleration\n\n");
                    usleep(500 * 1000);
                }
                else
                {
                    printf("Acceleration past threshold with reading: %f", measuredAcceleration);
                    isAccelerationPastThreshold = 1;
                    shouldSleep = 1;
                }
            }
            else
            {
                isAccelerationPastThreshold = 0;
            }
        }

        if (!isAccelerationPastThreshold && !isAngularVelocityPastThreshold)
        {
            shouldSleep = 0;
        }

        if (shouldSleep)
        {
            printf("Sleeping for 255ms and repolling to determine if fall is occurring\n");
            usleep(255 * 1000);
        }
    }

    return 0;
}

float ConvertBytesToFloat(char * buffer, int offset)
{
    floatUnion.chars[3] = buffer[offset];
    floatUnion.chars[2] = buffer[offset + 1];
    floatUnion.chars[1] = buffer[offset + 2];
    floatUnion.chars[0] = buffer[offset + 3];

    return floatUnion.f;
}

float ConvertStringToFloat(char * buffer)
{
    char result[32];

    char currentItem = buffer[2];
    int loopCounter = 0;
    while (currentItem != '\0')
    {
        result[loopCounter] = currentItem;
        ++loopCounter;
        currentItem = buffer[loopCounter + 2];
    }

    return atof(result);
}
