//HEADER FILES
#include<stdio.h>						// printf()
#include<stdlib.h>						// malloc()
#include<string.h>						// strlen()
#include<math.h>						// pow()
#include<time.h>						// clock()
#include<sys/socket.h>					// socket(),setsockopt(),bind()
#include<sys/types.h>					// socket(),setsockopt(),bind()
#include<netinet/in.h>					// sockaddr_in
#include<unistd.h>						// fork(),read(),write(),close()



//MACRO DEFINATIONS
#define WORD_LENGTH 256
#define MAX_OPERAND 30
#define PORT 6000
#define MAX_CLIENTS 20
#define LOG_FILE "server_records.txt"


//FUNCTION PROTOTYPES
char *eval_postfix(char *);				// function to evaluate postfix given in string and return result or error


//MAIN FUNCTION
int main()
{
	
	char *str;							//string input from client
	clock_t start,end;					//start,end times for client request
	double duration;					//elapsed times for client request
	int master_sock;					//Variable for socket
	int opt;							//option value for setsockopt
	socklen_t optlen;					//length of options
	struct sockaddr_in address;			//structure for socket address details
	int addrlen;						//variable for storing structure size
	int check;							//error check variable
	fd_set readfds;						//set of file descriptors to read
	int client_sock[MAX_CLIENTS];		//socket of client
	struct sockaddr_in addr_client; 	//address of client
	socklen_t length_client; 			//length of client address
	char *buffer;						//buffer string for message to send to client
	FILE *fp;							//file pointer for log file
	int i;								//loop variable
	int max_fd;							//maximum fd for select()
	int client_fd;						//new client fd
	int client_number[MAX_CLIENTS];		//each client number/id;
	int client_count;					//number of clients






	str=(char *)malloc(WORD_LENGTH*sizeof(char));
	buffer=(char *)malloc(WORD_LENGTH*sizeof(char));



	//Socket Establishment
	master_sock=socket(AF_INET,SOCK_STREAM,0);		
										//AF_INET = for IPV4 operations
										//SOCK_STREAM = for stream/connection based sockets
	if(master_sock==-1)							
    {
        perror("SOCKET ERROR ");
        exit(1);
    }
    printf("Socket established\n");


    //Setting options for the socket
    opt=1;
	optlen=sizeof(opt);
	setsockopt(master_sock,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,optlen);

	//Setting details of IP address or port for the socket
    memset((void *)&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);	
    									//INADDR_ANY binds socket to all available interfaces
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
    check=listen(master_sock,10);				//listen to a socket with maximum 10 pending connections
    if(check==-1)
    {
	   perror("LISTEN ERROR");
	   exit(1);
    }
    printf("Listening........\n");

    //Opening server records file
    fp=fopen(LOG_FILE,"w");

    //setting all client sockets to 0 initially
    for(i=0;i<MAX_CLIENTS;i++)
    {
    	client_sock[i]=0;
    	client_number[i]=0;
    }
    client_count=0;


	length_client = sizeof(addr_client);
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
			client_count++;
			printf("Connected with client socket number %d\n",client_count);
			fflush(NULL);
			for(i=0;i<MAX_CLIENTS;i++)
			{
				
				if(client_sock[i]==0)
				{
					client_sock[i]=client_fd;
					client_number[i]=client_count;
					break;
				}				
				
			}


		}
		else
		{
			for(i=0;i<MAX_CLIENTS;i++)
			{
				if(FD_ISSET(client_sock[i],&readfds))
				{
					memset(str,'\0',WORD_LENGTH);
					check=read(client_sock[i],str,WORD_LENGTH);
					if(check==-1)
					{
						perror("READ ERROR");
						exit(1);
					}
					if(strcmp(str,"EXIT")==0)
					{
						printf("Client socket number %d exits\n",client_number[i]);
						close(client_sock[i]);
						client_sock[i]=0;
						break;
					}
					//CLIENT REQUEST
					start=clock();		
					printf("Client socket number %d sent message : %s\n",client_number[i],str);
					buffer=eval_postfix(str);
					printf("Sending reply : %s\n",buffer);
					write(client_sock[i],buffer,strlen(buffer));
					end=clock();
					//END OF CLIENT REQUEST
					
					duration=(double)(end - start)/(double)CLOCKS_PER_SEC;
					fprintf(fp,"%d\t\t%s\t\t%s\t\t%lf sec\n",client_number[i],str,buffer,duration);
					fflush(NULL);
				}
			}
		}


	}
	close(master_sock);
	/*
	
	
	while(1)
	{
		client_sock = accept(sock, (struct sockaddr *) &addr_client,&length_client);
		int x=fork();
		if(x==0)
		{
			//Child Process
			break;
		}
		else
		{
			client_index++;
		}
	}
	printf("Connected with client socket number %d\n",client_index);
	while(1)
	{
		memset(str,'\0',WORD_LENGTH);
		check=read(client_sock,str,WORD_LENGTH);
		if(check==-1)
		{
			perror("READ ERROR");
			exit(1);
		}
		if(strcmp(str,"EXIT")==0)
		{
			printf("Client socket number %d exits\n",client_index);
			break;
		}
		//CLIENT REQUEST
		start=clock();		
		printf("Client socket number %d sent message : %s\n",client_index,str);
		buffer=eval_postfix(str);
		printf("Sending reply : %s\n",buffer);
		write(client_sock,buffer,strlen(buffer));
		end=clock();
		//END OF CLIENT REQUEST
		
		duration=(double)(end - start)/(double)CLOCKS_PER_SEC;
		fprintf(fp,"%d\t\t%s\t\t%s\t\t%lf sec\n",client_index,str,buffer,duration);
		fflush(NULL);
		
		
	}
	close(client_sock);
	*/
}


