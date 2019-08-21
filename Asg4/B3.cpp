#include <iostream>
#include <cstdio>
#include <cstdlib>
#include<typeinfo>

using namespace std;

//class prototypes 
class Node;
class LinkedList;

//function prototypes


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
	//Constructor
	Graph(int n,int m)
	{
		this->n=n;
		this->m=m;
		this->adj_list=new LinkedList*[this->n];
		for(int i=0;i<this->n;i++)
		{
			this->adj_list[i]=new LinkedList();
		}
		this->lv=new int[n];
	}
	void set_weight(int index,int weight)
	{
		//function to assign the weight of a particular vertex
		this->lv[index]=weight;
	}
	void add_edge(int u,int v)
	{
		//function to add an edge from u-index vertex to v-index vertex
		adj_list[u]->insert(v);
		adj_list[v]->insert(u);
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
				printf("%2d\n",st->get_index());
			}

		}
	}


};

//MAIN FUNCTION
int main()
{

	int n,m;
	cin>>n;
	cin>>m;
	Graph *G=new Graph(n,m);
	int arr[n];
	for(int i=0;i<n;i++)
	{
		cin>>arr[i];
		G->set_weight(i,arr[i]);
	}
	int u[m];
	int v[m];
	for(int i=0;i<m;i++)
	{
		cin>>u[i]>>v[i];
		G->add_edge(u[i],v[i]);
	}
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

	cout<<"\n+++ Input graph"<<endl;
	G->prngraph();


}
