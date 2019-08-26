#include <iostream>
#include <cstdio>
#include <cstdlib>
#include<cmath>
#include<queue>
#include<typeinfo>

using namespace std;

//class prototypes 
class Node;
class LinkedList;
class Graph;


//function prototypes
LinkedList *track_path(int *parent,int curr,int start);
void print_queue(queue <int>q);
void print_path(LinkedList *path,Graph *GS);


//class

class Node
{
	//class for representing each node of linked lists in adjacency list
private:
	int index;
	Node *next;
public:
	//Constructors
	Node()
	{
		this->index=-1;
		this->next=NULL;
	}
	Node(int k)
	{
		this->index=k;
		this->next=NULL;
	}

	//Functions for getting and setting data items
	int get_index()
	{
		return this->index;
	}
	Node* get_next()
	{
		return this->next;
	}
	void set_next(Node *p)
	{
		this->next=p;
	}
};

class LinkedList
{
	//class for representing LinkedList of nodes as in Adjacency list
public:
	Node *start;
	int count;
	//Constructor
	LinkedList()
	{
		this->start=NULL;
		this->count=0;
	}


	int isEmpty()
	{
		//Function to check if list is empty
		return(this->count==0);
	}

	void insert(int v)
	{
		Node *p=new Node(v);
		if(this->start==NULL)
		{
			this->start=p;
		}
		else
		{
			p->set_next(this->start);
			this->start=p;
		}
	}

	int search(int v)
	{
		Node *curr=this->start;
		while(curr!=NULL)
		{
			int index=curr->get_index();
			if(index==v)
				return 1;
			curr=curr->get_next();
		}
		return 0;
	}


};

class Graph
{
	//class for representing the Graph structure
private:
	int n;//Number of vertices
	int m;//Number of edges
	LinkedList **adj_list;//Array of pointers to the linkedlists
	int *lv;//Array of vertex weights
public:
	//Constructors
	Graph(int n)
	{
		this->n=n;
		this->m=0;
		this->adj_list=new LinkedList*[this->n];
		//Alloting memory for each of the linked lists
		for(int i=0;i<this->n;i++)
		{
			this->adj_list[i]=new LinkedList();
		}
		//Weights
		this->lv=new int[this->n];
		for(int i=0;i<this->n;i++)
		{
			this->lv[i]=0;
		}
	}
	Graph(int n,int m)
	{
		this->n=n;
		this->m=m;
		this->adj_list=new LinkedList*[this->n];
		//Alloting memory for each of the linked lists
		for(int i=0;i<this->n;i++)
		{
			this->adj_list[i]=new LinkedList();
		}
		this->lv=new int[this->n];
		//Weights
		for(int i=0;i<this->n;i++)
		{
			this->lv[i]=0;
		}
	}
	int get_n()
	{
		return this->n;
	}
	int get_m()
	{
		return this->m;
	}
	void set_weight(int index,int weight)
	{
		//function to assign the weight of a particular vertex
		this->lv[index]=weight;
	}

	int get_weight(int index)
	{
		//function to get the weight of a vertex
		return this->lv[index];
	}

	LinkedList **get_edges()
	{
		//function to return the adjacency list of a graph
		return this->adj_list;
	}
	
	void add_edge_un(int u,int v)
	{
		//function to add an edge from u-index vertex to v-index vertex for undirected graph
		this->adj_list[u]->insert(v);
		this->adj_list[v]->insert(u);
	}
	void add_edge_dir(int u,int v)
	{
		//function to add an edge from u-index vertex to v-index vertex for directed graph
		this->adj_list[u]->insert(v);
	}
	int is_edge(int u,int v)
	{
		//function for searching if edge is there between u and v;
		return adj_list[u]->search(v);
	}

