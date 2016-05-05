#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 12000
#define MY_ERROR(s)printf(s);system("PAUSE");exit(1);
#define SERVER_PORT 80

int main()
{
    SOCKET serverSocket;
    struct sockaddr_in serverAddress;
    int bytesRead;
    char buf[MAX_SIZE];

    //call WSAStartup first for Winsock
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2),(LPWSADATA)&wsadata)!=0)
    {
        MY_ERROR("Winsock Error\n");
    }
    /*connection*/
    memset(&serverAddress, 0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;

    serverAddress.sin_port = htons(SERVER_PORT);

    while(1)
    {

        printf("Enter the website <without http://>: ");
        char website[MAX_SIZE];
        scanf("%s",website);

        char ch, hostName[MAX_SIZE],path[MAX_SIZE];
        int i=0,j,flag=1;
        for(j = 0; j < strlen(website); j++)
        {
            ch = website[j];
            if(ch != '/' &&flag)
            {
                hostName[i++] = ch;
            }
            else if (ch == '/')
            {
                hostName[i] = '\0';
                flag = 0;
                i = 0;
                path[i++] = ch;
            }
            else
                path[i++] = ch;
        }

        path[i] = '\0';

        struct hostent *h = gethostbyname(hostName);
        if (h == NULL)
        {
            MY_ERROR("Couldn't get the host name! \n");
        }

        memcpy(&serverAddress.sin_addr, h->h_addr_list[0],h->h_length);

        /* open a socket for datagram */
        int sc = serverSocket = socket(AF_INET, SOCK_STREAM,0);
        if(sc == INVALID_SOCKET)
        {
            MY_ERROR("Invalid Socket!\n");
        }

        connect(serverSocket, (struct sockaddr *)&serverAddress,sizeof(serverAddress));

        char request[MAX_SIZE] = "GET ";
        strcat(request,path);
        strcat(request, " HTTP/1.1\r\nHost: ");
        strcat(request, hostName);
        strcat(request, "\r\nConnection: close");
        strcat(request, "\r\n\r\n");

        printf("\n======= Hyperlinks =======\n");
        //printf("\n[send request]: \n%s[hyperlinks]:\n", request)

        memset(buf,'\0',strlen(buf));
        send(serverSocket, request, strlen(request),0);

        int hyperlinksCount = 0;
        int bytes = 0;

        while(1)
        {

            bytesRead = recv(serverSocket, buf, MAX_SIZE,0);

            if(bytesRead > 0)
            {
                bytes = bytes + bytesRead;

            }
            else{

                printf("\nrecv: %d bytes\n", bytes);
                break;

            }
            buf[bytesRead] = '\0';
            char *delim = "<";

            char *pch = strtok(buf,delim);
            while(pch != NULL)
            {

                char str1[MAX_SIZE],str2[MAX_SIZE];
                sscanf(pch,"%s %s", str1, str2);

                if( !strcmp(str1, "a") && !strncmp(str2,"href",4) && str2[6]!='#' )
                {
                    char web[MAX_SIZE],temp2[MAX_SIZE];
                    int j=-1;
                    for(int i=6;i<strlen(str2);i++){
                      if(str2[i]=='\"')
                       break;
                      else{
                       j++;
                       web[j]=str2[i];
                       }

                    }

                    printf("%s\n",web);
                    hyperlinksCount++;
                }

                pch = strtok(NULL,delim);
            }

        }
        printf("Connection closed\n");

        printf("Count: %d\n\n", hyperlinksCount);

        closesocket(serverSocket);
    }

    system("PAUSE");
    return 0;
}
