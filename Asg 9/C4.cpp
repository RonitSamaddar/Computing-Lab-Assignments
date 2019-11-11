//HEADER FILE
#include<iostream>				// cin, cout
#include<cstdio>				// printf()
#include<cstdlib>				// rand()
#include<pthread.h>				// for all operations on thread
#include<unistd.h>				// sleep()

//MACRO DEFINATION
#define N 1000
#define P 4
#define EDGE_PROBABILITY 0.005// ==0.4(if N<=10) else = 5/N

using namespace std;

//GLOBAL VARIABLES
int adj[N][N];//Adjacency Matrix for the Graph
int visited[N];//Visited array for all vertices
int queue[N];//Visited array for all vertices
int front,back;//Front and Back pointers for queue
int chunk[P][2];//Chunk Array
pthread_mutex_t M_Q;//Mutex for Queue access
pthread_mutex_t M_V[16];//Mutex for visited array
pthread_barrier_t L;//Barrier for synchronization

//FUNCTION PROTOTYPES
void gen_graph();//Generates a random graph
void *t_func(void *);//Function to be executed by each created thread



//MAIN FUNCTION
 int main()
 {
 	//VARIABLE DECLARATIONS
 	int i,j;//Loop Variables
 	int t_no[P];;//Numbers assigned to threads
 	pthread_t t_id[P];//IDs for th threads
 	int level;//Level of BST traversal

 	int q,rem;//quotient and remainder for chunk divisions
 	int start,end,count;//start and end and count for each chunk


 	cout<<"+++ Master thread calling setup"<<endl;
 	
 	//GENERATING THE GRAPH
 	gen_graph();
 	cout<<"    Adjacency matrix created"<<endl;


 	//QUEUE INITIALIZATION
 	queue[0]=0;
	front=0;
	back=0;
	cout<<"    Q initialized"<<endl;

	//VISITED ARRAY INITIALIZATION
	for(i=0;i<N;i++)
		visited[i]=0;
	visited[0]=1;
	cout<<"    visited[] initialized"<<endl;

	//M_Q INITIALIZATION
	pthread_mutex_trylock(&M_Q);
	pthread_mutex_unlock(&M_Q);
	cout<<"    Mutex for Q initialized"<<endl;

	//M_V INITIALIZATION
	for(i=0;i<P*P;i++)
	{
		pthread_mutex_trylock(M_V+i);
		pthread_mutex_unlock(M_V+i);
	}
	cout<<"    16 mutexes for visited[] initialized"<<endl;

	//BARRIER INITIALIZATION
	pthread_barrier_init(&L,NULL,P+1);
	cout<<"    Loop-sync barrier initialized"<<endl;

	//PRINTING GRAPH
	cout<<"\n+++ Input graph"<<endl;
	for(i=0;i<N;i++)
	{
       printf("    %d -> ",i);
       for(j=0;j<N;j++)
       {
       	if(adj[i][j]==1)
       	{
       		printf("%d ",j);
       	}
       }
       cout<<endl; 
	}
	cout<<endl;

	//CREATION OF P WORKER THREADS
	for(i=0;i<P;i++)
	{
		t_no[i]=i;
		void *p=(void *)(t_no+i);
       pthread_create(t_id+i,NULL,&t_func,p);
	}

	//REST OF EXECUTION OF MASTER THREAD
	
	sleep(1.5);
	level=0;
	//LOOP WHILE BFS RUNS
	while(front<=back)
	{
		level++;
		q=(back-front+1)/P;
		rem=(back-front+1)%P;
		end=front-1;
		printf("\n+++ Level %d starts. Current Q position is [ %4d  %4d ].\n",level,front,back);

		//DIVIDING CHUNKS FOR DIFFERENT CHILDREN
		for(i=0;i<P;i++)
		{
			start=end+1;
			count=q;
			if(rem>=(i+1))
			{
				count++;
			}
			end=start+count-1;
			chunk[i][0]=start;
			chunk[i][1]=end;


		}
		front=back+1;
    	pthread_barrier_wait(&L);
    	pthread_barrier_wait(&L);
    	printf("+++ Level %d ends\n\n",level);
	}

	//ALL REACHABLE NODES HAVE BEEN REACHED
	printf("+++ All reachable nodes are visited. Current Q position is [%4d,%4d].\n\n",front,back);

	//SIGNALLING ALL CHILD THREADS TO TERMINATE
	for(i=0;i<4;i++)
	{
		chunk[i][0]=-1;
		chunk[i][1]=0;
	}

	//WAITING FOR CHILD THREADS TO TERMINATE
	
	pthread_barrier_wait(&L);

	for(i=0;i<P;i++)
	{
	  pthread_join(t_id[i],NULL);
	  if(i==0) printf("\n+++ Master thread waits for workers to terminate\n");
	  cout<<"    Worker thread "<<t_no[i]<<" with ID "<<t_id[i]<<" terminated"<<endl;
	}

	
	//for(int l=0;l<P;l++)
	//{
	//	pthread_join(t_id[l],NULL);
	//}
	cout<<"+++ Master thread winds up"<<endl;
  	pthread_barrier_destroy(&L);
    pthread_mutex_destroy(M_V);
    pthread_mutex_destroy(&M_Q);
    exit(0);
    pthread_exit(NULL);
	





 }

