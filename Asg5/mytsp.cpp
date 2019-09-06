#include <glpk.h>
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<cmath>

using namespace std;

//SAMPLE LINEAR PROGRAMMING PROBLEM GIVEN AT THE END

class Point
{
public:
	double x;
	double y; 
};

double dist(Point P,Point Q)
{
	return sqrt(pow((P.x-Q.x),2)+pow((P.y-Q.y),2));
}

void tpsolve1(int n, Point *points)
{
	cout<<"HELLO PLZ"<<endl;
	//n = number of points
	//points = array of point coordinates


	//We consider Xij in the optimization problem as Xk where k= i*n+j
	
	int flag=1;

	while(flag==1)
	{
		cout<<"HELLO2"<<endl;
		//DEFINING GPLK PROBLEM VARIABLE
		glp_prob *tsp;
		cout<<"HELLO3"<<endl;

		//DEFINING ARRAYS FOR INDEXES FOR FINAL ARRAY, number of rows(i) = number of constraints, number of columns = number of variables
		int *ca=new int[100000000];
		int *va=new int[100000000];

		//DEFINING ARRAY FOR FINAL ARRAY VALUES
		double ar[100000000];


		cout<<"HELLO4"<<endl;
		//CREATING PROBLEM AND SETTING NAME
		tsp = glp_create_prob();
		glp_set_prob_name(tsp, "My Travelling Salesman Problem");

		//SETTING OBJECTIVE = MAX/MIN
		glp_set_obj_dir(tsp, GLP_MIN);

		//SETTING NUMBER OF CONSTRAINSTS = NUMBER OF ROWS OF FINAL ARRAY
		int row_count = n+n+n*n; //number of constraints = n entering constraints + n leaving constraints + (n*n)/2 no 2-way leaving constraints
		glp_add_rows(tsp,row_count);

		//SETTING NAME,LIMITS FOR EACH CONSTRAINT
		int c_index=0;
		//Entering Constraints

		for(int j=0;j<=n-1;j++)
		{
			cout<<"\nDefining Entering Constraint for j = "<<j<<endl;
			cout<<"Constraint Number = "<<c_index<<endl;
			glp_set_row_bnds(tsp, c_index, GLP_DB, 1.0, 1.0);
			c_index++;
		}
		//Leaving Constraints		
		for(int i=0;i<=n-1;i++)
		{
			cout<<"\nDefining Leaving Constraint for i = "<<i<<endl;
			cout<<"Constraint Number = "<<c_index<<endl;
			glp_set_row_bnds(tsp, c_index, GLP_DB, 1.0, 1.0);
			c_index++;
		}
		//No 2-way cycle constraints		
		for(int k=0;k<n*n;k++)
		{

			cout<<"\nDefining No 2-way cycle Constraint for (i,j) = ("<<k/n<<","<<k%n<<")"<<endl;
			cout<<"Constraint Number = "<<c_index<<endl;
			glp_set_row_bnds(tsp, c_index, GLP_DB, 0.0, 1.0);
			c_index++;
		}

		//END OF CONSTRAINTS

		/*
		//SETTING NUMBER OF VARIABLES = NUMBER OF COLUMNS OF FINAL ARRAY
		int var_count=n*n;//Number of variables
		glp_add_cols(tsp, var_count);//since elements are of the form Xij so total number of elements = n * n


		//SETTING NAME AND BOUNDS AND OPTIMIZATION FUNCTION COEFFICIENT FOR EACH VARIABLE
		int v_index=1;
		for(int k=1;k<=n*n;k++)
		{
			cout<<"Defining variable for (i,j) = ("<<k/n<<","<<k%n<<")"<<endl;
			char buf[3]={0};
			sprintf(buf,"x%d",k);
			glp_set_col_name(tsp,k,buf);
			glp_set_col_bnds(tsp,k,GLP_DB, 0.0, 1.0);
			int i=k/n;
			int j=k%n;
			int d=dist(points[i],points[j]);
			glp_set_obj_coef(tsp, k, d);
		}



		cout<<"SETTING CONSTRAINT COEFFICIENTS!!!"<<endl;
		//SETTING CONSTRAINTS COEFFICIENT FOR EACH VARIABLE IN EACH ROW OF CONSTRAINTS
		for(int v=1;v<=n*n;v++)//j is the number of columns = number of variables
		{

			//variable = Xk where k=v
			//Inner loops = loop over the constraints
			for(int c=1;c<=n+n+n*n;c++)
			{


				va[(c-1)*n*n+(v-1)]=v;
				ca[(c-1)*n*n+(v-1)]=c;

				if(c>=1 && c<=n)//Entering constraints
				{
					cout<<"Constraint = "<<c<<"-th entering constraint, variable number = "<<v<<endl; 
					//c = value of j
					//Therefore this iteration corresponds to the j-th entering constraints
					//here a = 1 for all i and for j==c
					int j=v%n;
					if(j==c)
					{
						ar[(c-1)*n*n+(v-1)]=1;
					}
					else
					{
						ar[(c-1)*n*n+(v-1)]=0;
					} 
				}

				if(c>=n+1 && c<=n+n)//Leaving constraints
				{
					int c1=c-n;
					cout<<"Constraint = "<<c1<<"-th leaving constraint, variable number = "<<v<<endl; 
					//c1 = value of i
					//Therefore iteration c1 corresponds to the i-th entering constraints
					//here a = 1 for all j and for i==c1
					int i=v/n;
					if(i==c1)
					{
						ar[(c-1)*n*n+(v-1)]=1;
					}
					else
					{
						ar[(c-1)*n*n+(v-1)]=0;
					} 
				}

				if(c>n+n)//No Two way constraints
				{
					int c1=c-2*n;
					cout<<"Constraint = "<<c1<<"-th no-2-way-cycle constraint, variable number = "<<v<<endl; 
					//here c1 corresponds to the kth 2-way constraint 
					//that is for c1=k=i*n+j
					//Xij+xji<=1;
					//Therefore here for k = i*n+j, we can define k'=j*n+1
					//Therefore a[]=1 for all v(=k) where v==c1 and v'(=k')==c1
					int i=v/n;
					int j=v%n;
					if(i*n+j==c1 || j*n+i==c1)// that is for c1, k-th and k'th element ==1
					{
						ar[(c-1)*n*n+(v-1)]=1;
					}
					else
					{
						ar[(c-1)*n*n+(v-1)]=1;
					}


				}

			}

		}

		//SETTING UP THE PROBLEM BY LOADING THE MATRICES
		glp_load_matrix(tsp, row_count*var_count, ca, va, ar);

		//CALLING SIMPLEX
		glp_simplex(tsp, NULL);
		int z = glp_get_obj_val(tsp); //GETTING OPTIMIZED FUNCTION VALUE
		//GETTING VARIABLES VALUES WHICH GIVE US OPTIMUM
		for(int k=1;k<=n*n;k++)
		{
			int xk=glp_get_col_prim(tsp, k);
			int i=k/n;
			int j=k*n;
			cout<<"x("<<i<<","<<j<<") = "<<xk<<endl;
		}
		glp_delete_prob(tsp);
	
		*/
		flag=0;
	}


}

