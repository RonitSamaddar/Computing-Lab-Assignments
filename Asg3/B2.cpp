//TODO : FIX LINKED LIST INSERTION PROCESS AND DRY RUN WITH MAIN CODE

#include<iostream>
#include<cstdio>
#include<cstdlib>
using namespace std;

int gcd(int,int);
class Element;
class LinkedList;
class Node;
class Tree;

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
		cout<<"("<<a<<","<<b<<")"<<endl;
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
				cout<<"("<<curr->get_a()<<","<<curr->get_b()<<")-->";
				a=a->get_next();
			}
			curr=a->get_data();
			cout<<"("<<curr->get_a()<<","<<curr->get_b()<<")"<<endl;
		}
	}
};



class Tree
{
	Node *root;
public:
	Tree()
	{
		root=new Node();
		root->set_data(1,1);
	}
	Node *get_root()
	{
		return this->root;
	}

	Node *tsearch(int a,int b)
	{
		cout<<"Searching "<<a<<","<<b<<endl;
		if(a==1 && b==1)
			return this->root;
		if(gcd(a,b)!=1 || a<1 || b<1)
			return NULL;
		else
		{
			int dir=0,parent_a=0,parent_b=0;
			if(b>a)
			{
				cout<<"Node is to right of its parent"<<endl;
				dir=1;
				parent_a=a;
				parent_b=b-a;
			}
			else
			{
				cout<<"Node is to left of its parent"<<endl;
				dir=0;
				parent_a=a-b;
				parent_b=b;
			}
			cout<<"Parent "<<parent_a<<","<<parent_b<<" needs to be searched"<<endl;
			Node *parent=tsearch(parent_a,parent_b);
			cout<<"Parent "<<parent_a<<","<<parent_b<<" searched"<<endl;
			if(parent==NULL)
			{
				cout<<"Parent "<<parent_a<<","<<parent_b<<" does not exist so child "<<a<<","<<b<<" doesn't exists"<<endl;
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

	void tinsert(int a,int b)
	{
		cout<<"Inserting "<<a<<","<<b<<endl;
		if(gcd(a,b)==1)
		{
			cout<<"GCD = 1";
			Node *n=tsearch(a,b);
			cout<<"Finding if node is already there"<<endl;
			if(n==NULL)
			{
				cout<<"No such a,exists"<<endl;
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
				cout<<"Finding Parent with values "<<parent_a<<","<<parent_b<<endl;
				Node *parent=this->tsearch(parent_a,parent_b);
				if(parent==NULL)
				{
					cout<<"Parent needs to be inserted "<<endl;
					tinsert(parent_a,parent_b);
					cout<<"Parent inserted "<<endl;
					parent=this->tsearch(parent_a,parent_b);
				}
				if(dir==1)
				{
					parent->set_right(x);
					cout<<"New node "<<a<<","<<b<<" set to right of parent"<<endl;
				}
				else
				{
					parent->set_left(x);
					cout<<"New node "<<a<<","<<b<<" set to left of parent"<<endl;
				}

			}
		}
	}
	
	void tdelete(int a,int b)
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
			if(n->get_left())
			{
				Node *nd=n->get_left();
				a2=nd->get_a();
				b2=nd->get_b();
				tdelete(a2,b2);
			}
			if(n->get_right())
			{
				Node *nd=n->get_right();
				a2=nd->get_a();
				b2=nd->get_b();
				tdelete(a2,b2);
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
		}
		
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
	cout<<"("<<curr->get_a()<<","<<curr->get_b()<<") ";
	if(curr->get_left())
		printPreorder(curr->get_left());
	if(curr->get_right())
		printPreorder(curr->get_right());
}
void printInorder(Node *curr)
{
	if(curr->get_left())
		printInorder(curr->get_left());
	cout<<"("<<curr->get_a()<<","<<curr->get_b()<<") ";
	if(curr->get_right())
		printInorder(curr->get_right());
}
void printLexorder(Node *curr)
{

}




int main()
{
	Tree *T=new Tree();
	
	int a1=1,b1=1;
	while(a1!=0 || b1!=0)
	{
		cout<<"Enter values to insert or 0,0 to stop : ";
		cin>>a1>>b1;
		if(a1==0 && b1==0)
			break;
		T->tinsert(a1,b1);
		printTree(0,0,T->get_root());

	}
	a1=1;
	b1=1;
	while(a1!=0 || b1!=0)
	{
		cout<<"Enter values to delete or 0,0 to stop : ";
		cin>>a1>>b1;
		if(a1==0 && b1==0)
			break;
		T->tdelete(a1,b1);
		printTree(0,0,T->get_root());
	}
	printPreorder(T->get_root());
	cout<<endl;
	printInorder(T->get_root());
	cout<<endl;


	a1=1;
	b1=1;
	LinkedList *L=new LinkedList(4);
	while(a1!=0 || b1!=0)
	{
		cout<<"Enter values to insert to linkedlist or 0,0 to stop : ";
		cin>>a1>>b1;
		if(a1==0 && b1==0)
			break;
		Node *k=new Node();
		k->set_data(a1,b1);
		L->insert_sorted(k);

	}
	L->print_list();



	
}
