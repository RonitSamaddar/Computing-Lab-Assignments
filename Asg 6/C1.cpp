//PROGRESS = Defined class for Process

#include <iostream>// for cout(),cin()
#include <climits>// for using INT_MIN
#include<unistd.h>//for using getpid() or getppid(), dup2(),close(),pipe(),fork()
#include<cstdio>// for fopen(), fileno()
//#include<sys/types.h>
#include<sys/wait.h>//for wait()
#include<cstdlib>//for exit()
//#include<cstring>
//#include<cmath>


#define UNDEFINED INT_MIN
#define INPUT_FILE "file.txt"

using namespace std;


class Process
{
	//class to represent a node for a process
public:
	int key;//key of the node
	pid_t lpid;//process id for left child
	pid_t rpid;//process id for right child
	pid_t ppid;//process id for parent
	int lpipe[2];//pipe for left child
	int rpipe[2];//pipe for right child
	int *ppipe;//pipe for parent
	int nkeys;//number of keys this node reads
	int discard;//number of duplicate keys discarded

	//Constructor
	Process()
	{
		//FOR ROOT
		key=UNDEFINED;
		lpid=UNDEFINED;
		rpid=UNDEFINED;
		ppid=-1;
		ppipe=NULL;
		discard=0;
	}
	Process(int par_id,int par_pipe[2])
	{
		//FOR NON_ROOT NODES
		//Parent can also pass the pointer of its corresponding left/right pipe to be assigned as parent pipe of child node
		key=UNDEFINED;
		lpid=UNDEFINED;
		rpid=UNDEFINED;
		ppid=par_id;
		ppipe=par_pipe;
		discard=0;
	}
};

void init_root()
{
	//Function for Part 1
	FILE *root_file=fopen(INPUT_FILE,"r");//opening file pointer for input file
	int root_fd=fileno(root_file);//getting file descriptor for input file pointer
	dup2(root_fd,0);//redirecting input from stdin to above input file descriptor
	close(root_fd);//closing file descriptor for input file
}

