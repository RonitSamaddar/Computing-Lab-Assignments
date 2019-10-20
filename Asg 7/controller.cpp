//HEADER INCLUSIONS
#include<iostream>//for cin,cout
#include<sys/types.h>//for ftok(),shmat(),shmdt()
#include<sys/ipc.h>//for ftok(),shmget(),shmctl()
#include<sys/shm.h>//for shmget(),shmat(),shmdt(),shmctl()
#include<cstdlib>//for exit()
#include<cstdio>//for perror()
#include<sys/stat.h>//for mknod(),open()
#include<fcntl.h>//for open()
#include<cstring>//for strcmp()
#include<unistd.h>//for read(),write(),close(),
#include<typeinfo>//for typeid()

//MACRO DEFINATIONS
#define  SHM_KEY 999//for use in shmget
#define MAX_RQ_LEN 20//maximum length of request string

using namespace std;

class User
{
	//Class for storing pipes fds corresponding to the pids
public:
	int pid;
	int gq_fd;
	int dq_fd;
	User()
	{
		this->pid=0;
		this->gq_fd=0;
		this->dq_fd=0;
	}
	User(int id,int gq,int dq)
	{
		this->pid=id;
		this->gq_fd=gq;
		this->dq_fd=dq;
	}
};
User *search(User **arr,int P,int id)
{
	//arr is the array(size=N) of pointers to objects of User class for different users
	for(int i=0;i<P;i++)
	{
		if(arr[i]->pid==id)
		{
			return arr[i];
		}
	}
	return NULL;
}


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

	//CREATING USER ARRAY
	User **arr=new User*[P];
	//cout<<typeid(arr[0]).name()<<endl;

	int shared_requests=0;//Number of read only requests currently(like search and print)
	int num_user=0;//Number of users registered
	int quser=0;//Number of users quited

	//LOOP FOR REQUESTS
	cout<<"REQUESTS := "<<endl;
	while(true)
	{
		cout<<"Waiting for new request "<<endl;
		char *req=new char[MAX_RQ_LEN+1];//Defining string for taking request
		read(rq_fd,req,MAX_RQ_LEN);//Reading request from RQ
		cout<<"\n\nRequest = "<<req<<endl;
		int upid;
		if(req[0]=='R')
		{
			//Registration Request
			if(num_user==P)
			{
				continue;
			}
			num_user++;//Number of users increases
			
			sscanf(req,"R %d",&upid);
			//Now we need to set up the corresponding GQ and DQ pipes
			char GQ[7],DQ[7];
			sprintf(GQ,"GQ%d",upid);
			sprintf(DQ,"DQ%d",upid);
			mknod(GQ,S_IFIFO|0666,0);//creation of GQ(U) pipe
			int gq_fd=open(GQ,O_WRONLY|O_NDELAY);//opening the file descriptor corresponding to the FIFO GQ
			cout<<"PIPE GQ OPENED"<<endl;
			mknod(DQ,S_IFIFO|0666,0);//creation of DQ(U) pipe
			int dq_fd=open(DQ,O_RDONLY);//opening the file descriptor corresponding to the FIFO DQ
			cout<<"PIPE DQ OPENED"<<endl;

			//Creating Entry in Users array
			arr[num_user-1]=new User(upid,gq_fd,dq_fd);
		}
		if(req[0]=='S'||req[0]=='P')
		{
			//Reading user pid and key(if present) from request
			char GQ[7],DQ[7];
			char c;
			if(req[0]=='S')
			{
				int key;
				sscanf(req,"%c %d %d",&c,&upid,&key);
			}
			else
			{
				sscanf(req,"%c %d",&c,&upid);
			}
			
			//Accessing file GQ and DQ file descriptor
			User *u=search(arr,P,upid);
			printf("User searched with pid %d,  gq_fd %d,  dq_fd %d\n",u->pid,u->gq_fd,u->dq_fd);

			/*write(u->gq_fd,&N,sizeof(N));//Grants request right away
			cout<<"Request Granted"<<endl;
			shared_requests++;//Number of read only requests currently running increases*/
		}
		else if(req[0]=='I')
		{
			/*while(shared_requests!=0)
			{
				int x;
				read(dq_fd,x,sizeof(x));//Reading for done token
				shared_requests--;
			}
			//when no more read only operations exist, C allows user to Insert
			write(gq,N,sizeof(N));//Grants Insert request
			*/
		}
		else if(req[0]=='Q')
		{

		}
	}

	for(int i=0;i<num_user;i++)
	{
		User *u=arr[i];
		close(u->gq_fd);
		close(u->dq_fd);
	}
	close(rq_fd);
	shmdt(mem);
	shmctl(shmid,IPC_RMID,NULL);
}


