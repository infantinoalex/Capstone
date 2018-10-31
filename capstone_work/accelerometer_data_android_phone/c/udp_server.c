#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
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

int main(int argc, char ** argv)
{
    int sockfd;
    char buffer[BufferSize];
    
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

        float accelerationX = ConvertBytesToFloat(buffer, 0);
        float accelerationY = ConvertBytesToFloat(buffer, 4);
        float accelerationZ = ConvertBytesToFloat(buffer, 8);
        float rotationX = ConvertBytesToFloat(buffer, 24);
        float rotationY = ConvertBytesToFloat(buffer, 28);
        float rotationZ = ConvertBytesToFloat(buffer, 32);
        float pitch = ConvertBytesToFloat(buffer, 40);
        float roll = ConvertBytesToFloat(buffer, 44);

        printf("Acceleration: X: %f\tY: %f\tZ: %f\n", accelerationX, accelerationY, accelerationZ);
        printf("Rotation: X: %f\tY: %f\tZ: %f\n", rotationX, rotationY, rotationZ);
        printf("Pitch: %f\nRoll: %f\n", pitch, roll);

        float acceleration = sqrtf((accelerationX * accelerationX) + (accelerationY * accelerationY) + (accelerationZ * accelerationZ));
        float angularVelocity = sqrtf((rotationX * rotationX) + (rotationY * rotationY) + (rotationZ * rotationZ));
        float pitchAndRollStuff = sqrtf((pitch * pitch) + (roll * roll));

        printf("Acceleration: %f\n", acceleration);
        printf("Angular Velocity: %f\n\n\n\n\n", angularVelocity);

        int shouldSleep = 0;

        /*if (acceleration < accelerationThreshold)
        {
            if (isAccelerationPastThreshold)
            {
                printf("Fall detected due to Acceleration\n\n");
                exit(0);
            }
            else
            {
                printf("Acceleration past threshold with reading: %f\n", acceleration);
                isAccelerationPastThreshold = 1;
                shouldSleep = 1;
            }
        }
        else
        {
            isAccelerationPastThreshold = 0;
        }*/
        
        if (angularVelocity > angularVelocityThreshold)
        {
            if (isAngularVelocityPastThreshold)
            {
                printf("Fall detected due to Angular Velocity\n\n");
                exit(0);
            }
            else
            {
                printf("Angular Velocity past threshold with reading: %f\n", angularVelocity);
                isAngularVelocityPastThreshold = 1;
                shouldSleep = 1;
            }
        }
        else
        {
            isAngularVelocityPastThreshold = 0;
        }

        if (shouldSleep)
        {
            printf("Sleeping for 255ms to and repolling to determine if a fall is occuring\n");
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
