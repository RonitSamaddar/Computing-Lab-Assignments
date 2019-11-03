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
					1 integer for Ndone (ND)
					2*P sized chunk array C
*/


//FUNCTION DEFINATIONS
int *shm_init(char *,char ,int);
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

	//PARAMETER INITIALIZATIONS
	N=5;//CHANGE TO 1000
	P=3;//CHANGE TO 4


	//GETTING SHARED MEMORY SEGMENT M
	ftok_file="C3.pdf";
	ftok_ch='C';
	int size=N*(N+2)+2*P+5;
	int *mem=shm_init(ftok_file,ftok_ch,size);

	


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



