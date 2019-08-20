
#include<iostream>
#include<cstdio>
#include<cstdlib>
using namespace std;



int gcd(int a,int b)
{
	
	if(a>b)
	{
		return gcd(b,a);
	}
	else
	{
		if(b%a==0)
		{
			return a;
		}
		else
		{
			return gcd(b%a,a);
		}
	}
	
}

class Node
{
	int a;
	int b;
	Node *left;
	Node *right;
public:
	Node()
	{
		this->a=-1;
		this->b=-1;
		this->left=NULL;
		this->right=NULL;
	}
	void set_data(int a,int b)
	{
		this->a=a;
		this->b=b;
	}
	void set_left(Node *left)
	{
		this->left=left;
	}
	void set_right(Node *right)
	{
		this->right=right;
	}
	int get_a()
	{
		return this->a;
	}
	int get_b()
	{
		return this->b;
	}

	Node *get_left()
	{
		return this->left;
	}
	Node *get_right()
	{
		return this->right;
	}


	void printNode()
	{
		printf("(%2d,%2d) ",this->a,this->b);
	}

};

class Element
{
private:
	Node *data;
	Element *next;
public:
	Element(Node *k)
	{
		this->data=k;
		this->next=NULL;
	}
	Node *get_data()
	{
		return this->data;
	}
	Element *get_next()
	{
		return this->next;
	}
	void set_data(Node *k)
	{
		this->data=k;
	}
	void set_next(Element *a)
	{
		this->next=a;
	}

};

class LinkedList
{
private:
	Element *start;
	int count;
	int a;
public:
	LinkedList(int a)
	{
		this->start=NULL;
		this->count=0;
		this->a=a;
	}
	int isEmpty()
	{
		return(this->count==0);
	}
	int len()
	{
		return this->count;
	}
	void insert_sorted(Node *k)
	{
		if(k->get_a()==this->a)
		{
			this->count++;
			if(this->start==NULL)
			{
				this->start=new Element(k);
			}
			else
			{
				Element *curr=this->start;
				//cout<<"BREAKPOINT 1"<<endl;
				Node *curr_node=curr->get_data();
				//cout<<"BREAKPOINT 2"<<endl;
				int k_b=k->get_b();
				//cout<<"BREAKPOINT 3"<<endl;
				int curr_b=curr_node->get_b();
				//cout<<"BREAKPOINT 4"<<endl;
				
				//cout<<"BREAKPOINT 5"<<endl;
				Element *e=new Element(k);
				//cout<<"BREAKPOINT 6"<<endl;
				if(k_b<curr_b)
				{
					e->set_next(this->start);
					this->start=e;
				}
				else
				{
					while(curr->get_next()!=NULL && k_b>=((curr->get_next())->get_data())->get_b())
					{
						curr=curr->get_next();
					}
					Element *temp=curr->get_next();
					curr->set_next(e);
					e->set_next(temp);
				}

			}
		}
	}

	Node *del_first()
	{
		
		if(this->isEmpty())
		{
			cout<<"The List Is Empty!!"<<endl;
		}
		else
		{
			
			Node *curr=this->start->get_data();
			Element *a=(this->start)->get_next();
			free(this->start);
			this->start=a;
			this->count--;

			return curr;

		}
	}
	

	void print_list()
	{
		if(this->isEmpty())
		{
			cout<<"The List Is Empty!!"<<endl;
		}
		else
		{
			Element *a=start;
			Node *curr=a->get_data();
			while(a->get_next()!=NULL)
			{
				curr=a->get_data();
				printf("(%2d,%2d)-->",curr->get_a(),curr->get_b());
				a=a->get_next();
			}
			curr=a->get_data();
			printf("(%2d,%2d)\n",curr->get_a(),curr->get_b());
		}
	}
};





class Tree
{
	Node *root;
	int count;
public:
	Tree()
	{
		this->root=new Node();
		(this->root)->set_data(1,1);
		this->count=1;
	}
	int get_count()
	{
		return this->count;
	}
	Node *get_root()
	{
		return this->root;
	}