//OTHER FUNCTION
char *eval_postfix(char *str)
{

	int len;						//length of string
	char *ret;						//return value(either result of operation or error message)
	int i;							//loop variable
	char c;							//variable to store and process each character
	double arr[MAX_OPERAND];		//array to store numbers in the postfix
	int top;						//variable for indexing the array
	double token;					//getting each token(number) in the postfix expression
	int nflag;						//flag that the current token is a number
	int fflag;						//flag that the current number is a floating point number
	int x;							//power of 10 to divide with
	double na,nb,nc;				//variable for calculations		




	len=strlen(str);
	top=-1;
	token=0;
	ret=(char *)malloc(WORD_LENGTH*sizeof(int));
	nflag=0;
	fflag=0;
	x=10;
	for(i=0;i<len;i++)
	{
		c=str[i];


		//Checking Type Of c		
		//Type 1:Alphabet
		if(isalpha(c)!=0)
		{
			//letters should not be there;
			ret="ERROR : ONLY NUMBERS AND OPERATORS(+,-,*,/) ALLOWED";
			return ret;
		}
		//Type 2:Digit
		else if(isdigit(c)!=0)
		{
			nflag=1;//flagging that current token is a number
			if(fflag==1)
			{
				//Already decimal point occured before
				token=token+((0+(c-'0'))*1.0/x);
				x=x*10;
			}
			else
			{
				//decimal point not yet encountered
				token=token*10+(0+(c-'0'));
			}
		}
		else if(c=='.')
		{
			if(((i==0)||(isdigit(str[i-1])==0))&&((i==len-1)||(isdigit(str[i+1])==0)))
			{//if there is no digit before and also after the '.', then error
				ret="ERROR : INCORRECT '.' IN STRING";
				return ret;
			}
			else if(fflag==1)
			{
				//if two '.' in a single number than also error
				ret="ERROR : INCORRECT '.' IN STRING";
				return ret;
			}
			else
			{
				fflag=1;
			}
		}
		else if(c=='+'||c=='-'||c=='*'||c=='/')
		{
			if(((i>0)&&(str[i-1]!=' '))||((i<len-1)&&(str[i+1]!=' ')))
			{
				//if either previous or next character is not a blank,then error
				sprintf(ret,"ERROR: ILLEGAL PLACEMENT OF OPERATOR %c",c);
				return ret;
			}
			if(top<=0)
			{
				//if two former operands are not there,then error
				sprintf(ret,"ERROR: NOT ENOUGH OPERANDS FOR %c",c);
				return ret;
			}
			else
			{
				//valid operator
				nb=arr[top--];
				na=arr[top--];
				if(c=='+')
				{
					nc=na+nb;
				}
				else if(c=='-')
				{
					nc=na-nb;
				}
				else if(c=='*')
				{
					nc=na*nb;
				}
				else if(c=='/')
				{
					nc=na/nb;
				}
				arr[++top]=nc;
			}
		}
		else if(c==' ')
		{
			//processing of token
			if(nflag==1)
			{
				arr[++top]=token;
			}
			

			nflag=0;
			fflag=0;
			x=10;
			token=0;
		}
		else
		{
			ret="ERROR : ONLY NUMBERS AND OPERATORS(+,-,*,/) ALLOWED";
			return ret;
		}
		
	}
	//processing of token
	if(nflag==1)
	{
		arr[++top]=token;
	}

	if(top>0)
	{
		ret="ERROR : NOT ENOUGH OPERATORS";
		return ret;
	}
	
	sprintf(ret,"%lf",arr[0]);
	return ret;
}