void gen_graph()
{
	int i,j;//Loop Variables
	double r;//Variable for storing random decimal for edge probability

	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			 r=rand()*1.0/RAND_MAX;
			if(i!=j && r<=EDGE_PROBABILITY)
			{
				adj[i][j]=1;
			}
			else
			{
				adj[i][j]=0;
			}
		}
	}
}

void *t_func(void *tn)
{
	int i,j;//Loop Variables
	int t_no;//Thread number for the corresponding thread
	int start,end;//start and end for chunk given to this thread
	int v[N];// stores the neighbour/adjacent node
    int u[N];//stores the current node for which neighbour is being searched
    int adj_index;//Index for u[] and v[] arrays
    int vertex;//Adjacent or neighbour node;


	t_no=*((int *)tn);
	long unsigned uu=pthread_self();
	printf("+++ Worker thread %d with ID %lu starting\n",t_no,uu);
	sleep(1.5);

	//LOOP UNTIL BFS STOPS
	while(1)
	{
		//WAITING ON BARRIER
		pthread_barrier_wait(&L);

		//GETTING ITS CHUNK
		start=chunk[t_no][0];
		end=chunk[t_no][1];
		printf("--- Worker %d gets Q chunk [%4d,%4d]\n",t_no,start,end);

		sleep(1.5);

		//CHECKING FOR TERMINATION
		if(start==-1)
		{	
    			
    			printf("+++ Worker thread %d with ID %lu going to finish\n",t_no,uu);

    			break;
		}

		

    	adj_index=0;
    	while(start<=end)
    	{
    		//GETTING CURRENT NODE
    		vertex=queue[start];
    		start++;

    		//FINDING ADJACENT VERTICES
    		for(int i=0;i<N;i++)
    		{
    			if(adj[vertex][i]==1)
    			{
    				//LOCKING THE CORRESPONDING MUTEX
    				j=i%P*P;//Finding corresponding mutex index
    				pthread_mutex_lock(&M_V[j]);

    				//ADDING NEIGHBOURS TO PRIVATE ARRAY
    				if(visited[i]==0)
					{
						v[adj_index]=i;
						u[adj_index]=vertex;
						adj_index++;
						visited[i]=1;
					}
					pthread_mutex_unlock(&M_V[j]);
				}
    		}
    	}

    	//COPYING CONTENTS FROM PRIVATE ARRAY TO QUEUE
    	pthread_mutex_lock(&M_Q);
    	for(j=0;j<adj_index;j++)
    	{
    		back++;
        	queue[back]=v[j];
        	printf("    Worker thread %d adds the BFS link (%4d,%4d), and enqueues Q[%4d] =%4d\n",t_no,u[j],v[j],back,v[j]);
        }
        pthread_mutex_unlock(&M_Q);

        pthread_barrier_wait(&L);
	}	
}




