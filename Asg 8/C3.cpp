/*
TO DO = 	1) Write shared memory read and write functions
			2) Initialie adjacency matrix for random graph
*/


//HEADER FILES
#include<iostream>  				// cin,cout
#include<cstdio>					// printf()
#include<cstdlib>					// exit()
#include <sys/types.h>				// ftok(), shmat()
#include <sys/ipc.h>				// ftok(), shmget()
#include <sys/shm.h>				// shmget(), shmat()


using namespace std;


/*
	SHARED MEMORY STRUCTURE =
					1 integer for N
					1 integer for P
					N*N size adjacency matrix
					N size Visited array
					N size Queue
					2 integers F,B for queue
					1 integer for Ndone
					2*P sized chunk array C
*/


//FUNCTION DEFINATIONS
int *shm_init(char *,char ,int);
void writeN(int *,int);
int readN(int *);
void writeP(int *,int);
int readP(int *);
void writeAdj(int *);
void writeEdge(int *,int,int);
int readEdge(int *,int,int);






int main()
{
	//VARIABLE DEFINATIONS
	char *ftok_file;//filename for ftok argument
	char ftok_ch;//character for ftok argument
	int N;// dimension of graph
	int P;// Number of child processes

	//PARAMETER INITIALIZATIONS
	N=5;//CHANGE TO 1000
	P=3;//CHANGE TO 4


	//GETTING SHARED MEMORY SEGMENT M
	ftok_file="C3.pdf";
	ftok_ch='C';
	int size=N*(N+2)+2*P+5;
	int *mem=shm_init(ftok_file,ftok_ch,size);

	//TEST
	writeN(mem,N);
	writeP(mem,P);
	writeAdj(mem);
	int *mem2=readAdj(mem);
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			cout<<readEdge(mem,i,j)<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
	writeEdge(mem,0,1);
	writeEdge(mem,0,2);
	writeEdge(mem,1,3);
	writeEdge(mem,2,4);

	cout<<"N = "<<readN(mem)<<endl;
	cout<<"P = "<<readP(mem)<<endl;
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			cout<<readEdge(mem,i,j)<<" ";
		}
		cout<<endl;
	}

}



int *shm_init(char *file,char ch,int size)
{
	key_t key;//key for accessing shared memory
	int shmid;//id for referencing shared memory
	int *mem;//memory pointer



	key=ftok(file,ch);//generating key
	if(key==-1)
	{
		perror("Error: ");
		exit(1);
	}

	shmid=shmget(key,size,0666|IPC_CREAT);//generating id of shared memory
	if(shmid==-1)
	{
		perror("Error: ");
		exit(1);
	}

	mem=(int *)shmat(shmid,(void *)0,0);//attaching to the shared memory
	if(*mem==-1)
	{
		perror("Error: ");
		exit(1);
	}

	return mem;

}

void writeN(int *mem,int n)
{
	*(mem)=n;
}
void writeP(int *mem,int p)
{
	*(mem+1)=p;
}
void writeAdj(int *mem)
{
	int N;
	int i,j;

	N=*(mem);
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			*(mem+2+i*N+j)=0;
		}
	}
}
void writeEdge(int *mem,int u,int v)
{
	int N;

	N=*(mem);
	*(mem+2+u*N+v)=1;
	*(mem+2+v*N+u)=1;
}



int readN(int *mem)
{
	int N;

	N=*(mem);
	return N;
}
int readP(int *mem)
{
	int P;

	P=*(mem+1);
	return P;
}
int readEdge(int *mem,int u,int v)
{
	int N;
	int E;

	N=*(mem);
	E=*(mem+2+u*N+v);
	return E;
}