	void GR_rec(LinkedList **adj,int u,int curr,int lu,int * visited)
	{

		//Recursive function called by form_GR where curr is the present node reached 
		//and lu represents the path length still life.
		//We need to call this function for all u's
		
		visited[curr]=1;//setting present node as visited

		if(lu==0)
		{
			//we have reached end of path
			if(this->adj_list[u]->search(curr)==0)
			{
				//we only add the edge if it is already not there
				this->add_edge_dir(u,curr);
				this->m++;
			}
		}
		else
		{
			Node *st=adj[curr]->start;
			while(st!=NULL)
			{
				//traversing over neighbours of current node
				int index=st->get_index();
				if(visited[index]==0)
					GR_rec(adj,u,index,lu-1,visited);
				st=st->get_next();
			}
		} 
		visited[curr]=0;//as we backtrack, we set present node as unvisited for other paths

	}
	void form_GR(Graph *G)
	{
		//function of reachability Graph GR to form its adj matrix and number of edges 
		//by analysing in G to find all vertices v which are at path of length lu
		//from vertices u of Graph G
		int G_n=G->get_n();
		int *visited=new int[G_n];

		//setting weights for the new graph
		for(int i=0;i<G_n;i++)
		{
			visited[i]=0;
			int weight=G->get_weight(i);
			this->set_weight(i,weight);
		}
		

		LinkedList **adj=G->get_edges();
		//forming adj_list of new Graph
		for(int i=0;i<G_n;i++)
		{
			this->GR_rec(adj,i,i,G->get_weight(i),visited);

						
		}

	}

	void form_GS(Graph *G,Graph* GR)
	{
		int nV=G->get_n();
		int nVs=nV*nV;
		int u,v;

		//INITIALIZING WEIGHTS!!!!!
		for(int i=0;i<nVs;i++)
		{
			v=i%nV;
			u=i/nV;
			int w_v=G->get_weight(v);
			int w_u=G->get_weight(u);

			this->set_weight(i,w_u+w_v);
		}

		//ADDING EDGES
		LinkedList **adj=GR->get_edges();
		for(u=0;u<nV;u++)
		{
			Node *curr1=adj[u]->start;
			while(curr1!=NULL)
			{
				//Each Iteration of this loop corresponds to one edge of
				//graph for first robot
				int u1=curr1->get_index();
				
				for(int v=0;v<nV;v++)
				{
					Node *curr2=adj[v]->start;
					while(curr2!=NULL)
					{
						//Each Iteration of this loop corresponds to one edge of
						//graph for second robot
						int v1=curr2->get_index();
						int w1=u*nV+v;
						int w2=u1*nV+v1;
						if(this->adj_list[w1]->search(w2)==0)
						{	this->add_edge_dir(w1,w2);
							this->m++;
						}
						curr2=curr2->get_next();
					}
					
				}
				curr1=curr1->get_next();
			}
		}

		

	}

	void BFS_find_path(int s,int t,int mode)
	{
		
		//Finding number of vertices of original and of simultaneous reachability graph
		int nVs=this->get_n();
		int nV=sqrt(nVs);

		//Getting adjacency list of simultaneous reachability graph
		LinkedList **adj=this->get_edges();

		//Initializing parameters required for BFS path search
		queue <int>q;
		int visited[nVs];//to mark nodes already visited
		int parent[nVs];//to mark parent of each node in path
		for(int i=0;i<nVs;i++)
		{
			if(mode==1 && i%nV==i/nV)
				visited[i]=1;
			else
				visited[i]=0;
			parent[i]=-1;
		}
		int start=s*nV+t;
		int x=start;
		int dest=t*nV+s;



		//Running BFS
		//cout<<"BFS STARTED!!"<<endl;
		//cout<<"Finding path from "<<start<<" to "<<dest<<endl;
		visited[x]=1;
		while(x!=dest)
		{
			

			
			Node *curr=adj[x]->start;
			while(curr!=NULL)
			{
				int index=curr->get_index();
				//cout<<"Index = "<<index<<endl;
				if(visited[index]==0)
				{
					q.push(index);
					visited[index]=1;
					parent[index]=x;
				}
				curr=curr->get_next();
			}

			//cout<<"Current node = "<<x<<endl;
			//cout<<"VISITED : "<<endl;
			//for(int i=0;i<nVs;i++)
			//{
			//	cout<<"["<<i<<"] = "<<visited[i]<<endl;
			//}
			//cout<<"QUEUE : "<<endl;
			//print_queue(q);
			//getchar();

			x=q.front();
			q.pop();
		}

		//Getting the path as a linked list
		if(x==dest)
		{
			LinkedList *path=track_path(parent,x,start);
			print_path(path,this);
		}
		
	}

