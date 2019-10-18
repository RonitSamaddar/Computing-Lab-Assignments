//HEADER INCLUSIONS
#include<iostream>//for cin,cout
#include<sys/types.h>//for ftok(),shmat()
#include<sys/ipc.h>//for ftok(),shmget()
#include<sys/shm.h>//for shmget(),shmat()
#include<cstdlib>//for exit()
#include<cstdio>//for perror()

//MACRO DEFINATIONS
#define  shm_key 999//for use in shmget

using namespace std;


int main()
{
	int N,P;
	cin>>N>>P;
	
	int size=(sizeof(int)*(N*3+2));
	int shmid=shmget(shm_key,size,0666|IPC_CREAT);//generating id of shared memory
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
	cout<<"We have a shared memory segment"<<endl;
	getchar();
	getchar();


	

}


