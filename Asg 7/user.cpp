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


int memread_N(int *mem)
{
	return *(mem);
}
int memread_n(int *mem)
{
	int *mem2=mem+1;
	return *(mem2);
}
int memread_node_key(int *mem,int node)
{
	int *mem2=mem+3*node+2;
	return *(mem2);
}
int memread_node_left(int *mem,int node)
{
	int *mem2=mem+3*node+2+1;
	return *(mem2);
}
int memread_node_right(int *mem,int node)
{
	int *mem2=mem+3*node+2+2;
	return *(mem2);
}

void memwrite_N(int *mem,int N)
{
	*(mem)=N;
}
void memwrite_n(int *mem,int n)
{
	int *mem2=mem+1;
	*(mem2)=n;
}
void memwrite_node_key(int *mem,int node,int key)
{
	int *mem2=mem+3*node+2;
	*(mem2)=key;
}
void memwrite_node_left(int *mem,int node,int left)
{
	int *mem2=mem+3*node+2+1;
	*(mem2)=left;
}
int memwrite_node_right(int *mem,int node,int right)
{
	int *mem2=mem+3*node+2+2;
	*(mem2)=right;
}


class Tree
{
public:
	int *mem;// memory segment where tree is stored

	Tree()
	{
		mem=NULL;
		memwrite_N(mem,0);//Value of N
		memwrite_n(mem,0);//Value of n
	}
	Tree(int *shm_mem,int cap)
	{
		mem=shm_mem;
		memwrite_N(mem,cap);//Value of N
		memwrite_n(mem,0);//Value of n
	}
	
	//we use the shared memory as a single dimensional integer array of the form 
	//N,n,Key0,LeftChild0,RightChild0,Key1,LeftChild1,RightChild1,........
	void BSTinsert(int key)
	{

		int n=memread_n(mem);
		int N=memread_N(mem);
		if(n==N)
		{
			return;
		}
		int curr_node=n;//node number where current node must be entered in memory
		memwrite_n(mem,n+1); //updating value of n in shared memory
		memwrite_node_key(mem,curr_node,key); //writing key in new node
		//writing -1 as new node currently has no child
		memwrite_node_left(mem,curr_node,-1);
		memwrite_node_right(mem,curr_node,-1);
		
		if(n!=0)//here n is still the past value
		{
			//if current node is root node, our work is done, else we have to traverse
			//down to its parent and sets it corresponding child index value to curr_node
			int node=0;
			while(true)
			{
				int val=memread_node_key(mem,node);
				int flag=0;
				int newnode=0;
				if(key>val)
				{
					newnode=memread_node_right(mem,node);//next node number is the value of right child of current node
					flag=2;
				}
				else if(key<val)
				{
					newnode=memread_node_left(mem,node);//next node number is the value of left child of current node
					flag=1;
				}
				else
				{// for key==val for current node
					printf("    Insert(%d): Key Exists\n",key);
					break;
				}
				if(newnode==-1)
				{
					if(flag==1)
					{
						printf("    Insert(%d): Left Child of %d created\n",key,val);
						memwrite_node_left(mem,node,curr_node);

					}
					if(flag==2)
					{
						printf("    Insert(%d): Right Child of %d created\n",key,val);
						memwrite_node_right(mem,node,curr_node);
					}
					break;
				}
				else
				{
					node=newnode;
				}
			}
		}
		else
		{
			printf("    Insert(%d): Root created\n",key);
		}
	}

	void BSTsearch(int key)
	{
		int flag=-1;//flag will be 1 for success and 0 for failure
		int n=memread_n(mem);
		if(n==0)
		{
			flag=0;
		}
		else
		{
			int curr_node=0;//node number for current node(at present the root)
			while(true)
			{
				if(curr_node==-1 )//key not found
				{
					flag=0;
					break;
				}
				else if(memread_node_key(mem,curr_node)==key)//key found
				{
					flag=1;
					break;
				}
				else
				{
					int val=memread_node_key(mem,curr_node);
					if(key>val)
					{
						curr_node=memread_node_right(mem,curr_node);//going to the right child
					}
					else
					{
						curr_node=memread_node_left(mem,curr_node);//going to the left child
					}
				}
			}
		}
		if(flag==0)
		{
			printf("    Search(%d): Failed\n",key);
		}
		else if(flag==1)
		{
			printf("    Search(%d): Success\n",key);
		}

	}

	void inorder(int curr_node)
	{
		if(curr_node!=-1)
		{
			inorder(memread_node_left(mem,curr_node));//LEFT CHILD
			printf("%d ",memread_node_key(mem,curr_node));//ROOT
			inorder(memread_node_right(mem,curr_node));//RIGHT CHILD
		}
		
	}
	void BSTinorder()
	{
		printf(" Inorder listing of the keys in the BST: ");
		inorder(0);//calling inorder with node number for root node
		printf("\n");
	}
};





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
	Tree t=*(new Tree(mem,N));
	t.BSTsearch(50);

	t.BSTinsert(50);

	t.BSTsearch(20);

	t.BSTinsert(20);

	t.BSTinsert(60);

	t.BSTinsert(40);

	t.BSTinsert(75);

	t.BSTsearch(60);
	t.BSTsearch(61);
	t.BSTinorder();

}