	void prngraph()
	{
		//function to print graph in format according to sample outputs
		printf("    Number of vertices:  %d\n",this->n);
		printf("    Number of edges   :  %d\n",this->m);;
		for(int i=0;i<n;i++)
		{
			printf("    %2d  (%d) ->  ",i,this->lv[i]);
			Node *st=adj_list[i]->start;
			if(st!=NULL)
			{
				while(st->get_next()!=NULL)
				{
					printf("%2d, ",st->get_index());
					st=st->get_next();

				}
				printf("%2d",st->get_index());
			}
			printf("\n");


		}
	}


};

LinkedList *track_path(int *parent,int curr,int start)
{
	//forms the paths by backtracking along parent links
	LinkedList *path=new LinkedList();
	path->insert(curr);
	while(curr!=start)
	{
		curr=parent[curr];
		path->insert(curr);
	}
	return path;
}
void print_queue(queue <int>q)
{
	int x=q.front();
	while(!q.empty())
	{
		cout<<x<<" ";
		q.pop();
		x=q.front();
	}
	cout<<endl;
}
void print_path(LinkedList *path,Graph *GS)
{
	//prints points in the path, length of path, and total effort
	//Graph GS object required for getting weights of the points
	int nVs=GS->get_n();
	int nV=sqrt(nVs);
	Node *curr=path->start;
	int num_steps=0;
	int effort=0;
	while(curr!=NULL)
	{
		int x=curr->get_index();
		int v=x%nV;
		int u=x/nV;
		cout<<"("<<u<<","<<v<<") ";
		if(curr->get_next()!=NULL)
		{
			num_steps++;
			effort=effort+GS->get_weight(x);
		}
		curr=curr->get_next();
		
	}
	cout<<endl;
	cout<<"Number of steps = "<<num_steps<<endl;
	cout<<"Total effort    = "<<effort<<endl;



}

//MAIN FUNCTION
int main()
{

	//GETTING INPUTS
	int n,m;
	//Accepting dimensions
	cin>>n;
	cin>>m;
	Graph *G=new Graph(n,m);
	int arr[n];
	//Accepting l[u]`s
	for(int i=0;i<n;i++)
	{
		cin>>arr[i];
		G->set_weight(i,arr[i]);
	}
	int u[m];
	int v[m];

	//Accepting edges
	for(int i=0;i<m;i++)
	{
		cin>>u[i]>>v[i];
		G->add_edge_un(u[i],v[i]);
	}
	//Accepting start and end vertices
	int s,t;
	cin>>s;
	cin>>t;
	cout<<"\nn = "<<n<<endl;
	cout<<"m = "<<m<<endl;
	cout<<"+++ Path lengths: ";
	for(int i=0;i<n;i++)
	{
		cout<<arr[i]<<" ";
	}
	cout<<endl;
	cout<<"+++ Edges:\n    ";
	for(int i=0;i<m;i++)
	{
		printf("(%2d,%2d) ",u[i],v[i]);
	}
	printf("\n");

	//PRINTING INPUT GRAPH
	cout<<"\n+++ Input graph"<<endl;
	G->prngraph();
	int nV=G->get_n();

	//GENERATING AND PRINTING ONE STEP REACHABILITY GRAPH
	Graph *GR=new Graph(nV);
	GR->form_GR(G);
	cout<<"\n+++ Reachability Graph"<<endl;
	GR->prngraph();

	//GENERATING AND PRINTING SIMULTANEOUS REACHABILITY GRAPH
	int nVs=nV*nV;
	Graph *GS=new Graph(nVs);
	GS->form_GS(G,GR);
	cout<<"\n+++ Simultaneous reachability Graph"<<endl;
	GS->prngraph();

	cout<<"\ns = "<<s<<endl;
	cout<<"t = "<<t<<"\n"<<endl;

	//FINDING PATHS IN SIMULTANEOUS REACHABILITY GRAPH
	cout<<"+++ Minimum number of steps"<<endl;
	cout<<"--- Allowing robots to share a node"<<endl;
	GS->BFS_find_path(s,t,0);//last parameter = mode = 0 for node sharing allowed, 1 for sharing not allowed 
	cout<<"--- Disallowing robots to share a node"<<endl;
	GS->BFS_find_path(s,t,1);

	
	






}
