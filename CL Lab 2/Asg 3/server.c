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
#include<ctype.h>						//isdigit()	



//MACRO DEFINATIONS
#define PORT 6000
#define MAX_CLIENTS 5
#define BUFFER_CAP 256
#define	MESSAGE_CAP 200

//FUNCTION PROTOTYPES
int rand_int(int d);						//gives a random integer of d digits
int validate(char *str,int mode);			//error checks the format of different client commands

//STRUCTURES
struct Client
{
	int sockfd;
	int id;
};
struct Message
{
	int sourceid;
	int destid;
	char *message;
};


int main()
{
	//VARIABLE DECLARATIONS
	int master_sock;					//socket fd for server socket
	int opt;							//option value for setsockopt
	socklen_t optlen;					//length of options
	int check;							//error check variable
	struct sockaddr_in address;			//structure for socket address details
	int addrlen;						//variable for storing structure size
	struct Client clients[MAX_CLIENTS];	//array of client records
	struct Message messages[MESSAGE_CAP];//array of message records
	int i,j;							//loop variable
	fd_set readfds;						//set of file descriptors to read
	int max_fd;							//maximum fd for select()
	int client_fd;						//new client fd
	struct sockaddr_in addr_client; 	//address of client
	socklen_t length_client; 			//length of client address
	char *str,*buffer,*buffer2;			//string for receiving/sending message from client
	int cflag;							//flag if there is no space for new client
	int cid,cfd;						// variables for storing a client id and socket fd
	int flag;							// flag for other flag purposes

	srand(time(NULL));
	str=(char *)malloc(BUFFER_CAP*sizeof(char));
	buffer=(char *)malloc(BUFFER_CAP*sizeof(char));
	buffer2=(char *)malloc(BUFFER_CAP*sizeof(char));



	//setting all client sockets to 0 initially
    for(i=0;i<MAX_CLIENTS;i++)
    {
    	clients[i].sockfd=0;
    	clients[i].id=-1;
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
			if(clients[i].sockfd>0)		//adding all valid client sockets to fd set
			{
				FD_SET(clients[i].sockfd,&readfds);
				if(clients[i].sockfd>max_fd)
				{
					max_fd=clients[i].sockfd;
				}
			}
		}

		//printf("ABOUT TO SELECT\n");
		check=select(max_fd+1,&readfds,NULL,NULL,NULL);

		if(check==-1)
		{
			perror("SELECT ERROR :");
			exit(1);
		}
		if(FD_ISSET(master_sock,&readfds))
		{
			//printf("MASTER SOCKET ACTIVITY\n");
			//the activity is in the master socket
			//that is there is a incoming connection
			client_fd=accept(master_sock,(struct sockaddr *) &addr_client,&length_client);
			cid=rand_int(5);
			cflag=1;
			for(i=0;i<MAX_CLIENTS;i++)
			{
				
				if(clients[i].sockfd==0)
				{
					clients[i].sockfd=client_fd;
					sprintf(buffer,"SERVER\t:\tWelcome New Client with ID = %d",cid);
					printf("New Client Connection, ID = %d\n",cid);
					fflush(NULL);
					send(client_fd,buffer,strlen(buffer),0);
					clients[i].id=cid;
					cflag=0;
					break;
				}				
				
			}
			if(cflag==1)
			{
				sprintf(buffer,"SERVER\t:\tConnection limit exceeded");
				fflush(NULL);
				send(client_fd,buffer,strlen(buffer),0);
			}



		}
		else
		{
			//printf("CLIENT SOCKET ACTIVITY\n");
			for(i=0;i<MAX_CLIENTS;i++)
			{
				if(FD_ISSET(clients[i].sockfd,&readfds))
				{
					//printf("CLIENT %d ACTIVITY\n",clients[i].id);
					memset(str,'\0',BUFFER_CAP);
					//printf("#READING_CLIENT_MESSAGE\n");
					check=recv(clients[i].sockfd,str,BUFFER_CAP,0);
					if(check==-1)
					{
						perror("READ ERROR");
						exit(1);
					}
					printf("CLIENT %d\t:\t",clients[i].id);
					puts(str);
					if(strcmp(str,"/quit")==0)
					{
						//printf("Client socket number %d exits\n",client_number[i]);
						//BROADCAST TO ALL OTHER CLIENTS
						for(j=0;j<MAX_CLIENTS;j++)
						{
							if(clients[j].sockfd>0 &&j!=i)
							{
								sprintf(buffer,"SERVER\t:\t Client %d has left the chat",clients[i].id);
								send(clients[j].sockfd,buffer,strlen(buffer),0);
							}
						}
						sprintf(buffer,"/quit");
						send(clients[i].sockfd,buffer,strlen(buffer),0);
						close(clients[i].sockfd);
						clients[i].sockfd=0;
						clients[i].id=-1;

						break;
					}
					else if(strcmp(str,"/active")==0)	
					{
						sprintf(buffer,"SERVER\t:\t Online Clients :\n");
						for(j=0;j<MAX_CLIENTS;j++)
						{
							if(clients[j].sockfd>0)
							{
								if(j!=i)
									sprintf(buffer2,"%d\n",clients[j].id);
								else
									sprintf(buffer2,"%d : THIS IS YOU\n",clients[j].id);
								buffer=strcat(buffer,buffer2);
							}
						}
						send(clients[i].sockfd,buffer,strlen(buffer),0);
					}
					else if(strncmp(str,"/send",5)==0)
					{
						printf("send command\n");
						check=validate(str,1);
						flag=0;
						if(check==1)
						{
							sscanf(str,"/send %5d %s",&cid,buffer2);
							buffer2=str+12;
							//printf("/send\n");
							//printf("%5d\n%s\n",cid,buffer2);
							for(j=0;j<MAX_CLIENTS;j++)
							{
								if(clients[j].id==cid)
								{
									flag=1;
									sprintf(buffer,"CLIENT %d\t:\t",clients[i].id);
									buffer=strcat(buffer,buffer2);
									send(clients[j].sockfd,buffer,strlen(buffer),0);
								}
							}
							if(flag==0)
							{
								sprintf(buffer,"SERVER\t:\t: No Such Client");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}

						}
						if(check==0)
						{
							sprintf(buffer,"SERVER\t:\t Incorrect Format");
							send(clients[i].sockfd,buffer,strlen(buffer),0);
						}
						
					}

					//CLIENT REQUEST
					//printf("#READ_CLIENT_MESSAGE\n");
					//printf("SERVER SEND MESSAGE BACK TO CLIENT");

					//END OF CLIENT REQUEST
					break;
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

int validate(char *str,int mode)
{
	int i;								//Loop variable

	switch(mode)
	{
		case 1:
			// /send command
			//FORMAT =
			//			/send dest_id message
			//printf("DIGITS TO BE CHECKED\n");
			for(i=6;i<11;i++)
			{
				if(isdigit(str[i])==0)
				{
					return 0;
				}

			}
			//printf("DIGITS CHECKED\n");
			if(str[5]!=' '||str[11]!=' ')
				return 0;
			//printf("BLANKS CHECKED\n");
			return 1;
	}
}