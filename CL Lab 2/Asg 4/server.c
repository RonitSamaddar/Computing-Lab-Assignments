//Header file inclusion
#include<sys/types.h>					//socket(),setsockopt(),bind(),listen()
#include<sys/socket.h>					//socket(),setsockopt(),bind(),listen()
#include<netinet/in.h>					//sockaddr_in
#include<string.h>						//memset()
#include<stdlib.h>						//exit()
#include<stdio.h>						//printf(),perror()
#include<unistd.h>						//fork(),read(),write(),close()


//MACRO defination
#define PORT 6000
#define MESSAGE_LENGTH 256


//Function prototypes



int main()
{
	//VARIABLE DECLARATIONS
	int master_sock;					//socket id of master socket
	int opt;							//options value for setsockopt()
	socklen_t optlen;					//length of options field
	int check;							//error check variable
	struct sockaddr_in address;			//socket details address structure
	int addrlen;						//Length of above address structure
	int client_index;					//client index
	int client_sock;					//client socket id
	struct sockaddr_in addr_client;		//socket details address structure for client
	socklen_t length_client;					//Length of above address structure for client
	pid_t x;							//fork process id
	char *str,*buffer,*buffer2;			//char array for exchanging messages with client		


	client_index=0;
	str=(char *)malloc(MESSAGE_LENGTH*sizeof(char));
	buffer=(char *)malloc(MESSAGE_LENGTH*sizeof(char));
	buffer2=(char *)malloc(MESSAGE_LENGTH*sizeof(char));




	//Creating socket
	master_sock=socket(AF_INET,SOCK_STREAM,0);
	if(master_sock==-1)
	{
		perror("socket() ERROR : ");
		exit(1);
	}
	printf("Socket established\n");

	//Setting options for the socket
    opt=1;
	optlen=sizeof(opt);
	check=setsockopt(master_sock,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,optlen);
	if(check==-1)					//SOCK_STREAM = for stream/connection based sockets
    {
        perror("setsockopt() ERROR : ");
        exit(1);
    }
    
    //Setting details of IP address or port for the socket
    memset((void *)&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY binds socket to all available interfaces    									
    address.sin_port = htons(PORT);
    addrlen = sizeof(address);

    //Binding the socket to the given address
    check=bind(master_sock,(struct sockaddr *)&address,sizeof(address));
	if(check==-1)
    {
        perror("bind() ERROR : ");
        exit(1);
    }
    printf("Socket bound to Port %d\n",PORT);

    //Listening at the socket for any client requests
    check=listen(master_sock,5);				//listen to a socket with maximum 5 pending connections
    if(check==-1)
    {
	   perror("listen() ERROR : ");
	   exit(1);
    }
    printf("Listening........\n");


    while(1)
	{
		client_sock = accept(master_sock, (struct sockaddr *) &addr_client,&length_client);
		if(client_sock==-1)
		{
			perror("accept() ERROR : ");
	   		exit(1);
		}
		int x=fork();
		if(x==0)
		{
			//Child Process
			//Corresponds to new process handling the new client
			break;
		}
		else
		{
			client_index++;
		}
	}
	sprintf(buffer,"SERVER\t\t\t\t:\tWelcome New Client with ID = %d",client_index);
	printf("New Client Connection, ID = %d\n",client_index);
	fflush(NULL);
	send(client_sock,buffer,strlen(buffer),0);
	while(1)
	{
		memset(str,'\0',MESSAGE_LENGTH);
		check=read(client_sock,str,MESSAGE_LENGTH);
		if(check==-1)
		{
			perror("READ ERROR");
			exit(1);
		}
		if(strcmp(str,"/quit")==0)
		{
			printf("Client socket number %d exits\n",client_index);
			break;
		}
		//CLIENT REQUEST	
		printf("Client %d\t\t\t:\t%s\n",client_index,str);
		//END OF CLIENT REQUEST
	}
	close(client_sock);








}