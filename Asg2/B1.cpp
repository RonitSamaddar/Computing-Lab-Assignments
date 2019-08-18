#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<cmath>
#include<climits>
#include<cstring>
using namespace std;

class searchpos;
class node;


int read(char *,int);
void write(char *,int,int);
void hinit(char *);
void printFile(char *);
int hsearch(char *,int);
void hinsert(char *,int);
int hfindmin(char *);
void fswap(char *,int,int);
void heapify(char *,int);
void hdelmin(char *);
int hfindmax(char *);
node *dbinit(char *);
searchpos *dbsearch(node *,int);
void dbinsert(node *,int,int);

class searchpos
{
	char *filename;
	int pos;
public:
	searchpos(char *file,int loc)
	{
		int l=strlen(file);
		this->filename=new char[(l+1)];
		strcpy(this->filename,file);
		this->pos=loc;
	}
	char *get_name()
	{
		return this->filename;
	}
	int get_loc()
	{
		return this->pos;
	}
	void print_pos()
	{
		cout<<"The element is located in file "<<this->filename<<" in position "<<this->pos<<endl;

	}
};
class node
{
	char *filename;
	int min;
	int max;
	int level;
	node *left;
	node *right;
public:
	node()
	{
		this->filename=NULL;
		this->min=INT_MAX;
		this->max=INT_MIN;
		this->left=NULL;
		this->right=NULL;
	}

	char *get_name()
	{
		return this->filename;
	}
	int get_min()
	{
		return this->min;
	}
	int get_max()
	{
		return this->max;
	}
	int get_level()
	{
		return this->level;
	}
	node *get_left()
	{
		return this->left;
	}
	node *get_right()
	{
		return this->right;
	}


	void set_name(char *file)
	{
		if(file==NULL)
		{
			this->filename=NULL;
		}
		else
		{
			int len=strlen(file);
			this->filename=new char[(len+1)];
			strcpy(this->filename,file);
		}	
	}
	void set_min(int min)
	{
		this->min=min;
	}
	void set_max(int max)
	{
		this->max=max;
	}
	void set_level(int level)
	{
		this->level=level;
	}
	void set_left(node *left)
	{
		this->left=left;
	}
	void set_right(node *right)
	{
		this->right=right;
	}

	int isLeaf()
	{
		if(this->left==NULL&&this->right==NULL)
			return 1;
		else 
			return 0;
	}

};





int read(char *file,int pos)
{

	FILE *fptr=fopen(file,"r");
	if(fptr==NULL)
	{
		cout<<"NO SUCH FILE!!!!!"<<endl;
		fclose(fptr);
		return -1;
	}
	else
	{
		int x;
		fseek(fptr,pos*8,SEEK_SET);
		fscanf(fptr,"%7d",&x);
		fclose(fptr);
		return x;
	}


}

void write(char *file,int pos,int val)
{
	FILE *fptr=fopen(file,"r+");
	if(fptr==NULL)
	{
		cout<<"NO SUCH FILE!!!!!"<<endl;
		fclose(fptr);
	}
	else
	{
		fseek(fptr,pos*8,SEEK_SET);
		fprintf(fptr,"%7d",val);
		if(pos!=0)
			fprintf(fptr," ");
		else
			fprintf(fptr,"\n");
		fclose(fptr);
	}
}
void hinit(char *file)
{
	FILE *fptr=fopen(file,"w");
	fprintf(fptr,"%7d\n",0);
	fclose(fptr);
}
void printFile(char *file)
{
	FILE *fptr=fopen(file,"r+");
	if(fptr==NULL)
	{
		cout<<"NO SUCH FILE!!!!!"<<endl;
		fclose(fptr);
	}
	else
	{
		int size=read(file,0);
		cout<<size<<endl;
		for(int i=0;i<size;i++)
		{
			int x=read(file,i+1);
			cout<<x<<" ";
		}
		cout<<endl;
	}
}


int hsearch(char *filename,int val)
{
	int size=read(filename,0);
	for(int i=1;i<=size;i++)
	{
		int x=read(filename,i);
		if(x==val)
		{
			return i;
		}
	}
	return -1;
}

void hinsert(char *filename,int val)
{
	int size=read(filename,0);
	size++;
	write(filename,0,size);

	write(filename,size,val);
	int pos=size;


	while(pos>1)// Position of root = 1
	{
		int par=floor(pos/2);
		int pos_val=read(filename,pos);
		int par_val=read(filename,par);
		if(par_val>pos_val)
		{
			write(filename,pos,par_val);
			write(filename,par,pos_val);
		}
		pos=par;	
	}
}

int hfindmin(char *file)
{
	int size=read(file,0);
	if(size==0)
		return -1;
	else
		return read(file,1);
}

void fswap(char *file,int pos1,int pos2)
{
	int val1=read(file,pos1);
	int val2=read(file,pos2);
	write(file,pos1,val2);
	write(file,pos2,val1);

}