Process *buildproctree(Process *root)
{
	Process *curr=root;
	while(true)
	{
		//loop which each process goes through and ends if it receives -1 and sends -1
		//to its child process nodes if any

		//READING KEY
		int k;
		//cout<<"CURRENT NODE : PID = "<<getpid()<<" ,getppid = "<<getppid()<<"ppid = "<<curr->ppid<<endl;
		if(curr->ppid==-1)
		{

			//root takes input from redirected standard input that is input file
			//as done in Part 1
			cin>>k; 
			//cout<<"ROOT WITH PID "<<getpid()<<" reads "<<k<<endl;

		}
		else
		{
			//Other process takes input from read end([0])of parent pipe
			read(curr->ppipe[0],&k,sizeof(k));
			//cout<<"NODE WITH PID "<<getpid()<<" reads "<<k<<endl;
		}
		curr->nkeys++;


		//KEY HANDLING
		if(k!=-1)
		{//Normal key
			if(curr->key==UNDEFINED)
			{
				//If node can accomodate key, it does
				//cout<<"NODE WITH PID "<<getpid()<<" accepts key "<<k<<endl;
				curr->key=k;
			}
			else
			{
				//if node already has a key, there are 3 possible cases
				//Based on the value of the new key, it either discards it or writes it 
				//to corresponding child pipe
				if(k==curr->key)
				{
					//if new key matches, node key, we just discard it
					//cout<<"NODE WITH PID "<<getpid()<<" discards key "<<k<<endl;
					curr->discard++;
				}
				else if(k>curr->key)
				{
					//if new key is more than node's keys, we see if there is a right child
					//if right child is not there we create one and fork
					//and then the parent process can write the new key in the right pipe
					if(curr->rpid==UNDEFINED)
					{
						//no right child
						pipe(curr->rpipe);//creating pipe for right child
						Process *right= new Process(getpid(),curr->rpipe);						
						int rx=fork();
						if(rx!=0)
						{
							//parent process corresponding to parent node
							curr->rpid=rx;
							//cout<<"NODE WITH PID "<<getpid()<<" writes key "<<k<<" to right child pid = "<<curr->rpid<<endl; 
							write(curr->rpipe[1],&k,sizeof(k));
						}
						else
						{
							//child process corresponding to right child
							curr=right;
						}
							
						
					}
					else
					{
						//cout<<"NODE WITH PID "<<getpid()<<" writes key "<<k<<" to right child pid = "<<curr->rpid<<endl; 
						write(curr->rpipe[1],&k,sizeof(k));
					}

				}
				else if(k<curr->key)
				{
					//if new key is less than node's keys, we see if there is a left child
					//if left child is not there we create one and fork
					//and then the parent process can write the new key in the left pipe
					if(curr->lpid==UNDEFINED)
					{
						//no left child
						pipe(curr->lpipe);//creating pipe for left child
						Process *left= new Process(getpid(),curr->lpipe);						
						int lx=fork();
						if(lx!=0)
						{
							//parent process corresponding to parent node
							curr->lpid=lx;
							//cout<<"NODE WITH PID "<<getpid()<<" writes key "<<k<<" to left child pid = "<<curr->lpid<<endl; 
							write(curr->lpipe[1],&k,sizeof(k));
						}
						else
						{
							//child process corresponding to left child
							curr=left;
						}
							
						
					}
					else
					{
						//cout<<"NODE WITH PID "<<getpid()<<" writes key "<<k<<" to left child pid = "<<curr->lpid<<endl; 
						write(curr->lpipe[1],&k,sizeof(k));
					}
				}
			}
		}//End of normal key condition
		else
		{	//Termination key
			if(curr->key==UNDEFINED)
			{
				//If node can accomodate  some key, we can directly terminate
				//cout<<"NODE WITH PID "<<getpid()<<"received -1 and terminated"<<endl;
				return curr;
			}
			else
			{
				//if node already has a key, we will check both childs, if there, we write -1
				//to their pipes and then terminate

				
				//PRINTING FOR OUTPUT
				printf("    Process   %4d with parent  %4d gets key = %4d  [ New,  assigned ]\n",getpid(),curr->ppid,curr->key);
				for(int i=0;i<curr->discard;i++)
				{
				printf("    Process   %4d with parent  %4d gets key = %4d  [ Old, discarded ]\n",getpid(),curr->ppid,curr->key);
				}
				//END OF OUTPUT PRINTING
				

				if(curr->lpid!=UNDEFINED)
				{
					//cout<<"NODE WITH PID "<<getpid()<<" writes key "<<k<<" to left child pid = "<<curr->lpid<<endl; 
					write(curr->lpipe[1],&k,sizeof(k));
				}
				if(curr->rpid!=UNDEFINED)
				{
					//cout<<"NODE WITH PID "<<getpid()<<" writes key "<<k<<" to right child pid = "<<curr->rpid<<endl; 
					write(curr->rpipe[1],&k,sizeof(k));
				}
				//cout<<"NODE WITH PID "<<getpid()<<"received -1 and terminated"<<endl;
				return curr;
			}
		}//End of termination key condition
	}//End of while
	

}//End of buildproctree

void synchronize(Process *curr,int p[2],int q[2])
{
	if(curr->ppid==-1)
	{
		//Root process
		int n=curr->nkeys-1;//total number of keys = keys root reads - 1(for key = -1)
		int nb=n-curr->discard-1;//calculating number of bytes to write/read
		for(int i=0;i<nb;i++)
		{
			char c;
			read(p[0],&c,sizeof(c));//reading nb characters from pipe p
		}
		for(int i=0;i<nb;i++)
		{
			char c='a';
			write(q[1],&c,sizeof(c));//writing nb characters to pipe q
		}


	}
	else
	{
		//Non-Root process
		int nb=curr->discard+1;//calculating number of bytes to write/read
		for(int i=0;i<nb;i++)
		{
			char c='a';
			write(p[1],&c,sizeof(c));//writing nb characters to pipe p
		}
		for(int i=0;i<nb;i++)
		{
			char c;
			read(q[0],&c,sizeof(c));//reading nb characters from pipe q
		}
	}
}
int maxx(int x,int y)
{
	if(x>=y)
	{
		return x;
	}
	else
	{
		return y;
	}
}

