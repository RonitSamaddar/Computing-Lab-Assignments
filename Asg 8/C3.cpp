/*
TO DO = 	1) Initialize adjacency matrix for random graph
			2) Prints the Random Graph as per output format
*/


//HEADER FILES
#include<iostream>  				// cin,cout
#include<cstdio>					// printf()
#include<cstdlib>					// exit()
#include <sys/types.h>				// ftok(), shmat()
#include <sys/ipc.h>				// ftok(), shmget()
#include <sys/shm.h>				// shmget(), shmat()
#include <cmath>					// log(), ceil()

//MACRO DEFINATIONS
#define N_VALUE 10
#define P_VALUE 3
#define EDGE_PROBABILITY 0.4 // ==0.4(if n<=10) else = 5/N


using namespace std;


/*
	SHARED MEMORY STRUCTURE =
					1 integer for N
					1 integer for P
					N*N size adjacency matrix
					N size Visited array
					N size Queue
					2 integers F,B for queue
					1 integer for Ndone (ND)
					2*P sized chunk array C
*/


//FUNCTION DEFINATIONS
void mem_writeN(int *,int);
int mem_readN(int *);
void mem_writeP(int *,int);
int mem_readP(int *);
void mem_initAdj(int *);
void mem_writeEdge(int *,int,int);
int mem_readEdge(int *,int,int);
void mem_initVisited(int *);
void mem_writeVisited(int *,int);
int mem_readVisited(int *,int);
void mem_initQ(int *);
void mem_writeQF(int *,int);
void mem_writeQB(int *,int);
void mem_Qenqueue(int *,int);
int mem_readQF(int *);
int mem_readQB(int *);
int mem_Qdequeue(int *);
void mem_Qprint(int *);
void mem_writeND(int *,int d);
int mem_readND(int *);
void mem_initChunk(int *);
void mem_writeChunk(int *,int,int,int);
int mem_readChunk(int *,int,int);
void mem_printChunk(int *);







int main()
{
	//VARIABLE DEFINATIONS
	char *ftok_file;//filename for ftok argument
	char ftok_ch;//character for ftok argument
	int N;// dimension of graph
	int P;// Number of child processes
	long int size;//size of shared memory required
	int *mem;//Pointer to shared memory
	key_t key;//key for accessing shared memory
	int shmid;//id for referencing shared memory

	int i,j;//Loop iterators
	float bl;//Probability of each edge in adjacency graph


	//PARAMETER INITIALIZATIONS
	N=N_VALUE;
	P=P_VALUE;


	//GETTING SHARED MEMORY SEGMENT M
	ftok_file="C3.cpp";
	ftok_ch='D';
	size=100000000;
	key=ftok(ftok_file,ftok_ch);//generating key
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
	//cout<<"HELLO1"<<endl;
	mem_writeN(mem,N);
	//cout<<"HELLO2"<<endl;
	mem_writeP(mem,P);
	//cout<<"HELLO3"<<endl;

	

	//GENERATING ADACENCY MATRIX FOR RANDOM GRAPH
	mem_initAdj(mem);
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			if(i!=j)
			{
				bl=rand()*1.0/RAND_MAX;
				//cout<<bl<<" ";
				if(bl<=EDGE_PROBABILITY)
				{
					mem_writeEdge(mem,i,j);
				}
			}

		}
	}
	cout<<endl;
	cout<<"+++ Input graph"<<endl;
	for(int i=0;i<N;i++)
	{
		printf("    %4d -> ",i);
		for(int j=0;j<N;j++)
		{
			if(mem_readEdge(mem,i,j)==1)
			{
				printf("%4d ",j);
			}			
		}
		cout<<endl;
	}

	
	shmdt(mem);
	shmctl(shmid,IPC_RMID,NULL);

}


