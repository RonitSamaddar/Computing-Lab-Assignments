//HEADER INCLUSIONS
#include<iostream>//for cin,cout
#include<sys/types.h>//for ftok(),shmat()
#include<sys/ipc.h>//for ftok(),shmget()
#include<sys/shm.h>//for shmget(),shmat()
#include<cstdlib>//for exit()
#include<cstdio>//for perror()
#include<sys/stat.h>//for mknod(),open()
#include<fcntl.h>//for open()
#include<cstring>//for strcmp()
#include<unistd.h>//for read(),write()

//MACRO DEFINATIONS
#define  SHM_KEY 999//for use in shmget
#define MAX_RQ_LEN 20//maximum length of request string

using namespace std;


int main()
{
	int N,P;
	cin>>N>>P;
	
	//CREATING SHARED MEMORY M
	int size=(sizeof(int)*(N*3+2));
	int shmid=shmget(SHM_KEY,size,0666|IPC_CREAT);//generating id of shared memory
	if(shmid==-1)
	{
		perror("Error: ");
		exit(1);
	}
	int *mem=(int *)shmat(shmid,(void *)0,0);//attaching to the shared memory
	if(*mem==-1)
	{
		perror("Error: ");
		exit(1);
	}
	//cout<<"We have a shared memory segment"<<endl;
	

	//CREATING REQUEST QUEUE
	mknod("RQ",S_IFIFO|0666,0);//creation of RQ(request) fifo pipe
	int rq_fd=open("RQ",O_RDWR);//opening the file descriptor corresponding to the FIFO request pipe
	cout<<"PIPE RQ OPENED FOR READING"<<endl;




	cout<<"REQUESTS := "<<endl;
	while(true)//loop for reading requests
	{
		char *req=new char[MAX_RQ_LEN];
		read(rq_fd,req,sizeof(req));
		cout<<req<<endl;
		if(strcmp(req,"EXIT")==0)
		{
			break;
		}
		if(req[0]=='R')
		{
			//Registration Request
			int upid;
			sscanf(req,"R %d",&upid);
			//Now we need to set up the corresponding GQ and DQ pipes
			char GQ[7],DQ[7];
			sprintf(GQ,"GQ%d",upid);
			sprintf(DQ,"DQ%d",upid);
			mknod(GQ,S_IFIFO|0666,0);//creation of GQ(U) pipe
			int gq_fd=open(GQ,O_WRONLY);//opening the file descriptor corresponding to the FIFO GQ
			cout<<"PIPE GQ OPENED"<<endl;
			mknod(DQ,S_IFIFO|0666,0);//creation of DQ(U) pipe
			int dq_fd=open(DQ,O_RDONLY);//opening the file descriptor corresponding to the FIFO DQ
			cout<<"PIPE DQ OPENED"<<endl;
		}
	}


	

}


