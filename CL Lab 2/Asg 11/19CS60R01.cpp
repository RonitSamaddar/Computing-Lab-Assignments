#include<iostream>

using namespace std;
int check_cycle(int n,int m,int *src,int *dest)
{
	cout<<n<<" "<<m<<endl;
	for(int i=0;i<m;i++)
	{
		cout<<src[i]<<"->"<<dest[i]<<endl;
	}
}

int main()
{
	int n,m;
	cin>>n>>m;
	int src[m];
	int dest[n];
	int x,y;

	for(int i=0;i<m;i++)
	{
		cin>>x>>y;
		if(x<0 || x>=n || y<0 || y>=n)
		{
			cout<<"INVALID VERTICES GIVEN";
			continue;
		}

		src[i]=x;
		dest[i]=y;
	}

	check_cycle(n,m,src,dest);
}