void heapify(char *file,int pos)
{
	int value=read(file,pos);
	int size=read(file,0);

	int lchild=pos*2;
	int rchild=pos*2+1;
	if(rchild>size)
	{
		return;
	}
	int lvalue=read(file,lchild);
	int rvalue=read(file,rchild);

	int minn=value,minpos=pos;
	if(lvalue<value)
	{
		minn=lvalue;
		minpos=lchild;
	}
	if(rvalue<minn)
	{
		minn=rvalue;
		minpos=rchild;
	}
	if(minpos!=pos)
	{
		fswap(file,pos,minpos);
		heapify(file,minpos);
	}
}

void hdelmin(char *file)
{
	int size=read(file,0);
	int value=read(file,size);
	write(file,1,value);
	write(file,size,-1);

	size--;
	write(file,0,size);
	heapify(file,1);
}

int hfindmax(char *file)
{
	int size=read(file,0);
	int start=floor(size/2)+1;
	int max=INT_MIN;
	for(int i=start;i<=size;i++)
	{
		int value=read(file,i);
		if(value>max)
		{
			max=value;
		}
	}
	return max;
}

node *dbinit(char *filename)
{
	node *root=new node();
	root->set_name(filename);
	root->set_level(0);
	hinit(filename);
	return root;

}

searchpos *dbsearch(node *root,int val)
{

	node *curr=root;
	while(curr->isLeaf()!=1)
	{
		if(val<curr->get_min()||val>curr->get_max())
			return NULL;
		node *left=curr->get_left();
		node *right=curr->get_right();
		
		int lmin=left->get_min();
		int lmax=left->get_max();
		if(val>=lmin&&val<=lmax)
		{
			curr=left;
			continue;
		}

		
		int rmin=right->get_min();
		int rmax=right->get_max();
		if(val>=rmin&&val<=rmax)
		{
			curr=right;
			continue;
		}
		return NULL;
		
	}
	char *file=curr->get_name();
	int pos=hsearch(file,val);
	if(pos==-1)
		return NULL;
	searchpos *S=new searchpos(file,pos);
	return S;
	
}

void dbinsert(node *root,int L,int val)
{
	node *curr=root;
	while(curr->isLeaf()!=1)
	{
		if(val<curr->get_min())
			curr->set_min(val);
		else if(val >curr->get_max())
			curr->set_max(val);
		node *left=curr->get_left();
		node *right=curr->get_right();
		if(val<=left->get_max())
		{
			//cout<<"Val = "<<val<<" left min = "<<left->get_min()<<" left max = "<<left->get_max()<<endl;
			curr=left;
		}
		else
		{
			//cout<<"Val = "<<val<<" right min = "<<right->get_min()<<" right max = "<<right->get_max()<<endl;
			curr=right;
		}
	}

	//cout<<"INSERTION REACHES LEAF"<<endl;
	
	char *file=curr->get_name();
	int size=read(file,0);
	int pos=hsearch(file,val);
	if(pos==-1)
	{
		if(size<L)
		{
			hinsert(file,val);
			curr->set_min(hfindmin(file));
			curr->set_max(hfindmax(file));
		}
		else
		{
			int l=strlen(file);
			char *file2=new char[l+2];
			char *file1=new char[l+2];




			strncpy(file2,file,l-4);
			strncpy(file1,file,l-4);
			file2[l-4]='2';
			file1[l-4]='1';

			strcpy(file2+(l-3),file+(l-4));
			strcpy(file1+(l-3),file+(l-4));
			file1[l+1]='\0';
			file2[l+1]='\0';
			hinit(file2);
			rename(file,file1);
			//cout<<"BREAKPOINT 1"<<endl;

			for(int i=0;i<L/2;i++)
			{
				int x=hfindmin(file1);
				hinsert(file2,x);
				hdelmin(file1);
			}
			//cout<<"BREAKPOINT 2"<<endl;
			int maxx=hfindmax(file2);
			if(val<maxx)
			{
				hinsert(file2,val);
			}
			else
			{
				hinsert(file1,val);
			}

			//cout<<"BREAKPOINT 3"<<endl;

			int lvl=curr->get_level();
			node *ll=new node();
			ll->set_name(file2);
			ll->set_min(hfindmin(file2));
			ll->set_max(hfindmax(file2));
			ll->set_level(lvl+1);

			node *rr=new node();
			rr->set_name(file1);
			rr->set_min(hfindmin(file1));
			rr->set_max(hfindmax(file1));
			rr->set_level(lvl+1);

			//cout<<"BREAKPOINT 4"<<endl;

			curr->set_name(NULL);
			curr->set_left(ll);
			curr->set_right(rr);
			curr->set_min((curr->get_left())->get_min());
			curr->set_max((curr->get_right())->get_max());
			
			

			

		}

	}


}

