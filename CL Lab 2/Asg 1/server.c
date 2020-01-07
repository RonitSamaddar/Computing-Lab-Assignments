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

//FUNCTION PROTOTYPES
char *read_file_kth(int);							//reads the k-th line from the file
int append_file_msg(char *);						//writes message to the end of file	



int main()
{
	int sock;										//socket file descriptor
	struct sockaddr_in address;						//structure for socket address details
	int addrlen;									//variable for storing structure size
	int x;											//return of bind system call
	int y;											//return of listen system call
	struct sockaddr_in addr_client; 				//address of client
	socklen_t length_client; 						//length of client address
	int client_sock;								//new file descriptor returned by accept
	int check;										//variable for other error checks
	char *buffer,*buffer2;							//buffers for storing strings
	int opt;										//options field for setsockopt
	socklen_t optlen;								//size of options field

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
    addrlen = sizeof(address);
    x=bind(sock,(struct sockaddr *)&address,sizeof(address));
	if(x==-1)
    {
        perror("BIND ERROR");
        exit(1);
    }
    printf("Socket bound to Port %d\n",PORT);


    y=listen(sock,5);								//listen to a socket with maximum 5 pending connections
    if(y==-1)
    {
	   perror("LISTEN ERROR");
	   exit(1);
    }
    printf("Listening........\n");
	


	length_client = sizeof(addr_client);
	client_sock = accept(sock, (struct sockaddr *) &addr_client,&length_client);
	printf("Connection of client accepted\n");
	
	buffer=(char *)malloc((LineBytes+1)*sizeof(char));
	buffer2=(char *)malloc((LineBytes+1)*sizeof(char));
	memset(buffer,0,LineBytes);
	memset(buffer2,0,LineBytes);
	check = read(client_sock,buffer,LineBytes);
	if(check==-1)
	{
		perror("READ ERROR");
		exit(1);
	}
	printf("\nNew Command = %s\n",buffer);
	if(buffer[0]=='R')
	{
		//for READX k command
		int k;
		sscanf(buffer,"READX %d",&k);
		buffer2=read_file_kth(k);
		check = write(client_sock,buffer2,LineBytes);
		if(check==-1)
		{
			perror("READ ERROR");
			exit(1);
		}
	}
	if(buffer[0]=='W')
	{
		//for WRITEX msg command
		sscanf(buffer,"WRITEX %s",buffer2);
		x=append_file_msg(buffer2);
		if(x==1)
		{
			buffer2="SUCCESS";
			write(client_sock,buffer2,8);
		}
		else
		{
			buffer2="FAILURE";
			write(client_sock,buffer2,8);
		}
		
	}
	close(client_sock);
	close(sock);



    

}

char *read_file_kth(int k)
{
	FILE *fp;										//file pointer
	size_t line_size;								//variable for assigning size of a line
	char *line;										//content of the line
	int i;											//loop variables

	fp=fopen(InputFile,"r");
	line_size=(size_t)LineBytes;

	if(fp==NULL)
	{
		perror("FOPEN ERROR");
		exit(1);
	}

	line=(char *)malloc(LineBytes * sizeof(char));	//allocating memory for line
	if(line==NULL)
	{
		perror("MALLOC ERROR");
		exit(1);
	}
	
	for(i=1;i<=k;i++)
	{
		int ret=getline(&line,&line_size,fp); 		// getting i-th line from file
		if(ret==-1)
		{
			perror("GETLINE ERROR");
			exit(1);
		}
	}
	return line;									//returning k-th line
}

int append_file_msg(char *msg)
{
	FILE *fp;										//file pointer

	
	fp=fopen(InputFile,"a");


	if(fp==NULL)
	{
		perror("FOPEN ERROR");
		exit(1);
	}

	fprintf(fp,"\n%s",msg);							//writes the msg to the end of file after creating a newline
	fflush(NULL);

	return 1;										//returns 1 indicating success 

}