void mem_writeN(int *mem,int n)
{
	*(mem)=n;
}
void mem_writeP(int *mem,int p)
{
	*(mem+1)=p;
}
void mem_initAdj(int *mem)
{
	int N;
	int i,j;

	N=*(mem);
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			//cout<<"HELLO"<<i<<j<<endl;
			*(mem+2+i*N+j)=0;
		}
	}
}
void mem_writeEdge(int *mem,int u,int v)
{
	int N;

	N=*(mem);
	*(mem+2+u*N+v)=1;
	*(mem+2+v*N+u)=1;
}
void mem_initVisited(int *mem)
{
	int N;
	int i;

	N=*(mem);
	for(i=0;i<N;i++)
	{
		*(mem+2+N*N+i)=0;
		
	}
}
void mem_writeVisited(int *mem,int vert)
{
	int N;

	N=*(mem);
	*(mem+2+N*N+vert)=1;
}
void mem_initQ(int *mem)
{
	int F=0;
	int B=-1;
	mem_writeQF(mem,F);
	mem_writeQB(mem,B);
}
void mem_writeQF(int *mem,int F)
{
	int N;

	N=*(mem);
	*(mem+2+N*N+N)=F;
}
void mem_writeQB(int *mem,int B)
{
	int N;

	N=*(mem);
	*(mem+2+N*N+N+1)=B;
}
void mem_Qenqueue(int *mem,int key)
{
	int B;
	int N;

	N=*(mem);
	B=*(mem+2+N*N+N+1);
	B++;
	*(mem+2+N*N+N+1)=B;

	*(mem+2+N*N+N+2+B)=key;
}
void mem_writeND(int *mem,int d)
{
	int N;

	N=*(mem);
	*(mem+2+N*N+N+2+N)=d;
}
void mem_initChunk(int *mem)
{
	int N;
	int P;

	N=*(mem);
	P=*(mem+1);
	for(int i=0;i<2*P;i++)
	{
		*(mem+2+N*N+N+2+N+1+i)=0;
	}

}
void mem_writeChunk(int *mem,int chunk_no,int P1,int P2)
{
	int N;
	int P;

	N=*(mem);
	P=*(mem+1);
	*(mem+2+N*N+N+2+N+1+(chunk_no-1)*2)=P1;
	*(mem+2+N*N+N+2+N+1+(chunk_no-1)*2+1)=P2;
}




int mem_readN(int *mem)
{
	int N;

	N=*(mem);
	return N;
}
int mem_readP(int *mem)
{
	int P;

	P=*(mem+1);
	return P;
}
int mem_readEdge(int *mem,int u,int v)
{
	int N;
	int E;

	N=*(mem);
	E=*(mem+2+u*N+v);
	return E;
}
int mem_readVisited(int *mem,int vert)
{
	int N;
	int V;

	N=*(mem);
	V=*(mem+2+N*N+vert);
	return V;
}
int mem_readQF(int *mem)
{
	int N;
	int F;

	N=*(mem);
	F=*(mem+2+N*N+N);
	return F;
}
int mem_readQB(int *mem)
{
	int N;
	int B;

	N=*(mem);
	B=*(mem+2+N*N+N+1);
	return B;
}
int mem_Qdequeue(int *mem)
{
	int F;
	int N;
	int key;

	N=*(mem);
	F=*(mem+2+N*N+N);
	key=*(mem+2+N*N+N+2+F);
	F++;
	*(mem+2+N*N+N)=F;
	return key;
}

void mem_Qprint(int *mem)
{
	int F;
	int B;
	int N;
	int i;
	int key;

	N=*(mem);
	F=*(mem+2+N*N+N);
	B=*(mem+2+N*N+N+1);
	for(i=F;i<=B;i++)
	{
		key=*(mem+2+N*N+N+2+i);
		cout<<key<<" ";
	}
	cout<<endl;
}
int mem_readND(int *mem)
{
	int N;
	int D;

	N=*(mem);
	D=*(mem+2+N*N+N+2+N);
	return D;
}
int mem_readChunk(int *mem,int chunk_no,int index)
{
	int N;
	int P;
	int C;

	N=*(mem);
	P=*(mem+1);
	C=*(mem+2+N*N+N+2+N+1+(chunk_no-1)*2+index-1);
	return C;
}
void mem_printChunk(int *mem)
{
	int N;
	int P;
	int key;
	int i;

	N=*(mem);
	P=*(mem+1);
	for(i=0;i<2*P;i++)
	{
		key=*(mem+2+N*N+N+2+N+1+i);
		cout<<key<<" ";
	}
	cout<<endl;

}