	Node *tsearch(int a,int b)
	{
		//cout<<"Searching "<<a<<","<<b<<endl;
		if(a==1 && b==1)
			return this->root;
		if(gcd(a,b)!=1 || a<1 || b<1)
			return NULL;
		else
		{
			int dir=0,parent_a=0,parent_b=0;
			if(b>a)
			{
				//cout<<"Node is to right of its parent"<<endl;
				dir=1;
				parent_a=a;
				parent_b=b-a;
			}
			else
			{
				//cout<<"Node is to left of its parent"<<endl;
				dir=0;
				parent_a=a-b;
				parent_b=b;
			}
			//cout<<"Parent "<<parent_a<<","<<parent_b<<" needs to be searched"<<endl;
			Node *parent=tsearch(parent_a,parent_b);
			//cout<<"Parent "<<parent_a<<","<<parent_b<<" searched"<<endl;
			if(parent==NULL)
			{
				//cout<<"Parent "<<parent_a<<","<<parent_b<<" does not exist so child "<<a<<","<<b<<" doesn't exists"<<endl;
				return NULL;
			}
			else
			{
				if(dir==0)
				{
					return parent->get_left();
				}
				else if(dir==1)
				{
					return parent->get_right();
				}
			}
		}

	}

	int tinsert(int a,int b)
	{
		//cout<<"Inserting "<<a<<","<<b<<endl;
		if(gcd(a,b)==1)
		{
			//cout<<"GCD = 1";
			Node *n=tsearch(a,b);
			//cout<<"Finding if node is already there"<<endl;
			if(n==NULL)
			{

				//cout<<"No such a,exists"<<endl;
				Node *x=new Node();
				x->set_data(a,b);
				int dir=0,parent_a=0,parent_b=0;
				if(b>a)
				{
					dir=1;
					parent_a=a;
					parent_b=b-a;
				}
				else
				{
					dir=0;
					parent_a=a-b;
					parent_b=b;
				}
				//cout<<"Finding Parent with values "<<parent_a<<","<<parent_b<<endl;
				Node *parent=this->tsearch(parent_a,parent_b);
				int pcount=0;
				if(parent==NULL)
				{
					//cout<<"Parent needs to be inserted "<<endl;
					pcount=tinsert(parent_a,parent_b);
					//cout<<"Parent inserted "<<endl;
					parent=this->tsearch(parent_a,parent_b);
				}
				if(dir==1)
				{
					parent->set_right(x);
					//cout<<"New node "<<a<<","<<b<<" set to right of parent"<<endl;
				}
				else
				{
					parent->set_left(x);
					//cout<<"New node "<<a<<","<<b<<" set to left of parent"<<endl;
				}
				this->count++;
				return 1+pcount;

			}
		}
		return 0;
	}
	
	int tdelete(int a,int b)
	{
		
		Node *n=tsearch(a,b);
		if(n!=NULL)
		{
			int dir=0,parent_a=0,parent_b=0;
			if(b>a)
			{
				dir=1;
				parent_a=a;
				parent_b=b-a;
			}
			else
			{
				dir=0;
				parent_a=a-b;
				parent_b=b;
			}
			Node *parent=tsearch(parent_a,parent_b);
			int a2,b2;
			int l=0,r=0;
			if(n->get_left())
			{
				Node *nd=n->get_left();
				a2=nd->get_a();
				b2=nd->get_b();
				l=tdelete(a2,b2);
			}
			if(n->get_right())
			{
				Node *nd=n->get_right();
				a2=nd->get_a();
				b2=nd->get_b();
				r=tdelete(a2,b2);
			}
			if(dir==0)
			{
				free(parent->get_left());
				parent->set_left(NULL);

			}
			else if(dir==1)
			{
				free(parent->get_right());
				parent->set_right(NULL);
			}
			this->count--;
			return 1+l+r;
		}

		return 0;
		
	}
	


};

void printTree(int level,int dir,Node *root)
{

	Node *curr=root;
	for(int i=0;i<level;i++)
	{
		cout<<"\t";
	}
	if(dir==-1)
	{
		cout<<"L=";
	}
	else if(dir==1)
	{
		cout<<"R=";
	}
	curr->printNode();
	if(root->get_left())
		printTree(level+1,-1,root->get_left());
	if(root->get_right())
		printTree(level+1,1,root->get_right());
}