void print_min_max1(node *root)
{
	node *curr=root;
	if(curr->isLeaf()!=1)
	{
		print_min_max1(curr->get_left());
		print_min_max1(curr->get_right());
	}
	else
	{
		cout<<curr->get_min()<<" "<<curr->get_max()<<" ";
	}
	if(curr->get_level()==0)
	{
		cout<<endl;
	}
}

int get_file_min(char *file)
{
	int minn=INT_MAX;
	int size=read(file,0);
	for(int i=1;i<=size;i++)
	{
		int x=read(file,i);
		if(x<minn)
			minn=x;
	}
	return minn;
}
int get_file_max(char *file)
{
	int maxx=INT_MIN;
	int size=read(file,0);
	for(int i=1;i<=size;i++)
	{
		int x=read(file,i);
		if(x>maxx)
			maxx=x;
	}
	return maxx;
}
void print_min_max2(node *root)
{
	node *curr=root;
	if(curr->isLeaf()!=1)
	{
		print_min_max2(curr->get_left());
		print_min_max2(curr->get_right());
	}
	else
	{
		cout<<get_file_min(curr->get_name())<<" "<<get_file_max(curr->get_name())<<" ";
	}
	if(curr->get_level()==0)
		cout<<endl;
}

void print_sorted_mins(node *root)
{
	node *curr=root;
	if(curr->isLeaf()!=1)
	{
		cout<<curr->get_min()<<" ";
		print_sorted_mins(curr->get_left());
		print_sorted_mins(curr->get_right());
	}
	else
	{
		cout<<curr->get_min()<<" ";
	}
	if(curr->get_level()==0)
		cout<<endl;
}
void print_sorted_maxs(node *root)
{
	node *curr=root;
	if(curr->isLeaf()!=1)
	{
		print_sorted_maxs(curr->get_left());
		print_sorted_maxs(curr->get_right());
		cout<<curr->get_max()<<" ";
	}
	else
	{
		cout<<curr->get_max()<<" ";
	}
	if(curr->get_level()==0)
		cout<<endl;
}
void print_tree(node *root)
{
	node *curr=root;
	int level=curr->get_level();
	if(level==0 || level==1)
	{
		cout<<"    ";
	}
	else
	{
		for(int i=1;i<=level;i++)
		{
			cout<<"    ";
		}
	}
	if(level>=1)
		cout<<"+---";
	cout<<"Range = ["<<curr->get_min()<<","<<curr->get_max()<<"], "; 
	
	if(curr->isLeaf()!=1)
	{
		cout<<"File :  None"<<endl;
		print_tree(curr->get_left());
		print_tree(curr->get_right());
	}
	else
	{
		cout<<"File : "<<curr->get_name()<<endl;
	}
	
}

int node_count(node *root)
{
	if(root==NULL)
	{
		return 0;
	}
	else
	{
		return 1+node_count(root->get_left())+node_count(root->get_right());
	}
}

int leaf_count(node *root)
{
	if(root==NULL)
	{
		return 0;
	}
	if(root->get_left()==NULL && root->get_right()==NULL)
	{
		return 1;
	}
	else
	{
		return leaf_count(root->get_left())+leaf_count(root->get_right());
	}
}

int height(node* root)
{
	if(root==NULL)
	{
		return 0;
	}
	else
	{
		int lheight=height(root->get_left());
		int rheight=height(root->get_right());
		return 1+max(lheight,rheight);
	}
}








int main()
{
	int L=32;

	int nin;

	cout<<"nins = ";
	cin>>nin;
	cout<<"Insert keys :"<<endl;
	node *root=new node();
	root=dbinit("File.txt");
	for(int i=0;i<nin;i++)
	{
		int x;
		cin>>x;
		getchar();
		dbinsert(root,L,x);
		

	}
	cout<<"+++ Inorder listing of min and max values of leaves"<<endl;
	print_min_max1(root);
	cout<<"+++ Inorder listing of min and max values read from files"<<endl;
	print_min_max2(root);

	cout<<"+++ Sorted listing of min values at all nodes"<<endl;
	print_sorted_mins(root);
	cout<<"+++ Sorted listing of max values at all nodes"<<endl;
	print_sorted_maxs(root);

	cout<<"+++ Statistics of the BST"<<endl;
	cout<<"    Number of nodes = "<<node_count(root)<<endl;
	cout<<"    Number of leaves = "<<leaf_count(root)<<endl;
	cout<<"    Height = "<<height(root)<<endl;

	cout<<"+++ The BST"<<endl;
	print_tree(root);

	int x,y;
	cout<<"Enter values to be searched :"<<endl;
	cin>>x>>y;
	searchpos *px,*py;
	px=dbsearch(root,x);
	py=dbsearch(root,y);

	cout<<"+++ Search results"<<endl;
	printf("    search(%7d) :",x);
	if(px!=NULL)
		cout<<" PRESENT"<<endl;
	else
		cout<<" ABSENT"<<endl;
	printf("    search(%7d) :",y);
	if(py!=NULL)
		cout<<" PRESENT"<<endl;
	else
		cout<<" ABSENT"<<endl;



	
	



}