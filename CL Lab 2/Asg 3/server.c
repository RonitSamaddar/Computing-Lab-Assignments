//HEADER FILE INCLUSIONS
#include<stdio.h>						//printf()
#include<stdlib.h>						//exit()
#include<sys/types.h>					//socket(),setsockopt(),bind()
#include<sys/socket.h>					//socket(),setsockopt(),bind()
#include<netinet/in.h>					//sockaddr_in
#include<string.h>						//memset()
#include<unistd.h>						//read(),write(),close()
#include<math.h>						//pow()
#include<time.h>						//time(NULL)	



//MACRO DEFINATIONS
#define PORT 6000
#define MAX_CLIENTS 5
#define MESSAGE_CAP 256

//FUNCTION PROTOTYPES
int rand_int(int d);						//gives a random integer of d digits


int main()
{
	//VARIABLE DECLARATIONS
	int master_sock;					//socket fd for server socket
	int opt;							//option value for setsockopt
	socklen_t optlen;					//length of options
	int check;							//error check variable
	struct sockaddr_in address;			//structure for socket address details
	int addrlen;						//variable for storing structure size
	int client_sock[MAX_CLIENTS];		//socket of client
	int client_number[MAX_CLIENTS];		//each client number/id;
	int i;								//loop variable
	fd_set readfds;						//set of file descriptors to read
	int max_fd;							//maximum fd for select()
	int client_fd;						//new client fd
	struct sockaddr_in addr_client; 	//address of client
	socklen_t length_client; 			//length of client address
	char *str,*buffer;					//string for receiving/sending message from client
	int cflag;							//flag if there is no space for new client

	srand(time(NULL));
	str=(char *)malloc(MESSAGE_CAP*sizeof(char));
	buffer=(char *)malloc(MESSAGE_CAP*sizeof(char));



	//setting all client sockets to 0 initially
    for(i=0;i<MAX_CLIENTS;i++)
    {
    	client_sock[i]=0;
    	client_number[i]=0;
    }

	//Establishing socket
	master_sock=socket(AF_INET,SOCK_STREAM,0);//AF_INET = for IPV4 operations
	if(master_sock==-1)					//SOCK_STREAM = for stream/connection based sockets
    {
        perror("SOCKET ERROR ");
        exit(1);
    }
    printf("Socket established\n");


    //Setting options for the socket
    opt=1;
	optlen=sizeof(opt);
	check=setsockopt(master_sock,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,optlen);
	if(check==-1)					//SOCK_STREAM = for stream/connection based sockets
    {
        perror("SETSOCKOPT ERROR ");
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
        perror("BIND ERROR");
        exit(1);
    }
    printf("Socket bound to Port %d\n",PORT);

    //Listening at the socket for any client requests
    check=listen(master_sock,5);				//listen to a socket with maximum 5 pending connections
    if(check==-1)
    {
	   perror("LISTEN ERROR");
	   exit(1);
    }
    printf("Listening........\n");


    while(1)
	{

		FD_ZERO(&readfds);				//clearing the fd set
		FD_SET(master_sock,&readfds);	//adding master socket to fd set
		max_fd=master_sock;
		for(i=0;i<MAX_CLIENTS;i++)
		{
			if(client_sock[i]>0)		//adding all valid client sockets to fd set
			{
				FD_SET(client_sock[i],&readfds);
				if(client_sock[i]>max_fd)
				{
					max_fd=client_sock[i];
				}
			}
		}

		check=select(max_fd+1,&readfds,NULL,NULL,NULL);
		if(check==-1)
		{
			perror("SELECT ERROR :");
			exit(1);
		}
		if(FD_ISSET(master_sock,&readfds))
		{
			//the activity is in the master socket
			//that is there is a incoming connection
			client_fd=accept(master_sock,(struct sockaddr *) &addr_client,&length_client);
			int id=rand_int(5);
			cflag=1;
			for(i=0;i<MAX_CLIENTS;i++)
			{
				
				if(client_sock[i]==0)
				{
					client_sock[i]=client_fd;
					sprintf(buffer,"SERVER\t:\tWelcome New Client with ID = %d",id);
					printf("New Client Connection, ID = %d\n",id);
					fflush(NULL);
					write(client_fd,buffer,strlen(buffer));
					client_number[i]=id;
					cflag=0;
					break;
				}				
				
			}
			if(cflag==1)
			{
				sprintf(buffer,"SERVER\t:\tConnection limit exceeded");
				fflush(NULL);
				write(client_fd,buffer,strlen(buffer));
			}



		}
		else
		{
			for(i=0;i<MAX_CLIENTS;i++)
			{
				if(FD_ISSET(client_sock[i],&readfds))
				{
					memset(str,'\0',MESSAGE_CAP);
					//printf("#READING_CLIENT_MESSAGE\n");
					check=read(client_sock[i],str,MESSAGE_CAP);
					if(check==-1)
					{
						perror("READ ERROR");
						exit(1);
					}
					if(strcmp(str,"EXIT")==0)
					{
						//printf("Client socket number %d exits\n",client_number[i]);
						close(client_sock[i]);
						client_sock[i]=0;
						break;
					}
					//CLIENT REQUEST
					//printf("#READ_CLIENT_MESSAGE\n");
					printf("CLIENT %d\t:\t",client_number[i]);
					puts(str);
					sprintf(buffer,"SERVER\t:\t ");
					fflush(NULL);
					//printf("#GENERATING_REPLY\n");
					buffer=strcat(buffer,str);

					write(client_sock[i],buffer,strlen(buffer));
					//printf("SERVER SEND MESSAGE BACK TO CLIENT");

					//END OF CLIENT REQUEST
				}
			}
		}


	}









    close(master_sock);

    


}

int rand_int(int d)
{
	int ll,ul;							//lower,upper limit of range
	int r;								//random number generated

	ll=pow(10,d-1);
	ul=pow(10,d);
	r=ul;
	
	while(r==ul)
	{
		r=(int)(rand()*1.0*(ul-ll)/RAND_MAX+(float)ll);
	}
	return r;

}