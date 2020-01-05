/*
TO DO = 	1) Initialize adjacency matrix for random graph
			2) Prints the Random Graph as per output format
*/


//HEADER FILES
#include<iostream>  				// cin,cout
#include<cstdio>					// printf()
#include<cstdlib>					// exit()
#include <sys/types.h>				// ftok(), shmat() ,semget(), semctl(), getpid()
#include <sys/ipc.h>				// ftok(), shmget(), semget(), semctl()
#include <sys/shm.h>				// shmget(), shmat()
#include <sys/sem.h>				// semget(), semctl()
#include <cmath>					// log(), ceil()
#include<unistd.h>					// getpid(), fork()

//MACRO DEFINATIONS
#define N_VALUE 1000
#define P_VALUE 4
#define EDGE_PROBABILITY 0.005 // ==0.4(if n<=10) else = 5/N

#define P(s) semop(s, &Pop, 1)  
#define V(s) semop(s, &Vop, 1)
#define P2(s) semop(s,&Pop2,1)
#define V2(s) semop(s,&Vop2,1)    


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
int search(pid_t *,int size,pid_t);








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
	int semid_Sls,semid_Sle,semid_Sq,semid_Sv;//ids for referencing semaphores
	struct sembuf Pop, Vop, Pop2, Vop2;//structures for P and V semaphore operations
	pid_t par_pid;//for storing pid of parent
	pid_t temp;//temp variable for storing pid
	pid_t *child_pids;//array for storing pids of child processes
	

	int F,B;//Variables for Queue Pointers

	int i,j;//Loop iterators
	float bl;//Probability of each edge in adjacency graph

	int level;//level of BFS traversal
	int q,rem;// quotient and remainder variables for dividing queue instance into chunk
	int start,end,count;//front and back and count of individual chunks

	int pid;//for storing child pids
	int index;//for searching index of child in child array

	int adj1[N_VALUE];//Private array for storing adjacent nodes;
	int adj2[N_VALUE];//Private array for storing adjacent nodes;
	int vertex;//Variable for adjacent node
	int adj_index;//index of adjacent array
	int ndone;//storing value of N_done



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
	cout<<endl;//Giving an extra line gap before the next output



	//PERFORMING STEP 1 OF BFS TRAVERSAL
	mem_initQ(mem);
	mem_writeQF(mem,0);//setting F = 0
	mem_writeQB(mem,-1);//setting B = 0
	mem_Qenqueue(mem,0);//setting Q[0]=0

	//PERFORMING STEP 2 OF BFS TRAVERSAL
	mem_initVisited(mem);//setting visited[i]=0 for all i
	mem_writeVisited(mem,0);//setting visited[0]=1

	//INITIALIZING SEMAPHORE ARRAYS
	semid_Sls=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semid_Sle=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semid_Sq=semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semid_Sv=semget(IPC_PRIVATE, P*P, 0777|IPC_CREAT);

	semctl(semid_Sls, 0, SETVAL, 0);
	semctl(semid_Sle, 0, SETVAL, 0);
	semctl(semid_Sq,0,SETVAL,1);
	for(i =0;i<P*P;i++)
	{
		semctl(semid_Sv,i,SETVAL,1);
	}

	//SETTING POP AND VOP STRUCTURES FOR P AND V SEMAPHORE OPERATIONS
	Pop.sem_num = Vop.sem_num = 0;
	Pop.sem_flg = Vop.sem_flg = 0;
	Pop.sem_op = -1 ; Vop.sem_op = 1 ;


	//SPAWNING P CHILD PROCESSES
	par_pid=getpid();
	child_pids=new pid_t[P]; 
	for(i=0;i<P;i++)
	{ 
		temp=fork();
		if(temp==0)
		{
			//j++;
			//idchild[kk]=getpid();
			child_pids[i]=getpid();
			printf("+++ Child %d with PID %d created\n",i,child_pids[i]);
			break;		
		}
		
			//printf("Child %d with PID %d created\n",i,tt);
	}

	if(getpid()==par_pid)
	{
		sleep(1);
		
		//PARENT's WORK

		
		F=mem_readQF(mem);
		B=mem_readQB(mem);
		cout<<endl;
		level=0;
		while(F<=B)
		{
			//LOOP FOR EACH LEVEL

			level++;
			q=(B-F+1)/P;
			rem=(B-F+1)%P;
			printf("+++ Level %d starts. Current Q position is [ %d  %d ].\n",level,F,B);
			end=F-1;
			//DIVIDING CHUNKS FOR DIFFERENT CHILDREN
			mem_initChunk(mem);
			for(i=0;i<P;i++)
			{
				start=end+1;
				count=q;
				if(rem>=(i+1))
				{
					count++;
				}
				end=start+count-1;
				mem_writeChunk(mem,i+1,start,end);
			}

			//SIGNALING S_LS SEMAPHORE FOR EACH CHILD
			for(i=0;i<P;i++)
			{
				V(semid_Sls);
			}

    		//WAITING ON S_LE SEMAPHORE FOR END OF LEVEL
    		P(semid_Sle);
    	
    		printf("+++ Level %d ends\n\n",level);
    		F=mem_readQF(mem);
    		B=mem_readQB(mem);
		}
		//LOOP ENDS WHEN ALL NODES ARE REACHED
		printf("+++ All reachable nodes are visited. Current Q position is [ %d, %d].\n",F,B);
		
		//SIGNALLING ALL CHILDREN THAT THERE ARE NO MORE REACHABLE NODES
		for(i=0;i<P;i++)
		{
			mem_writeChunk(mem,i+1,-1,B);
		}
		for(i=0;i<P;i++)
			{
				V(semid_Sls);
			}


		//DEATTACHING AND REMOVING SHARED MEMORY AND SEMAPHORES
		shmdt(mem);
		semctl(semid_Sls, 0, IPC_RMID, 0);
		semctl(semid_Sle, 0, IPC_RMID, 0);
		semctl(semid_Sq, 0, IPC_RMID, 0);
		semctl(semid_Sv, P*P, IPC_RMID, 0);	
		shmctl(shmid,IPC_RMID,NULL);
		


		
	}
	else
	{
		//CHILD's WORK

		while(1)
		{
			P(semid_Sls);
			pid=getpid();
			index=search(child_pids,P,pid);//index starts from 1( same as chunk number)

			//GETTING ITS OWN CHUNK
			start=mem_readChunk(mem,index,1);
			end=mem_readChunk(mem,index,2);
			printf("--- Process %d with PID %d gets Q chunk  [ %d  %d] \n",index-1,pid,start,end);
			if(start==-1)
			{
				//On getting end signal from parent , we signal end of level and break loop
				V(semid_Sle);
				break;
			}
			
			sleep(1.5);
			//PROCESSING ITS CHUNK
			adj_index=0;
			while(start<=end)
			{
				while(start!=mem_readQF(mem));
				vertex=mem_Qdequeue(mem);
				start++;
				

				//FINDING ADJACENT VERTICES
				for(i=0;i<N;i++)
				{
					
					if(mem_readEdge(mem,vertex,i)==1)
					{
						j=i%(P*P);
						//SETTING POP AND VOP STRUCTURES FOR P AND V SEMAPHORE OPERATIONS
						Pop2.sem_num=Vop2.sem_num=j;
						Pop2.sem_flg=Vop2.sem_flg=0;
						Pop2.sem_op = -1; Vop2.sem_op=1;
						P2(semid_Sv);
						if(mem_readVisited(mem,i)==0)
						{
							mem_writeVisited(mem,i);//setting visited[i]==1
							adj1[adj_index]=vertex;
							adj2[adj_index++]=i;//storing i in private array
						}
						V2(semid_Sv);
					}
				}
			}

			//ENQUEUEING IN QUEUE
			P(semid_Sq);
			for(j=0;j<adj_index;j++)
            {
                 vertex=adj1[j];
                 i=adj2[j];
                 mem_Qenqueue(mem,i);
                 F=mem_readQF(mem);
                 B=mem_readQB(mem);
            	 printf("    Process %d with PID %d adds the bfs link [%d %d ],and enqueue Q[  %d] = %d\n",index-1,pid,adj1[j],adj2[j],B,adj2[j]);    	 
            }
            V(semid_Sq);

            //INCREMENTING Ndone AND CHECKING IF LAST CHILD PROCESS TO EXIT
            ndone=mem_readND(mem);
            ndone++;  	 
            mem_writeND(mem,ndone);
            if(ndone==P)
            {
            	ndone=0;
            	mem_writeND(mem,0);
            	V(semid_Sle);
            }

	        
	    }
	}
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
	//chunk number starts from 1
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


int search(pid_t *arr,int size,pid_t key)
{
	int i;


	for(i=0;i<size;i++)
	{
		if(arr[i]==key)
		{
			return i+1;
		}
	}
	return -1;
}




