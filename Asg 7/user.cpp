//HEADER INCLUSIONS
#include<iostream>//for cin,cout
#include<sys/types.h>//for ftok(),shmat(),getpid()
#include<sys/ipc.h>//for ftok(),shmget()
#include<sys/shm.h>//for shmget(),shmat()
#include<cstdlib>//for exit()
#include<cstdio>//for perror()
#include<sys/stat.h>//for mknod(),open()
#include<fcntl.h>//for open()
#include<cstring>//for strcmp()
#include<unistd.h>//for read(),write(),getpid()
#include<cmath>//for ceil()

//MACRO DEFINATIONS
#define  SHM_KEY 999//for use in shmget

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
	}
	Tree(int *shm_mem)
	{
		mem=shm_mem;
	}
	
	//we use the shared memory as a single dimensional integer array of the form 
	//N,n,Key0,LeftChild0,RightChild0,Key1,LeftChild1,RightChild1,........
	int BSTinsert(int key)
	{

		int n=memread_n(mem);
		int N=memread_N(mem);
		if(n==N)
		{
			return -1;
		}
		int curr_node=n;//node number where current node must be entered in memory
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
		memwrite_n(mem,n+1); //updating value of n in shared memory
		return 0;
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

int rand_int(int l,int u)
{
	int x=u+1;
	while(x==u+1)
		x=(int)(rand()*1.0*(u+1-l)/RAND_MAX+(float)l);
	//cout<<"x = "<<x<<endl;
	return x;
}





int main()
{
	
	

	int id=getpid();
	srand(id);

	//CHECKING IF REQUEST QUEUE RQ IS AVAILABLE
	mknod("RQ",S_IFIFO|0666,0);//creation of RQ(request) fifo pipe
	int rq_fd=open("RQ",O_WRONLY|O_NDELAY);//opening the file descriptor corresponding to the FIFO request pipe
	if(rq_fd==-1)
	{
		cout<<"*** The controller is not ready yet. Exiting..."<<endl;
		exit(1);
	}
	cout<<"PIPE RQ OPENED FOR WRITING"<<endl;

	//Sending registration request
	char *req=new char[7];
	sprintf(req,"R %d",id);
	write(rq_fd,req,sizeof(req));
	cout<<"User Registed"<<endl;

	//Opening the shared memory
	int size=0;
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
	int N=*(mem);
	int n=*(mem+1);
	Tree *T=new Tree(mem);


	//CREATION OF GQ(U) and DQ(U) queues
	char GQ[7],DQ[7];
	sprintf(GQ,"GQ%d",id);
	sprintf(DQ,"DQ%d",id);
	cout<<"Creating GQ and DQ pipes"<<endl;
	mknod(GQ,S_IFIFO|0666,0);//creation of GQ(U) pipe
	int gq_fd=open(GQ,O_RDONLY);//opening the file descriptor corresponding to the FIFO GQ
	cout<<"PIPE GQ OPENED "<<gq_fd<<endl;
	mknod(DQ,S_IFIFO|0666,0);//creation of DQ(U) pipe
	int dq_fd=open(DQ,O_WRONLY);//opening the file descriptor corresponding to the FIFO DQ
	cout<<"PIPE DQ OPENED"<<dq_fd<<endl;

	//LOOP FOR SENDING REQUESTS
	char s[21];
	int X;
	while(true)
	{
		cout<<"Generating request... "<<endl;
		//GENERATING RANDOM REQUESTS AND KEYS
		int req_no=rand_int(1,3);
		int key=rand_int(1,1000);
		if(req_no==1)//Search
		{
			sprintf(s,"S %d %d",id,key);
		}
		else if(req_no==2)
		{
			sprintf(s,"P %d",id);
		}
		else if(req_no==3)
		{
			sprintf(s,"I %d %d",id,key);
		}
		cout<<"To be Written s = "<<s<<endl;
		write(rq_fd,s,strlen(s));//Writing requests to RQ
		cout<<"s written"<<endl;
		

		//READING FOR GRANT TOKEN
		read(gq_fd,&X,sizeof(X));
		cout<<"Granted permission with token = "<<X<<endl;
		int iflag=0;
		if(s[0]=='S')
		{
			cout<<"BST search"<<endl;
			T->BSTsearch(key);
			cout<<"Search DONE!!"<<endl;
		}
		else if(s[0]=='P')
		{
			cout<<"BST inorder"<<endl;
			T->BSTinorder();
			cout<<"Inorder DONE!!"<<endl;
		}
		else if(s[0]=='I')
		{
			cout<<"BST insert"<<endl;
			iflag=T->BSTinsert(key);
			cout<<"Insertion done"<<endl;
			
		}
		//Sending Back Done(Completed token)
		write(dq_fd,&X,sizeof(X));
		cout<<"Sent back Done Token"<<endl;

		if(iflag==-1)
		{
				cout<<"End of loop"<<endl;
				break;
		}

		getchar();
		//cout<<"CMP = "<<strcmp(s,"EXIT")<<"\n"<<endl;
	}

	//LAST PRINT REQUEST
	sprintf(s,"P %d",id);
	cout<<"Last Print Request"<<endl;
	write(rq_fd,s,strlen(s));//Writing request to RQ
	cout<<"Print Request Sent"<<endl;
	read(gq_fd,&X,sizeof(X));//Reading for Grant token
	cout<<"Granted permission with token = "<<X<<endl;
	cout<<"BST inorder"<<endl;
	T->BSTinorder();//BST inorder
	cout<<"Inorder DONE!!"<<endl;
	write(dq_fd,&X,sizeof(X));//Sending Back Done(Completed token)
	cout<<"Sent back Done Token"<<endl;




	//Quit Request Code here
	sprintf(s,"Q %d",id);
	cout<<"Quit Request"<<endl;
	write(rq_fd,s,strlen(s));//Writing request to RQ
	cout<<"Quit Request Sent"<<endl;


	//Clsoing and removing all queues
	close(dq_fd);
	remove(DQ);
	close(gq_fd);
	remove(GQ);
	close(rq_fd);
	remove("RQ");

	//Detaching from SHM
	shmdt(mem);
	shmctl(shmid,IPC_RMID,NULL);



}