int main()
{
	int n;
	cin>>n;
	Point *P=new Point[n];
	for(int i=0;i<n;i++)
	{
		double x,y;
		Point Q;
		cin>>x>>y;
		Q.x=x;
		Q.y=y;
		P[i]=Q;
	}
	cout<<"\n\nn = "<<n<<endl;
	for(int i=0;i<n;i++)
	{
		cout<<P[i].x<<"  "<<P[i].y<<endl;
	}
	cout<<"HELLO"<<endl;
	tpsolve1(n,P);
}


















/*
//sample.c 
#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>
int main(void)
{

//DEFINING GPLK PROBLEM VARIABLE
glp_prob *lp;

//DEFINING INDEXES FOR FINAL ARRAY
int ia[1+1000], ja[1+1000];

//DEFINING FINAL ARRAY VALUES
double ar[1+1000], z, x1, x2, x3;

//CREATING PROBLEM AND SETTING NAME
s1:		lp = glp_create_prob();
s2:		glp_set_prob_name(lp, "sample");

//SETTING OBJECTIVE = MAX/MIN
s3:		glp_set_obj_dir(lp, GLP_MAX);


//SETTING NUMBER OF CONSTRAINSTS = NUMBER OF ROWS OF FINAL ARRAY
s4:		glp_add_rows(lp, 3);

//SETTING NAME,LIMITS FOR EACH CONSTRAINT
s5:		glp_set_row_name(lp, 1, "p");
s6:		glp_set_row_bnds(lp, 1, GLP_UP, 0.0, 100.0);
s7:		glp_set_row_name(lp, 2, "q");
s8:		glp_set_row_bnds(lp, 2, GLP_UP, 0.0, 600.0);
s9:		glp_set_row_name(lp, 3, "r");
s10: 	glp_set_row_bnds(lp, 3, GLP_UP, 0.0, 300.0);


//SETTING NUMBER OF VARIABLES = NUMBER OF COLUMNS OF FINAL ARRAY
s11: 	glp_add_cols(lp, 3);

//SETTING NAME AND BOUNDS AND OPTIMIZATION FUNCTION COEFFICIENT FOR EACH VARIABLE
s12: 	glp_set_col_name(lp, 1, "x1");
s13: 	glp_set_col_bnds(lp, 1, GLP_LO, 0.0, 0.0);
s14: 	glp_set_obj_coef(lp, 1, 10.0);
s15: 	glp_set_col_name(lp, 2, "x2");
s16: 	glp_set_col_bnds(lp, 2, GLP_LO, 0.0, 0.0);
s17: 	glp_set_obj_coef(lp, 2, 6.0);
s18: 	glp_set_col_name(lp, 3, "x3");
s19: 	glp_set_col_bnds(lp, 3, GLP_LO, 0.0, 0.0);
s20: 	glp_set_obj_coef(lp, 3, 4.0);

//SETTING CONSTRAINTS COEFFICIENT FOR EACH VARIABLE IN EACH ROW OF CONSTRAINTS
s21: 	ia[1] = 1, ja[1] = 1, ar[1] = 1.0;  		means a[1,1] = 1
s22: 	ia[2] = 1, ja[2] = 2, ar[2] = 1.0; 			means a[1,2] = 1
s23: 	ia[3] = 1, ja[3] = 3, ar[3] = 1.0;  		means a[1,3] = 1
s24: 	ia[4] = 2, ja[4] = 1, ar[4] = 10.0;  		means a[2,1] = 10
s25: 	ia[5] = 3, ja[5] = 1, ar[5] = 2.0;  		means a[3,1] = 2
s26: 	ia[6] = 2, ja[6] = 2, ar[6] = 4.0;  		means a[2,2] = 4
s27: 	ia[7] = 3, ja[7] = 2, ar[7] = 2.0;
s28: 	ia[8] = 2, ja[8] = 3, ar[8] = 5.0;
s29:	ia[9] = 3, ja[9] = 3, ar[9] = 6.0;

//SETTING UP THE PROBLEM BY LOADING THE MATRICES
s30:	glp_load_matrix(lp, 9, ia, ja, ar);

//CALLING SIMPLEX
s31:	glp_simplex(lp, NULL);
s32:	z = glp_get_obj_val(lp); //GETTING OPTIMIZED FUNCTION VALUE
//GETTING VARIABLES VALUES WHICH GIVE US OPTIMUM
s33:	x1 = glp_get_col_prim(lp, 1);
s34:	x2 = glp_get_col_prim(lp, 2);
s35:	x3 = glp_get_col_prim(lp, 3);
s36:	printf("\nz = %g; x1 = %g; x2 = %g; x3 = %g\n",z, x1, x2, x3);
s37:	glp_delete_prob(lp);
return 0






*/