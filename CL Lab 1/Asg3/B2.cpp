
#include<iostream>
#include<cstdio>
#include<cstdlib>
using namespace std;



int gcd(int a,int b)
{
	//Function to calculate gcd of two numbers assuming a<b
	if(a>b)
	{	//So if a>b, we swap the two
		return gcd(b,a);
	}
	else
	{	//recursive approach of the repeated division process for gcd calculation process
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
	//class for representing one Node of the Tree

	int a;
	int b;
	Node *left;
	Node *right;
public:
	Node()
	{	//Constructor to initialize data items to default values
		this->a=-1;
		this->b=-1;
		this->left=NULL;
		this->right=NULL;
	}
	//Various functions for setting values, getting values and printing Node data
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
	//class for representing one node of the linkedlist 
	Node *data;//Here data of each element of linkedlist is one node of the Tree
	Element *next;
public:
	Element(Node *k)
	{	//Initializing the element with its corresponding Node
		this->data=k;
		this->next=NULL;
	}
	//Functions for getting and setting the data items
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
	//class for representing a Linkedlist H[a] characterized by 
	//start element, count of elements in list, and the a value for which list
	//is defined

	Element *start;
	int count;
	int a;
public:
	LinkedList(int a)
	{	//Initializing the data items to default values. Setting value of a for the list.
		this->start=NULL;
		this->count=0;
		this->a=a;
	}
	int isEmpty()
	{	//Checking if list is Empty
		return(this->count==0);
	}
	int len()
	{	//Getting length of list, that is count of elements
		return this->count;
	}
	void insert_sorted(Node *k)
	{
		//We insert each Node in its right place in a list sorted on the node's b values
		if(k->get_a()==this->a)
		{	//we check that a value on input Node matches with a value of the list
			this->count++;
			if(this->start==NULL)
			{
				//if list is empty, we directly set the input node as the start node	
				this->start=new Element(k);
			}
			else
			{
				//Else we run a loop to the point, where input b is more than current element's b
				//but less than b value of its successor element. We then insert b value in between the
				//two mentioned nodes

				Element *curr=this->start;
				Node *curr_node=curr->get_data();
				int k_b=k->get_b();
				int curr_b=curr_node->get_b();
				Element *e=new Element(k);
				
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
		//We will always delete the first element of the list and
		//get the corresponding node.
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
		//print list function for debugging purposes
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
	//class for representing a Tree characterized by its root node and count of nodes 
	Node *root;
	int count;
public:
	Tree()
	{
		//initializing the Tree by assigning a root node of (1,1)
		this->root=new Node();
		(this->root)->set_data(1,1);
		this->count=1;
	}
	int get_count()
	{
		//function for getting number of nodes in tree
		return this->count;
	}
	Node *get_root()
	{
		//function for getting the root node of the tree
		return this->root;
	}

	Node *tsearch(int a,int b)
	{
		//function for searching a (a,b) node in the tree

		if(a==1 && b==1)
			return this->root;// Since (1,1) = root node
		if(gcd(a,b)!=1 || a<1 || b<1)
			return NULL;	//since tree can only have nodes (a,b) whose gcd =1 and with a,b >=1
		else
		{
			//Here first we find possible parent node of our node (if present in tree)
			//and also if our node is the left or right child
			int dir=0,parent_a=0,parent_b=0;
			if(b>a)
			{
				//RIGHT CHILD
				dir=1;
				parent_a=a;
				parent_b=b-a;
			}
			else
			{
				//LEFT CHILD
				dir=0;
				parent_a=a-b;
				parent_b=b;
			}

			//Then we search for the parent node recursively
			Node *parent=tsearch(parent_a,parent_b);
			
			if(parent==NULL)
			{
				//Parent not present in tree. So no such node is possible.
				return NULL;
			}
			else
			{
				//if parent is present, we return corresponding child node, where our actual
				//node must have been, as determined earlier
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
		//function for inserting node(a,b) in tree
		
		if(gcd(a,b)==1)
		{
			//we can only insert (a,b) whose gcd = 1
			
			Node *n=tsearch(a,b);
			//First we search if such a node is already present
			if(n==NULL)
			{
				//Only if it is not present, than we insert the node

				Node *x=new Node();
				x->set_data(a,b);
				int dir=0,parent_a=0,parent_b=0;
				//Like search, we first determine its possible parent node (parent_a,parent_b)
				//and also whether our given node is left or right child

				if(b>a)
				{
					//RIGHT CHILD
					dir=1;
					parent_a=a;
					parent_b=b-a;
				}
				else
				{
					//LEFT CHILD
					dir=0;
					parent_a=a-b;
					parent_b=b;
				}
				
				//Then we search for the parent node in question
				Node *parent=this->tsearch(parent_a,parent_b);
				int pcount=0;
				if(parent==NULL)
				{
					//If parent node already not in tree, we need to insert it too
					pcount=tinsert(parent_a,parent_b);
					parent=this->tsearch(parent_a,parent_b);
				}
				//Then we set assign the parent's left or right child to the given node,
				//as determined earlier
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
				//Number of nodes inserted = 1 + number of nodes inserted for inserting parent
				return 1+pcount;


			}
		}
		return 0;//if no insertion takes place
	}
	
	int tdelete(int a,int b)
	{
		//Function for deleting a node
		Node *n=tsearch(a,b);
		if(n!=NULL)
		{
			//We can only delete a node, if it is actually there in the tree
			int dir=0,parent_a=0,parent_b=0;
			//Like search, we first determine its possible parent node (parent_a,parent_b)
			//and also whether our given node is left or right child
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

			//Then we search for the parent node in question
			Node *parent=tsearch(parent_a,parent_b);
			int a2,b2;
			int l=0,r=0;

			//Before deleting the node, we have to delete its children nodes
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

			//Then we delete the node by freeing its memory and setting left or right
			//child of node of parent to NULL, as determined above
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
			//Number of deletions = 1 + deletions for left node + deletions for right node
			return 1+l+r;
		}

		return 0;//if no deletions takes place
		
	}
	


};

void printTree(int level,int dir,Node *root)
{
	//Tree printing function for debugging 

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
	//Function for printing Preorder
	curr->printNode();
	if(curr->get_left())
		printPreorder(curr->get_left());
	if(curr->get_right())
		printPreorder(curr->get_right());
}
void printInorder(Node *curr)
{
	//Function for printing Inorder
	if(curr->get_left())
		printInorder(curr->get_left());
	curr->printNode();
	if(curr->get_right())
		printInorder(curr->get_right());
}

int max(int x,int y)
{
	//Function for calculating which is greater, x or y
	if(x>=y)
		return x;
	else 
		return y;
}

int find_MaxA(Node *curr)
{
	//Function for finding maximum value of a in tree
	if(curr==NULL)
		return 0;
	else if(curr->get_left()==NULL && curr->get_right()==NULL)
		return curr->get_a();
	else
	{
		//Maximum value = max( a of current node, max_a of left subtree, max_a of right subtree)
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
	//Debuggging function for printing number of elements in all the lists
	for(int i=1;i<=A;i++)
	{
		cout<<"List"<<i<<" ,Count = "<<L[i]->len()<<endl;
	}
}
void lex_rec(LinkedList **L,int index,int A)
{
	//Recursion function that prints the portion of lexOrder for each a
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
	//Function for printing Lexorder

	int A=find_MaxA(curr);
	//We define an array of linked lists, one for each a=1,2,..,A
	LinkedList *L[A+1];
	for(int i=1;i<=A;i++)
	{
		L[i]=new LinkedList(i);
	}
	L[1]->insert_sorted(curr);
	//print_arr_list(L,A);
	for(int i=1;i<=A;i++)
	{// Call recursive function for each a
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
