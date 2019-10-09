//PROGRESS = Defined class for Process

#include <iostream>// for cout(),cin()
#include <climits>// for using INT_MIN
#include<unistd.h>//for using getpid() or getppid(), dup2(),close()
#include<cstdio>// for fopen(), fileno()
//#include<cstdlib>
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

	//Constructor
	Process()
	{
		key=UNDEFINED;
		lpid=UNDEFINED;
		rpid=UNDEFINED;
		ppid=getppid();
	}
	Process(int par_pipe[2])
	{
		//Parent can also pointer of its corresponding pipe to be assigned as parent pipe of child node
		key=UNDEFINED;
		lpid=UNDEFINED;
		rpid=UNDEFINED;
		ppid=getppid();
		ppipe=par_pipe;
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

int main()
{

	//We are in the root process, so lets create process node for it;
	Process *root=new Process();
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



}


