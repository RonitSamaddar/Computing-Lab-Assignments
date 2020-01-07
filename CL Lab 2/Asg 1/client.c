//HEADER FILE INCLUSIONS
#include<stdio.h> 			// FILE
#include<errno.h> 			// perror()
#include<stdlib.h> 			// exit()
#include<string.h>			// memset()
#include<sys/socket.h>		// socket()
#include<sys/types.h>		// socket()
#include <netinet/in.h>		// sockaddr_in
#include<unistd.h>			// read(),write(),close()

/*#include<sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
*/

//MACRO DEFINATIONS
#define InputFile "input.txt"
#define LineBytes 256
#define PORT 6000



int main()
{
	int sock;										//socket file descriptor
	struct sockaddr_in address;						//structure for socket address details
	int check;										//variable for error checks

	int addrlen;									//variable for storing structure size
	int x;											//return of bind system call
	int y;											//return of listen system call
	struct sockaddr_in addr_client; 				//address of client
	socklen_t length_client; 						//length of client address
	int client_sock;								//new file descriptor returned by accept
	
	char *buffer,*buffer2;							//buffers for storing strings
	int opt;										//options field for setsockopt
	socklen_t optlen;								//size of options field										


	printf("ENTER COMMAND :\n");
	buffer=(char *)malloc(LineBytes*sizeof(char));
	buffer2=(char *)malloc(LineBytes*sizeof(char));
	gets(buffer);

	sock=socket(AF_INET,SOCK_STREAM,0);				//AF_INET = for IPV4 operations			
	if(sock==-1)									//SOCK_STREAM = for stream/connection based sockets
    {
        perror("SOCKET ERROR ");
        exit(1);
    }
    printf("Socket established\n");

    opt=1;
	optlen=sizeof(opt);
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,optlen);


    memset((void *)&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);	//INADDR_ANY binds socket to all available interfaces
    address.sin_port = htons(PORT);
   

   	check=connect(sock, (struct sockaddr *)&address, sizeof(address));
    if(check< 0)
    {
        perror("CONNECT ERROR");
        return -1;
    }
    printf("Client Connected\n");
    //puts(buffer);
    write(sock,buffer,strlen(buffer));
    if(buffer[0]=='R')
    {
    	read(sock,buffer2,LineBytes);
    	printf("\nRequired Line = \t\n");
    	puts(buffer2);
    }
    if(buffer[0]=='W')
    {
    	read(sock,buffer2,8);
    	if(buffer2[0]=='S')
    	{
    		printf("Writing Successful\n");
    	}
    	else if(buffer[1]=='F')
    	{
    		printf("Writing Failed\n");
    	}
    }	
	close(client_sock);
	close(sock);



    

}