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
#define MAX_GROUPS 10

//FUNCTION PROTOTYPES
int rand_int(int d);						//gives a random integer of d digits
int validate(char *str,int mode);			//error checks the format of different client commands

//STRUCTURES
struct Client
{
	int sockfd;							//socket file descriptor of client
	int id;								//client id
};
struct Message
{
	int sourceid;						//client id sending the message
	int destid;							//client id receiving the message
	char *message;						//the message
};
struct Group
{
	int id;								//unique id of group
	int admin_id;						// id of client who is the admin of the group				
	int member_id[MAX_CLIENTS];			// array of ids of all members
	int req_sent[MAX_CLIENTS];			// array of ids of all members who have been requested
	int req_recv[MAX_CLIENTS];			// array of ids of all members who have requested to join
	int count;							// count of members of group
	int count_sent;						// count of members who have been requested
	int count_recv;						// count of members who have requested
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
	//struct Message messages[MESSAGE_CAP];//array of message records
	int i,j,k,l;						//loop variable
	fd_set readfds;						//set of file descriptors to read
	int max_fd;							//maximum fd for select()
	int client_fd;						//new client fd
	struct sockaddr_in addr_client; 	//address of client
	socklen_t length_client; 			//length of client address
	char *str,*buffer,*buffer2;			//string for receiving/sending message from client
	int cflag;							//flag if there is no space for new client
	int cid,cfd;						// variables for storing a client id and socket fd
	int flag,pflag;						// flag for other flag purposes
	int num_mem;						// number of members
	struct Group **garr;            	// array of active group
	int group_count;					// number of groups
	int invalid_id;						// storing which id is invalid
	int gid,gindex;						// variable for storing a group id, index of group in garr

	srand(time(NULL));
	str=(char *)malloc(BUFFER_CAP*sizeof(char));
	buffer=(char *)malloc(BUFFER_CAP*sizeof(char));
	buffer2=(char *)malloc(BUFFER_CAP*sizeof(char));
	group_count=0;