int calcheight(Process *curr)
{
	int hl=-1,hr=-1;
	if(curr->lpid!=UNDEFINED)
	{
		//has a left child
		read(curr->lpipe[0],&hl,sizeof(hl));
	}
	if(curr->rpid!=UNDEFINED)
	{
		//has a right child
		read(curr->rpipe[0],&hr,sizeof(hr));
	}
	int h=1+maxx(hl,hr);
	if(curr->ppid!=-1)
	{
		write(curr->ppipe[1],&h,sizeof(h));
	}
	return h;
}
void inorder(Process *curr,int flag2[2])
{
	//every process shall send 0 to its childs pipe if child is not allowed to execute
	// and 1 if child is allowed to execute
	// so left child will directly be sent 1 and allowed to execute
	// right child will be sent 1 only after parent prints its own key
	// so for any process/node,
					// 1) We wait for confirmation from parent that it can run(unless root)
					// 2) We send 1 to left child/process and waitpid() for it to end
					// 3) After left child finishes execution, we print parent process's key
					// 4) We send 1 to right child/process and terminate

	if(curr->ppid==-1)
	{
		//Root Process
		int a=1;
		write(curr->lpipe[1],&a,sizeof(a));//allowing left child to run
		waitpid(curr->lpid,NULL,0);// waiting for left child process to finish
		
		//Synchronization for printing
		int c=0;
		read(flag2[0],&c,sizeof(c));
		c++;
		write(flag2[1],&c,sizeof(c));
		if(c==1)
			cout<<"\n+++ Inorder listing of the keys :  ";

		cout<<curr->key<<" ";//printing own key
		cout.flush();
		write(curr->rpipe[1],&a,sizeof(a));//allowing right child to run
		waitpid(curr->rpid,NULL,0);// waiting for right child process to finish
		delete curr;
		exit(0);
	}
	else
	{
		int k,a=1;
		while(true)
		{
			read(curr->ppipe[0],&k,sizeof(k));
			if(k==1)
			{
				break;
			}
		}//if k==1 that is parent allows this child to run
		write(curr->lpipe[1],&a,sizeof(a));//allowing left child to run
		waitpid(curr->lpid,NULL,0);// waiting for left child process to finish
		
		//Synchronization for printing
		int c=0;
		read(flag2[0],&c,sizeof(c));
		c++;
		write(flag2[1],&c,sizeof(c));
		if(c==1)
			cout<<"\n+++ Inorder listing of the keys :  ";

		cout<<curr->key<<" ";//printing own key
		cout.flush();//Important as no endl in previous line so output no getting flushed
		write(curr->rpipe[1],&a,sizeof(a));//allowing right child to run
		waitpid(curr->rpid,NULL,0);// waiting for right child process to finish
		delete curr;
		exit(0);
		
	}
}



int main()
{

	//We are in the root process, so lets create process node for it;
	Process *root=new Process();
	int root_pid=getpid();
	init_root();//Function for Part 1

	/* TEST CODE FOR TESTING WORKING OF PART 1
	int x;
	cin>>x;
	while(x!=-1)
	{
		cout<<x<<" ";
		cin>>x;
	}
	cout<<endl;
	*/

	//Declaration of pipes for Synchronization purposes
	int p[2],q[2];
	pipe(p);
	pipe(q);
	int p1[2],q1[2];
	pipe(p1);
	pipe(q1);

	//Declaration of flag pipes for marking end of buildproctree and start of calcheight,
	// end of calcheight and start of inorder for printing newline and output text at the junctions 
	int flag1[2],flag2[2];
	pipe(flag1);
	pipe(flag2);
	int a=0;
	write(flag1[1],&a,sizeof(a));
	write(flag2[1],&a,sizeof(a));

	cout<<"+++ Building process tree"<<endl;
	Process *curr=buildproctree(root);
	synchronize(curr,p,q);
	int h=calcheight(curr);
	int b;
	read(flag1[0],&b,sizeof(b));
	b++;
	write(flag1[1],&b,sizeof(b));
	if(b==1)
		cout<<endl;
	printf("    Height of the process  %4d with key %4d is %d",getpid(),curr->key,h);
	if(curr->ppid==-1)
	{
		cout<<"  [Height of entire tree]"<<endl;
	}
	else
	{
		cout<<endl;
	}
	synchronize(curr,p1,q1);
	inorder(curr,flag2);


	//wait(NULL);
	//exit(0);
	



}


