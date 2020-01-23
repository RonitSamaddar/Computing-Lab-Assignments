//HEADER FILE INCLUSIONS
#include<stdio.h>           // FILE
#include<errno.h>           // perror()
#include<stdlib.h>          // exit()
#include<string.h>          // memset()
#include<sys/socket.h>      // socket()
#include<sys/types.h>       // socket()
#include <netinet/in.h>     // sockaddr_in
#include<unistd.h>          // read(),write(),close()

//MACRO DEFINATIONS
#define WORD_CAP 256
#define PORT 6000



int main()
{
    //VARIABLE DECLARATIONS
    int sock;                                       //socket file descriptor
    struct sockaddr_in address;                     //structure for socket address details
    int check;                                      //variable for error checks

    int addrlen;                                    //variable for storing structure size
    int x;                                          //return of bind system call
    int y;                                          //return of listen system call
    struct sockaddr_in addr_client;                 //address of client
    socklen_t length_client;                        //length of client address
    int client_sock;                                //new file descriptor returned by accept
    
    char *buffer,*buffer2;                          //buffers for storing strings
    int opt;                                        //options field for setsockopt
    socklen_t optlen;                               //size of options field                                     


    //Allocating memory for character arrays
    buffer=(char *)malloc(WORD_CAP*sizeof(char));
    buffer2=(char *)malloc(WORD_CAP*sizeof(char));
    
    

    //Socket establishment
    sock=socket(AF_INET,SOCK_STREAM,0);             //AF_INET = for IPV4 operations         
    if(sock==-1)                                    //SOCK_STREAM = for stream/connection based sockets
    {
        perror("SOCKET ERROR ");
        exit(1);
    }
    printf("Socket established\n");

    //Setting options for the socket
    opt=1;
    optlen=sizeof(opt);
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,optlen);


    //setting IP address and port details for the socket
    memset((void *)&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);    //INADDR_ANY binds socket to all available interfaces
    address.sin_port = htons(PORT);
   

    //Connecting to the server
    printf("HELLO1\n");
    check=connect(sock, (struct sockaddr *)&address, sizeof(address));
    printf("HELLO2\n");
    if(check< 0)
    {
        perror("CONNECT ERROR");
        return -1;
    }
    printf("Connected to server\n");
    fflush(NULL);
    recv(sock,buffer,WORD_CAP,0);
    puts(buffer);
    if(strcmp(buffer,"SERVER\t:\tConnection limit exceeded")==0)
    {
        exit(1);
    }
    //puts(buffer);
    check=fork();
    if(check==0)
    {
        //SENDING

        while(1)
        {  
            //Inputing Message to send 
            //printf("#WAITING_FOR_USER_MESSAGE\n");
            gets(buffer);
            //printf("#GOT_MESSAGE\n");
            //Sending message to server
            send(sock,buffer,strlen(buffer),0);
            //printf("#SENT_MESSAGE\n");
        }

    }
    else
    {
        //RECEIVING

        while(1)
        {
            memset(buffer,'\0',WORD_CAP);
            //printf("#READING_SERVER_MESSAGE\n");
            recv(sock,buffer,WORD_CAP,0);
            //printf("#READ_SERVER_MESSAGE\n");
            if(strcmp(buffer,"/quit")==0)
            {
                break;
            }
            puts(buffer);
        }
    }
    


    //Closing socket
    close(sock);
}