void printPreorder(Node *curr)
{
	curr->printNode();
	if(curr->get_left())
		printPreorder(curr->get_left());
	if(curr->get_right())
		printPreorder(curr->get_right());
}
void printInorder(Node *curr)
{
	if(curr->get_left())
		printInorder(curr->get_left());
	curr->printNode();
	if(curr->get_right())
		printInorder(curr->get_right());
}

int max(int x,int y)
{
	if(x>=y)
		return x;
	else 
		return y;
}

int find_MaxA(Node *curr)
{
	if(curr==NULL)
		return 0;
	else if(curr->get_left()==NULL && curr->get_right()==NULL)
		return curr->get_a();
	else
	{
		int M1=curr->get_a();
		if(curr->get_left())
			M1=max(M1,find_MaxA(curr->get_left()));
		if(curr->get_right())
			M1=max(M1,find_MaxA(curr->get_right()));
		return M1;
	}
}
void print_arr_list(LinkedList **L,int A)
{
	for(int i=1;i<=A;i++)
	{
		cout<<"List"<<i<<" ,Count = "<<L[i]->len()<<endl;
	}
}
void lex_rec(LinkedList **L,int index,int A)
{
	int length=L[index]->len();
	int p=0;
	while(L[index]->isEmpty()!=1)
	{
		//cout<<++p<<"-th iteration"<<endl;
		Node *curr=L[index]->del_first();
		curr->printNode();
		int a=curr->get_a();
		int b=curr->get_b();
		Node *left=curr->get_left();
		Node *right=curr->get_right();
		if(left)
			L[a+b]->insert_sorted(left);
		if(right)
			L[index]->insert_sorted(right);
	}
	free(L[index]);
}
void printLexorder(Node *curr)
{
	int A=find_MaxA(curr);
	LinkedList *L[A+1];
	for(int i=1;i<=A;i++)
	{
		L[i]=new LinkedList(i);
	}
	L[1]->insert_sorted(curr);
	//print_arr_list(L,A);
	for(int i=1;i<=A;i++)
	{
		lex_rec(L,i,A);

	}
	cout<<endl;

}




int main()
{
	Tree *T=new Tree();
	int nins,ndel;
	cout<<"nins = ";
	cin>>nins;
	cout<<"ndel = ";
	cin>>nins;


	int a,b;
	cout<<"\n//For insertion and deletion enter a and b with a space in between\n"<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"+++ INSERTION PHASE"<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;

	for(int i=0;i<nins;i++)
	{
		
		cin>>a>>b;
		int count=T->tinsert(a,b);
		printf("(%2d,%2d) : %2d nodes added\n",a,b,count);
		
	}

	cout<<"\n+++ Number of nodes = "<<T->get_count()<<endl;
	cout<<"\n+++ Preorder"<<endl;
	printPreorder(T->get_root());
	cout<<"\n\n+++ Inorder"<<endl;
	printInorder(T->get_root());
	cout<<"\n\n+++ Lexical Order"<<endl;
	printLexorder(T->get_root());

	cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"+++ SEARCH PHASE"<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	int succ_state=0,fail_state=0;
	while(succ_state==0 || fail_state==0)
	{
		a=rand()%99+1;
		b=rand()%99+1;
		Node *n=T->tsearch(a,b);
		if(n==NULL)
		{
			if(fail_state==0)
			{
				cout<<"("<<a<<","<<b<<") : Search failed"<<endl;
				fail_state=1;
			}
		}
		else
		{
			if(succ_state==0)
			{
				cout<<"("<<a<<","<<b<<") : Search successful"<<endl;
				succ_state=1;
			}
		}
		
	}

	cout<<"\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"+++ DELETION PHASE"<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;

	for(int i=0;i<nins;i++)
	{
		int a,b;
		cin>>a>>b;
		int count=T->tdelete(a,b);
		printf("(%2d,%2d) : %2d nodes deleted\n",a,b,count);
	}

	cout<<"\n+++ Number of nodes = "<<T->get_count()<<endl;
	cout<<"\n+++ Preorder"<<endl;
	printPreorder(T->get_root());
	cout<<"\n\n+++ Inorder"<<endl;
	printInorder(T->get_root());
	cout<<"\n\n+++ Lexical Order"<<endl;
	printLexorder(T->get_root());





	
}
