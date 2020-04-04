#include<iostream>

using namespace std;

int check_cycle(int v,int n,int m,int *local_visited,int *visited,int *src,int *dest)
{
	if(local_visited[v]==1)
	{
		//cout<<"Returned 1 for vertex "<<v<<endl;
		return 1;
	}
	else if(visited[v]==1)
	{
		//cout<<"Returned 0 for vertex "<<v<<endl;
		return 0;
	}
	else
	{
		visited[v]=1;
		local_visited[v]=1;
		for(int i=0;i<m;i++)
		{
			if(src[i]==v)
			{
				//cout<<"Exploring edge "<<src[i]<<"->"<<dest[i]<<endl;
				int x=check_cycle(dest[i],n,m,local_visited,visited,src,dest);
				if(x==1)
				{
					//cout<<"Returned 1 for vertex "<<v<<endl;
					local_visited[v]=0;
					return 1;
				}
			}
		}
		local_visited[v]=0;
		//cout<<"Returned 0 for vertex "<<v<<endl;
		return 0;
	}
}
int check(int n,int m,int *src,int *dest)
{

	int visited[n]={0};
	//visited[] corresponds to whether we have ever encountered v in a path before,
	//if we have, we do not want to evaluate further from v, as we have already checked all paths diverging from v
	int local_visited[n];
	//local visited[] corresponds to visits in the current path,
	//we will detect a cycle only if we come across a vertex having local_visited[v]=1
	
	//looping across all vertices
	for(int v=0;v<n;v++)
	{
		if(visited[v]==1)
			continue;

		//local visited array have to be set to all 0's
		for(int i=0;i<n;i++)
		{
			local_visited[i]=0;
		}

		int curr_v=v;
		//cout<<"Current path from vertex = "<<curr_v<<endl;
		//finding if v is a part of a cycle
		int x=check_cycle(curr_v,n,m,local_visited,visited,src,dest);
		//cout<<"Visited array after checking path = "<<endl;
		//for(int i=0;i<n;i++)
		//{
		//	cout<<visited[i]<<" ";
		//}
		//cout<<"\n"<<endl;
		if(x==1)
		{
			return 1;
		}
	}
	return 0;
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
			cout<<"INVALID VERTICES GIVEN"<<endl;
			continue;
		}

		src[i]=x;
		dest[i]=y;
	}

	int q=check(n,m,src,dest);
	if(q==1)
	{
		cout<<"Graph contains cycle"<<endl;
	}
	else 
	{
		cout<<"Graph doesn't contain cycle"<<endl;
	}
}