	//setting all client sockets to 0 initially
    for(i=0;i<MAX_CLIENTS;i++)
    {
    	clients[i].sockfd=0;
    	clients[i].id=-1;
    }
    garr=(struct Group **)malloc(MAX_GROUPS*sizeof(struct Group *));
    //printf("STAGE 1\n");
    for(i=0;i<MAX_GROUPS;i++)
	{
		garr[i]=(struct Group *)malloc((5+MAX_CLIENTS*3)*sizeof(int));
		//printf("HELLO\n");
		fflush(stdout);
		//printf("STAGE %d\n",i+2);
		garr[i]->id=-1;
		garr[i]->admin_id=-1;
		garr[i]->count=0;
		garr[i]->count_sent=0;
		garr[i]->count_recv=0;
		for(j=0;j<MAX_CLIENTS;j++)
		{
			garr[i]->member_id[j]=-1;
			garr[i]->req_sent[j]=-1;
			garr[i]->req_recv[j]=-1;
		}
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
			printf("##MASTER SOCKET ACTIVITY\n");
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
					sprintf(buffer,"SERVER\t\t\t\t:\tWelcome New Client with ID = %d",cid);
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
				sprintf(buffer,"SERVER\t\t\t\t:\tConnection limit exceeded");
				fflush(NULL);
				send(client_fd,buffer,strlen(buffer),0);
			}



		}
		else
		{
			printf("##CLIENT SOCKET ACTIVITY\n");
			for(i=0;i<MAX_CLIENTS;i++)
			{
				if(clients[i].id!=-1 && FD_ISSET(clients[i].sockfd,&readfds))
				{
					printf("##CLIENT %d ACTIVITY\n",clients[i].id);
					memset(str,'\0',BUFFER_CAP);
					//printf("#READING_CLIENT_MESSAGE\n");
					check=recv(clients[i].sockfd,str,BUFFER_CAP,0);
					if(check==-1)
					{
						perror("READ ERROR");
						exit(1);
					}
					printf("CLIENT %d\t\t:\t",clients[i].id);
					puts(str);
					if(strcmp(str,"/quit")==0)
					{
						printf("##INSIDE /quit\n");
						//printf("Client socket number %d exits\n",client_number[i]);
						//BROADCAST TO ALL OTHER CLIENTS
						for(j=0;j<MAX_CLIENTS;j++)
						{
							if(clients[j].sockfd>0 &&j!=i)
							{
								sprintf(buffer,"SERVER\t\t\t\t:\tClient %d has left the chat",clients[i].id);
								send(clients[j].sockfd,buffer,strlen(buffer),0);
							}
						}
						//exit from all groups
						for(j=0;j<MAX_GROUPS;j++)
						{
							struct Group *g=garr[j];
							if(g->admin_id==clients[i].id)
							{
								//client was the admin of the group so we have to set any other member as admin
								//if no other member delete group
								if(g->count==1)
								{
									//no other member, thus delete group
									//BROADCAST TO ALL OTHER CLIENTS
									for(k=0;k<MAX_CLIENTS;k++)
									{
										if(clients[k].sockfd>0 &&k!=i)
										{
											sprintf(buffer,"SERVER\t\t\t\t:\tGroup %d will be deleted",garr[j]->id);
											send(clients[k].sockfd,buffer,strlen(buffer),0);
										}
									}
									//delete group
									garr[j]->id=-1;
									garr[j]->admin_id=-1;
									garr[j]->count=0;
									garr[j]->count_sent=0;
									garr[j]->count_recv=0;
									for(k=0;k<MAX_CLIENTS;k++)
									{
										garr[j]->member_id[k]=-1;
										garr[j]->req_sent[k]=-1;
										garr[j]->req_recv[k]=-1;
									}
									
								}
								else
								{
									//if there are other members, set any other as admin
									for(k=0;k<MAX_CLIENTS;k++)
									{
										if(garr[j]->member_id[k]!=clients[i].id&&garr[j]->member_id[k]!=-1)
										{
											garr[j]->admin_id=garr[j]->member_id[k];
											break;
										}

									}
									//Notify the new admin
									for(l=0;l<MAX_CLIENTS;l++)
									{
										if(clients[l].id==garr[j]->admin_id)
										{
											sprintf(buffer,"SERVER\t\t\t\t:\tYou are the new admin of group %d",garr[j]->id);
											send(clients[l].sockfd,buffer,strlen(buffer),0);
											break;
										}

									}
									//delete the original client from group
									for(k=0;k<MAX_CLIENTS;k++)
									{
										if(garr[j]->member_id[k]==clients[i].id)
										{
											garr[j]->member_id[k]=-1;
											garr[j]->count--;
										}
									}
								}
							}
							else
							{
								//delete the original client from group
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(garr[j]->member_id[k]==clients[i].id)
									{
										garr[j]->member_id[k]=-1;
										garr[j]->count--;
									}
								}
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
						printf("##INSIDE /active\n");
						sprintf(buffer,"SERVER\t\t\t\t:\tOnline Clients :\n");
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
					else if(strncmp(str,"/send ",6)==0)
					{
						printf("##INSIDE /send");
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
									sprintf(buffer,"CLIENT %d\t\t\t:\t",clients[i].id);
									buffer=strcat(buffer,buffer2);
									send(clients[j].sockfd,buffer,strlen(buffer),0);
								}
							}
							if(flag==0)
							{
								sprintf(buffer,"SERVER\t\t\t\t:\tNo Such Client");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}

						}
						if(check==0)
						{
							sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Format");
							send(clients[i].sockfd,buffer,strlen(buffer),0);
						}
						
					}
					else if(strncmp(str,"/broadcast ",11)==0)
					{
						printf("##INSIDE /broadcast\n");
						buffer2=str+11;
						sprintf(buffer,"CLIENT %d(broadcast)\t\t:\t",clients[i].id);
						buffer=strcat(buffer,buffer2);
						for(j=0;j<MAX_CLIENTS;j++)
						{
							if(j!=i && clients[j].sockfd>0)
							{
								send(clients[j].sockfd,buffer,strlen(buffer),0);
							}
						}
					}
					
					else if(strncmp(str,"/makegroup ",11)==0)
					{
						//printf("INSIDE /makegroup\n");

						check=validate(str,2);
						if(check==1)
						{
							//printf("##VALID /makegroup\n");
							buffer2=str+10;
							//buffer 2= " id1 id2 id3 ....."
							//therefore len of buffer2 = 6*num_members
							num_mem=strlen(buffer2)/6;

							//Checking if all ids are valid member ids
							flag=0;
							//printf("##Checking if clients ids are valid\n");
							for(j=0;j<num_mem;j++)
							{
								sscanf(buffer2," %5d%s",&cid,buffer);
								//Adding provided client ids to group memebers list
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(clients[k].id==cid)
									{
										flag=flag+1;
										break;
									}
								}
								//Therefore flag increases by for each valid id
								buffer2=buffer2+6;
							}
							if(flag!=num_mem)
							{
								//Some client ids are not valid
								sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Client ID given");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}
							else
							{
								buffer2=str+10;
								//printf("##Clients ids are valid\n");
								//printf("##CREATING GROUP at index %d\n",group_count);
								
								if(group_count==MAX_GROUPS)
								{
									sprintf(buffer,"SERVER\t\t\t\t:\tGroup Limit Reached");
									send(clients[i].sockfd,buffer,strlen(buffer),0);
									continue;
								}
								for(j=0;j<MAX_GROUPS;j++)
								{
									if(garr[j]->admin_id==-1)
									{
										//Non allocated group
										gindex=j;
										break;

									}
								}

								//Creation of a Group
								garr[gindex]->id=rand_int(3);
								garr[gindex]->admin_id=clients[i].id;
								int temp=garr[gindex]->count;
								//Adding admin to group memebers list
								garr[gindex]->member_id[temp]=clients[i].id;
								garr[gindex]->count++;

								for(j=0;j<num_mem;j++)
								{
									puts(buffer2);
									flag=0;
									sscanf(buffer2," %5d%s",&cid,buffer);
									//printf("NEW MEMBER TO BE ADDED %d\n",cid);
									for(k=0;k<garr[gindex]->count;k++)
									{
										if(garr[gindex]->member_id[k]==cid)
										{
											flag=1;
											//printf("MEMBER ALREADY THERE\n");
											break;
										}
									}
									if(flag==0)
									{
										//Adding provided client ids to group memebers list
										int temp=garr[gindex]->count;
										//Adding admin to group memebers list
										garr[gindex]->member_id[temp]=cid;
										garr[gindex]->count++;
									}
									
									
									buffer2=buffer2+6;
								}
								sprintf(buffer,"SERVER\t\t\t\t:\tCreated Group, ID = %d",garr[gindex]->id);
								send(clients[i].sockfd,buffer,strlen(buffer),0);
								group_count++;
								printf("##Groups =\n");
								for(j=0;j<MAX_GROUPS;j++)
								{
									if(garr[j]->admin_id!=-1)
									{
										struct Group *g=garr[j];
										printf("%d %d %d\n",g->id,g->admin_id,g->count);
									}
										
								}


							}							
						}
						else
						{
							sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Format");
							send(clients[i].sockfd,buffer,strlen(buffer),0);
						}
					}					
					else if(strncmp(str,"/activegroups",13)==0)
					{
						sprintf(buffer,"SERVER\t\t\t\t:\tGroups =\n");
						printf("SERVER\t\t\t\t:\tGroups =\n");
						for(j=0;j<MAX_GROUPS;j++)
						{
							struct Group *g=garr[j];
							flag=0;
							if(g->admin_id==-1)
							{
								continue;//non-allocated group
							}
							for(k=0;k<g->count;k++)
							{
								if(g->member_id[k]==clients[i].id)
								{
									flag=1;
									break;
								}
							}
							if(flag==1)
							{
								sprintf(buffer2,"\nGroup ID:= %d\n",g->id);
								buffer=strcat(buffer,buffer2);
								sprintf(buffer2,"Group Admin ID:=%d\n",g->admin_id);
								buffer=strcat(buffer,buffer2);
								sprintf(buffer2,"Group Member_count:=%d\n",g->count);
								buffer=strcat(buffer,buffer2);
								sprintf(buffer2,"Group Members:=\n");
								buffer=strcat(buffer,buffer2);
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(g->member_id[k]==-1)
									{
										continue;
										
									}
									sprintf(buffer2,"%d ",g->member_id[k]);
									buffer=strcat(buffer,buffer2);
								}
								sprintf(buffer2,"\n");							
								buffer=strcat(buffer,buffer2);
								sprintf(buffer2,"Clients Requested:=\n");
								buffer=strcat(buffer,buffer2);
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(g->req_sent[k]==-1)
									{
										continue;
										
									}
									sprintf(buffer2,"%d ",g->req_sent[k]);
									buffer=strcat(buffer,buffer2);
								}
								sprintf(buffer2,"\n");							
								buffer=strcat(buffer,buffer2);
							}
								
						}
						send(clients[i].sockfd,buffer,strlen(buffer),0);
					}
					else if(strncmp(str,"/activeallgroups",16)==0)
					{
						sprintf(buffer,"SERVER\t\t\t\t:\tGroups =\n");
						printf("SERVER\t\t\t\t:\tGroups =\n");
						for(j=0;j<group_count;j++)
						{
							struct Group *g=garr[j];
							sprintf(buffer2,"\nGroup ID:= %d\n",g->id);
							buffer=strcat(buffer,buffer2);
							sprintf(buffer2,"Group Admin ID:=%d\n",g->admin_id);
							buffer=strcat(buffer,buffer2);
							sprintf(buffer2,"Group Member_count:=%d\n",g->count);
							buffer=strcat(buffer,buffer2);
							sprintf(buffer2,"Group Members:=\n");
								buffer=strcat(buffer,buffer2);
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(g->member_id[k]==-1)
									{
										continue;
									}
									sprintf(buffer2,"%d ",g->member_id[k]);
									buffer=strcat(buffer,buffer2);
								}
								sprintf(buffer2,"\n");							
								buffer=strcat(buffer,buffer2);
								sprintf(buffer2,"Clients Requested:=\n");
								buffer=strcat(buffer,buffer2);
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(g->req_sent[k]==-1)
									{
										continue;
									}
									sprintf(buffer2,"%d ",g->req_sent[k]);
									buffer=strcat(buffer,buffer2);
								}
								sprintf(buffer2,"\n");							
								buffer=strcat(buffer,buffer2);
						}
						send(clients[i].sockfd,buffer,strlen(buffer),0);
					}
					else if(strncmp(str,"/makegroupreq ",14)==0)
					{
						//printf("INSIDE /makegroupreq\n");

						//printf("INSIDE /makegroup\n");

						check=validate(str,3);
						if(check==1)
						{
							//printf("##VALID /makegroup\n");
							buffer2=str+13;
							//buffer 2= " id1 id2 id3 ....."
							//therefore len of buffer2 = 6*num_members
							num_mem=strlen(buffer2)/6;

							//Checking if all ids are valid member ids
							flag=0;
							//printf("##Checking if clients ids are valid\n");
							for(j=0;j<num_mem;j++)
							{
								sscanf(buffer2," %5d%s",&cid,buffer);
								//Adding provided client ids to group memebers list
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(clients[k].id==cid)
									{
										flag=flag+1;
										break;
									}
								}
								//Therefore flag increases by for each valid id
								buffer2=buffer2+6;
							}
							if(flag!=num_mem)
							{
								//Some client ids are not valid
								sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Client ID given");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}
							else
							{
								buffer2=str+13;
								//printf("##Clients ids are valid\n");
								//printf("##CREATING GROUP at index %d\n",group_count);
								
								if(group_count==MAX_GROUPS)
								{
									sprintf(buffer,"SERVER\t\t\t\t:\tGroup Limit Reached");
									send(clients[i].sockfd,buffer,strlen(buffer),0);
									continue;
								}
								for(j=0;j<MAX_GROUPS;j++)
								{
									if(garr[j]->admin_id==-1)
									{
										//Non allocated group
										gindex=j;
										break;

									}
								}

								//Creation of a Group
								garr[gindex]->id=rand_int(3);
								garr[gindex]->admin_id=clients[i].id;
								int temp=garr[gindex]->count;
								//Adding admin to group memebers list
								garr[gindex]->member_id[temp]=clients[i].id;
								garr[gindex]->count++;

								for(j=0;j<num_mem;j++)
								{
									puts(buffer2);
									flag=0;
									sscanf(buffer2," %5d%s",&cid,buffer);
									//printf("NEW MEMBER TO BE ADDED %d\n",cid);
									for(k=0;k<garr[gindex]->count;k++)
									{
										if(garr[gindex]->member_id[k]==cid)
										{
											flag=1;
											//printf("MEMBER ALREADY THERE\n");
											break;
										}
									}
									if(flag==0)
									{
										//Adding provided client ids to group memebers list
										int temp=garr[gindex]->count_sent;
										//Adding admin to group memebers list
										garr[gindex]->req_sent[temp]=cid;
										garr[gindex]->count_sent++;
										for(k=0;k<MAX_CLIENTS;k++)
										{
											if(clients[k].id==cid)
											{
												sprintf(buffer,"SERVER\t\t\t\t:\tYou are requested to join group %d",garr[gindex]->id);
												send(clients[k].sockfd,buffer,strlen(buffer),0);
											}
										}
									}
									
									
									buffer2=buffer2+6;
								}
								sprintf(buffer,"SERVER\t\t\t\t:\tCreated Group, ID = %d",garr[gindex]->id);
								send(clients[i].sockfd,buffer,strlen(buffer),0);
								group_count++;
								printf("##Groups =\n");
								for(j=0;j<MAX_GROUPS;j++)
								{
									if(garr[j]->admin_id!=-1)
									{
										struct Group *g=garr[j];
										printf("%d %d %d\n",g->id,g->admin_id,g->count);
									}
										
								}


							}							
						}
						else
						{
							sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Format");
							send(clients[i].sockfd,buffer,strlen(buffer),0);
						}
					}
					else if(strncmp(str,"/sendgroup ",11)==0)
					{
						check=validate(str,5);
						if(check==1)
						{
							sscanf(str,"/sendgroup %3d %s",&gid,buffer);
							buffer2=str+15;
							flag=0;
							for(j=0;j<MAX_GROUPS;j++)
							{
								if(garr[j]->id==gid)
								{
									gindex=j;
									flag=1;
								}
							}
							if(flag==0)
							{
								sprintf(buffer,"SERVER\t\t\t\t:\tNot a valid Group");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
								continue;
							}
							flag=0;
							for(j=0;j<MAX_CLIENTS;j++)
							{
								if(garr[gindex]->member_id[j]==clients[i].id)
								{
									//whether sender is a part of the group or not
									flag=1;
									break;

								}
							}
							if(flag==1)
							{
								sprintf(buffer,"GROUP %d, CLIENT %d\t\t:\t",gid,clients[i].id);
								buffer=strcat(buffer,buffer2);
								for(j=0;j<MAX_CLIENTS;j++)
								{
									if(garr[gindex]->member_id[j]!=-1)
									{
										for(k=0;k<MAX_CLIENTS;k++)
										{
											if(clients[k].id==garr[gindex]->member_id[j])
											{
												send(clients[k].sockfd,buffer,strlen(buffer),0);
											}
										}

									}
								}

							}
							else
							{
								sprintf(buffer,"SERVER\t\t\t\t:\tYou are not a part of the Group");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}
							

						}
						else
						{
							sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Format");
							send(clients[i].sockfd,buffer,strlen(buffer),0);
						}
					}
					else if(strncmp(str,"/joingroup ",11)==0)
					{
						check=validate(str,4);
						if(check==1)
						{
							sscanf(str,"/joingroup %3d",&gid);
							flag=0;
							for(j=0;j<group_count;j++)
							{
								if(garr[j]->id==gid)
								{
									gindex=j;
									flag=1;

								}
							}
							if(flag==0)
							{
								sprintf(buffer,"SERVER\t\t\t\t:\tNot a valid Group");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
								continue;
							}
							flag=0;
							for(j=0;j<MAX_CLIENTS;j++)
							{
								if(garr[gindex]->req_sent[j]==clients[i].id)
								{
									flag=1;
									//Removing the user from the req list
									garr[gindex]->req_sent[j]=-1;
									garr[gindex]->count_sent--;
									break;
								}
							}
							if(flag==1)
							{
								//user was in the "clients who were sent requests" list
								//user has been removed from req_sent list
								//user needs to be put in the member list
								for(j=0;j<MAX_CLIENTS;j++)
								{
									if(garr[gindex]->member_id[j]==-1)
									{
										garr[gindex]->member_id[j]=clients[i].id;
										garr[gindex]->count++;
										break;
									}
								}

								//Notifying admin of the given group
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(clients[k].id==garr[gindex]->admin_id)
									{
										sprintf(buffer,"SERVER\t\t\t\t:\tClient %d joined group %d\n",clients[i].id,garr[gindex]->id);
										send(clients[i].sockfd,buffer,strlen(buffer),0);
									}
								}

								sprintf(buffer,"SERVER\t\t\t\t:\tEntered Group %d",gid);
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}
							else
							{
								//user was not in the "clients who were sent requests" list
								sprintf(buffer,"SERVER\t\t\t\t:\tNot in request list\n");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}
						}
						else
						{
							sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Format");
							send(clients[i].sockfd,buffer,strlen(buffer),0);
						}
					}
					else if(strncmp(str,"/declinegroup ",14)==0)
					{
						check=validate(str,6);
						if(check==1)
						{
							sscanf(str,"/declinegroup %3d",&gid);
							flag=0;
							for(j=0;j<group_count;j++)
							{
								if(garr[j]->id==gid)
								{
									gindex=j;
									flag=1;

								}
							}
							if(flag==0)
							{
								sprintf(buffer,"SERVER\t\t\t\t:\tNot a valid Group");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
								continue;
							}
							flag=0;
							for(j=0;j<MAX_CLIENTS;j++)
							{
								if(garr[gindex]->req_sent[j]==clients[i].id)
								{
									flag=1;
									//Removing the user from the req list
									garr[gindex]->req_sent[j]=-1;
									garr[gindex]->count_sent--;
									break;
								}
							}
							if(flag==1)
							{
								//user was in the "clients who were sent requests" list
								//user has been removed from req_sent list
								sprintf(buffer,"SERVER\t\t\t\t:\tRemoved Request");
								send(clients[i].sockfd,buffer,strlen(buffer),0);

								//Notifying admin of the given group
								for(k=0;k<MAX_CLIENTS;k++)
								{
									if(clients[k].id==garr[gindex]->admin_id)
									{
										sprintf(buffer,"SERVER\t\t\t\t:\tClient %d did not join group %d\n",clients[i].id,garr[gindex]->id);
										send(clients[i].sockfd,buffer,strlen(buffer),0);
									}
								}
							}
							else
							{
								//user was not in the "clients who were sent requests" list
								sprintf(buffer,"SERVER\t\t\t\t:\tNot in request list\n");
								send(clients[i].sockfd,buffer,strlen(buffer),0);
							}
						}
						else
						{
							sprintf(buffer,"SERVER\t\t\t\t:\tIncorrect Format");
							send(clients[i].sockfd,buffer,strlen(buffer),0);
						}
					}
					
					


					//CLIENT REQUEST
					//printf("#READ_CLIENT_MESSAGE\n");
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