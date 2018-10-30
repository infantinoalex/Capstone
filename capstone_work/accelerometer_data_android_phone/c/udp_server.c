#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BufferSize 1024
#define PortNumber 50000

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
        int i;
        for (i = 0; i < 92; i += 4)
        {
            char data[4];
            data[0] = buffer[i];
            data[1] = buffer[i + 1];
            data[2] = buffer[i + 2];
            data[3] = buffer[i + 3];

            printf("%1.4f", data);
            printf("%1.4f,\n\n", atof(data));
        }

        printf("\nThats all\n");

    }

    return 0;
}
