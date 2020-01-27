//Header file inclusion
#include<sys/types.h>					//socket(),setsockopt(),bind(),listen(),ftok()
#include<sys/socket.h>					//socket(),setsockopt(),bind(),listen()
#include<netinet/in.h>					//sockaddr_in
#include<string.h>						//memset()
#include<stdlib.h>						//exit()
#include<stdio.h>						//printf(),perror()
#include<unistd.h>						//fork(),read(),write(),close()
#include<sys/ipc.h>						//ftok(),shmget()
#include<sys/shm.h>						//shmget()
#include<math.h>						//pow()
#include<ctype.h>						//isdigit()


//MACRO defination
#define PORT 6000
#define MESSAGE_LENGTH 256
#define MAX_CLIENTS 5

struct Client
{
	int fd;
	int id;
};
struct Message
{
	int source_id;
	int dest_id;
	char *msg;
};

//Function prototypes
int rand_int(int d);						//gives a random integer of d digits
int validate(char *str,int mode);			//error checks the format of different client commands
int sigintHandler(int sig_num);				//signal handler for Ctrl+C
int printArray(struct Client *arr,int n);	//debug print function for client array




int main()
{
	//VARIABLE DECLARATIONS
	int master_sock;					//socket id of master socket
	int opt;							//options value for setsockopt()
	socklen_t optlen;					//length of options field
	int check;							//error check variable
	struct sockaddr_in address;			//socket details address structure
	int addrlen;						//Length of above address structure
	struct sockaddr_in addr_client;		//socket details address structure for client
	socklen_t length_client;			//Length of above address structure for client
	pid_t x;							//fork process id
	char *str,*buffer,*buffer2;			//char array for exchanging messages with client
	size_t mem_count_cl;				//size of shared memory
	int shmid_cl;						//id of the shared memory
	struct Client *shmad_cl;			//shared memory address
	int i,j,k,l;						//loop variable
	int flag;							//variable for flagging 
	int cid,cfd;						//process's client id,client socket fd
	int temp_cid,temp_cfd;				//client id,client socket fd


	srand(getpid());
	str=(char *)malloc(MESSAGE_LENGTH*sizeof(char));
	buffer=(char *)malloc(MESSAGE_LENGTH*sizeof(char));
	buffer2=(char *)malloc(MESSAGE_LENGTH*sizeof(char));

	//Creating Client socket,id array shared memory
	// format of data in memory
	//			num_clients,client_fd_1,client_id_1,client_fd_2,client_id_2,...................
	// size of shared memory required = 1 + MAX_CLIENTS * 2
	

	/*tok1=ftok("A4.pdf",2);
	if(tok1==-1)
	{
		perror("ftok() ERROR : ");
		exit(1);
	}
	*/

	mem_count_cl=MAX_CLIENTS;
	shmid_cl =shmget(IPC_PRIVATE,mem_count_cl*sizeof(struct Client),0777|IPC_CREAT|IPC_EXCL);
	if(shmid_cl==-1)
	{
		perror("shmget() ERROR : ");
		exit(1);
	}
	shmad_cl =(struct Client *)shmat(shmid_cl,NULL,0);
	if(shmad_cl==(struct Client *)(-1))
	{
		perror("shmat() ERROR : ");
		shmctl(shmid_cl,IPC_RMID,NULL);
		exit(1);
	}


	//Initializing client array
	for(i=0;i<MAX_CLIENTS;i++)
	{
		printf("Initializing index %d\n",i);
		(*(shmad_cl+i)).fd=0;
		(*(shmad_cl+i)).id=-1;
	}
	//printArray(shmad_cl,mem_count_cl);
	





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
		cfd = accept(master_sock, (struct sockaddr *) &addr_client,&length_client);
		if(cfd==-1)
		{
			perror("accept() ERROR : ");
	   		exit(1);
		}
		//printf("New connection accepted. Creating new process...........\n");
		int x=fork();
		if(x==0)
		{
			//Child Process
			//Corresponds to new process handling the new client
			break;
		}
	}
	printf("CLIENT HANDLING\n");
	//NEW CLIENT HANDLING
	flag=1;
	while(1)
	{
		cid=rand_int(5);
		for(j=0;j<MAX_CLIENTS;j++)
		{
			if((*(shmad_cl+j)).id==cid)
			{
				flag=0;
			}
		}
		if(flag==1)
		{
			break;
		}
		else
		{
			flag=1;
		}
	}
	
	flag=1;
	//printArray(shmad_cl,mem_count_cl);
	//printf("HELOOOOOOO\n");
	
	
	for(i=0;i<MAX_CLIENTS;i++)
	{
		if((*(shmad_cl+i)).fd==0)
		{
			(*(shmad_cl+i)).fd=cfd;
			sprintf(buffer,"SERVER\t\t\t\t:\tWelcome New Client with ID = %d",cid);
			printf("New Client Connection, ID = %d\n",cid);
			//printArray(shmad1,mem_count1);
			fflush(NULL);
			check=send(cfd,buffer,strlen(buffer),0);
			if(check==-1)
			{
				perror("send() ERROR : ");
				exit(1);
			}

			(*(shmad_cl+i)).id=cid;
			flag=0;
			break;
		}			
	}
	if(flag==1)
	{
		sprintf(buffer,"SERVER\t\t\t\t:\tConnection limit exceeded");
		fflush(NULL);
		send(cfd,buffer,strlen(buffer),0);
		if(check==-1)
		{
			perror("send() ERROR : ");
			exit(1);
		}
	}
	printArray(shmad_cl,mem_count_cl);
	while(1)
	{
		//ALL FURTHER QUERIES FROM CLIENT
		memset(str,'\0',MESSAGE_LENGTH);
		check=read(cfd,str,MESSAGE_LENGTH);
		printf("Client %d\t\t\t:\t%s\n",cid,str);
		if(check==-1)
		{
			perror("read() ERROR : ");
			exit(1);
		}
		if(strncmp(str,"/active",7)==0)	
		{
			//printf("##INSIDE /active\n");
			sprintf(buffer,"SERVER\t\t\t\t:\tOnline Clients :\n");
			for(j=0;j<MAX_CLIENTS;j++)
			{
				if((*(shmad_cl+j)).fd>0)
				{
					if(j!=i)
						sprintf(buffer2,"%d\n",(*(shmad_cl+j)).id);
					else
						sprintf(buffer2,"%d : THIS IS YOU\n",(*(shmad_cl+j)).id);
					buffer=strcat(buffer,buffer2);
				}
			}
			check=send(cfd,buffer,strlen(buffer),0);
			if(check==-1)
			{
				perror("send() ERROR : ");
				exit(1);
			}
		}
		else if(strncmp(str,"/quit",5)==0)
		{
			check=send(cfd,str,strlen(str),0);
			if(check==-1)
			{
				perror("send() ERROR : ");
				exit(1);
			}
			printf("Client %d exits\n",cid);
			for(j=0;j<MAX_CLIENTS;j++)
			{
				if((*(shmad_cl+j)).fd>0 &&j!=i)
				{
					sprintf(buffer,"SERVER\t\t\t\t:\tClient %d has left the chat",cid);
					fflush(NULL);
					check=send((*(shmad_cl+j)).fd,buffer,strlen(buffer),0);
					if(check==-1)
					{
						perror("send() ERROR : ");
						exit(1);
					}
					fflush(NULL);
				}
			}
			break;
		}
		else if(strncmp(str,"/send ",6)==0)
		{
			check=validate(str,1);
			flag=0;
			if(check==1)
			{
				sscanf(str,"/send %5d %s",&temp_cid,buffer);
				buffer2=str+12;
				//printf("/send\n");
				//printf("%5d\n%s\n",cid,buffer2);
				for(j=0;j<MAX_CLIENTS;j++)
				{
					if((*(shmad_cl+j)).id==temp_cid)
					{
						flag=1;
						sprintf(buffer,"CLIENT %d\t\t\t:\t",cid);
						fflush(NULL);
						buffer=strcat(buffer,buffer2);
						check=send((*(shmad_cl+j)).fd,buffer,strlen(buffer),0);
						if(check==-1)
						{
							perror("send() ERROR : ");
							exit(1);
						}
						fflush(NULL);
					}
				}
				if(flag==0)
				{
					sprintf(buffer,"SERVER\t\t\t\t:\tNo Such Client");
					check=send(cfd,buffer,strlen(buffer),0);
					if(check==-1)
					{
						perror("send() ERROR : ");
						exit(1);
					}
				}

			}
			if(check==0)
			{
				sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Format");
				check=send(cfd,buffer,strlen(buffer),0);
				if(check==-1)
				{
					perror("send() ERROR : ");
					exit(1);
				}
			}
			
		}
		//CLIENT REQUEST	
		//END OF CLIENT REQUEST
	}
    shmdt(shmad_cl);
	close(cfd);
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
	int count;							//count variable%6

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
			break;
		case 2:
			// /makegroup command
			//FORMAT =
			//			/makegroup <client_id1>,<client_id2>,.....
			count=0;
			for(i=11;i<strlen(str);i++)
			{
				count=(count+1)%6;
				//as all client ids must be 5 digit integers, count will increase from 1
				// to 5 across an id and then again become 0 on space
				if((count>0 && isdigit(str[i])==0)||(count==0 && str[i]!=' ' && str[i]!=','))
				{
					return 0;
				}
			}
			return 1;
			break;
		case 3:
			// /makegroupreq command
			//FORMAT =
			//			/makegroupreq <client_id1>,<client_id2>,.....
			count=0;
			for(i=14;i<strlen(str);i++)
			{
				count=(count+1)%6;
				//as all client ids must be 5 digit integers, count will increase from 1
				// to 5 across an id and then again become 0 on space
				if((count>0 && isdigit(str[i])==0)||(count==0 && str[i]!=' ' && str[i]!=','))
				{
					return 0;
				}
			}
			return 1;
			break;
		case 4:
			// /joingroup command
			//FORMAT =
			//			/joingroup <group_id>
			if(strlen(str)!=14)
			{
				return 0;
			}
			for(i=11;i<14;i++)
			{
				if(isdigit(str[i])==0)
				{
					return 0;
				}
			}
			return 1;
			break;
		case 5:
		// /sendgroup command
		//FORMAT = 
		//				/sendgroup <group_id> message
		if(str[14]!=' ')
		{
			//printf("INCORRECT SPACE\n");
			return 0;
		}
		for(i=11;i<14;i++)
		{

			if(isdigit(str[i])==0)
			{
				//printf("INCORRECT CHAR AT POSITION %d\n",i);
				return 0;
			}
		}
		return 1;
		break;
	case 6:
			// /declinegroup command
			//FORMAT =
			//			/declinegroup <group_id>
			if(strlen(str)!=17)
			{
				return 0;
			}
			for(i=14;i<17;i++)
			{
				if(isdigit(str[i])==0)
				{
					return 0;
				}
			}
			return 1;
			break;

	}
}


int printArray(struct Client *arr,int n)
{
	printf("ARRAY = \n");
	for(int i=0;i<n;i++)
	{
		struct Client c=arr[i];
		printf("%d %d ",c.fd,c.id);
	}
	printf("\n");
}