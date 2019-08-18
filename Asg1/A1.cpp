#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>


using namespace std;

class Point;
class Polygon;
class Pixel;
class Vert_Strip;
class Point_Polygon;
class Point_Angle;

void print_Line(Point P,Point Q);
int on_line_seg(Point P1,Point P2,Point P);
Point *segint(Point,Point,Point,Point);
int side(Point,Point,Point);
int pointpos(Polygon,Point);
double tarea(Point,Point,Point);
double parea(Polygon,int);
Pixel get_pixel(int,int);
double aoi1(Polygon,Polygon,int);
Vert_Strip get_strip(int,int);
Point_Polygon *sort_polygonpoints(Point_Polygon *,int);
void print_polygonpoints(Point_Polygon *,int);
int points_below(Point *,Point *);
double aoi2(Polygon,Polygon,int);
double get_overlap(Point *,Point *);

int get_edge_intersections(Point *,int,Polygon,Polygon);
int get_corners(Point *,int,Polygon,Polygon);
void sort_anglePoints(Point_Angle *,int);
double aoi3(Polygon R1,Polygon R2);



class Point
{
//class to represent a point by its x and y coordinates
public:
	double x;
	double y;

	Point(double x,double y)
	{
		this->x=x;
		this->y=y;
	}
	Point()
	{

	}
	void print_Point()
	{
		//function to print a representation of a point
		cout<<"("<<this->x<<","<<this->y<<")";
	}
	double slope_with(Point P)
	{
		//function to compute slope of line segment from point P to given point
		double m=(this->y-P.y)/(this->x-P.x);
		return m;
	}
	double sq_dist_with(Point P)
	{
		//compute square of the distance between point P and given point
		double dist=pow(this->x-P.x,2)+pow(this->y-P.y,2);
		return dist;
	}

};

class Polygon
{
//class to represent a Polygon by its list of points and count of number of points
public:
	Point Pt[8];
	int count;

	Polygon()
	{
		this->count=0;
	}
	void add_point(Point P)
	{
		//function to add a point to the Polygon set of points
		this->Pt[this->count++]=P;
	}
	Point *get_intersection_with_line(Point P1,Point P2)
	{
		//function to get points where a line(P1->P2)intersects the given polygon
		//for a convex polygon, any line can maximum intersect polygon at 2 points
		Point *IP=new Point[2];
		int index=0; 
		for(int i=0;i<this->count;i++)
		{
			
			Point *P=segint(P1,P2,this->Pt[i],this->Pt[(i+1)%this->count]);
			if(P!=NULL)
			{
				IP[index++]=*P;
				if(index==2)
				{
					break;
				}
			}
			

		}
		return IP;
	}
	
};

class Pixel
{
//class to represent a pixel for the Graphical Method
public:
	double x1;
	double x2;
	double y1;
	double y2;
	Pixel()
	{
		;
	}
	Pixel(double x1,double y1,double x2,double y2)
	{
		this->x1=x1;
		this->y1=y1;
		this->x2=x2;
		this->y2=y2;
	}
	void print_Pixel()
	{
		//function to print representation of pixel
		cout<<"("<<this->x1<<","<<this->y1<<")-->("<<this->x2<<","<<this->y2<<")"<<endl;
	}
	Point get_centre()
	{
		//function to get centre point of the given pixel;
		double xmid=(this->x1+this->x2)*1.0/2;
		double ymid=(this->y1+this->y2)*1.0/2;
		Point *P=new Point(xmid,ymid);
		return *P;
	}
};

class Vert_Strip
{
//class to represent a Vertical Strip for the Numerical Method
public:
	Pixel VS;
	Vert_Strip(double x1,double x2)
	{
		this->VS=*(new Pixel(x1,0,x2,1));
	}
	void print_strip()
	{
		//function to print a representation of a vertical strip
		cout<<this->VS.x1<<"-->"<<this->VS.x2<<endl;
	}
	double get_midline()
	{
		//function to get the midline of the vertical strip
		Point P=VS.get_centre();
		double x=P.x;
		return x;
	}
};

class Point_Polygon
{
//class to store a point and corresponding polygon number together 
public:
	Point P;
	int poly;
	Point_Polygon()
	{

	}



};

class Point_Angle
{
	//class to have a structure to store a point and its correponding angle to centre together
public:
	Point P;
	double angle;


};


void print_Line(Point P,Point Q)
{
	//function to print a representation of a line, if needed
	cout<<"("<<P.x<<","<<P.y<<")--->("<<Q.x<<","<<Q.y<<")"<<endl;

}

int on_line_seg(Point P1,Point P2,Point P)
{
	//function to determine if a Point P lying in line joining P1->P2 lies inside the
	//line segment or not
	double m2=P.sq_dist_with(P1);
	double n2=P.sq_dist_with(P2);
	double p2=P1.sq_dist_with(P2);
	if(m2+n2<p2)//if sum of square of distances from each end point is less than whole distance
		//point lies inside the line segment
		return 1;
	else
		return 0;
}
Point *segint(Point P1,Point P2,Point Q1,Point Q2)
{
	//function to find out point of intersection of line segments P1->P2 and Q1->Q2
	double mp,mq,x_int,y_int;
	if(P1.x==P2.x)
	{ 	//special case where first line || to y axis,so slope = infinity
		if(Q1.x==Q2.x)
		{
			// if second line is also ||to y axis,no intersections,
			return NULL;
		}
		else
		{
			//finding intersection
			mq=Q2.slope_with(Q1);
			x_int=P1.x;//x will be the same, as all points of first line have same x
			y_int=Q1.y+mq*(x_int-Q1.x);// y can be found in terms of x

		}
		
	}
	else if(Q1.x==Q2.x)
	{
		//special case where second line || to y axis,so slope = infinity
		mp=P2.slope_with(P1);
		x_int=Q1.x;
		y_int=P1.y+mp*(x_int-P1.x);
	}
	else
	{
		// general case where no line is parallel to y axis
		mp=P2.slope_with(P1);
		mq=Q2.slope_with(Q1);
		//cout<<"mp = "<<mp<<endl;
		//cout<<"mq = "<<mq<<end;
		x_int=(Q1.y-P1.y+(mp*P1.x-mq*Q1.x))/(mp-mq);
		y_int=(mp*(x_int-P1.x)+P1.y);
	}
	
	Point *P=new Point(x_int,y_int);
	//cout<<"The possible Point of Intersection "<<endl;
	//P->print_Point();

	if(on_line_seg(P1,P2,*P)==1&&on_line_seg(Q1,Q2,*P)==1)
	{// checking if the point of intersection, actually lies inside the line segment for both
	 // the line segments,if so we return the point
	 // else we return NULL	
		return P;
	}
	else
	{
		return NULL;
	}
	

}

int side(Point P1,Point P2,Point P)
{
	//function to compute where a point P lies with respect to a line P1->P2
	double D=P2.x*P.y-P.x*P2.y+P1.x*P2.y-P1.x*P.y+P.x*P1.y-P2.x*P1.y;
	if(D>0)
		return 1;//left
	else if(D<0)
		return -1;//right
	else
		return 0;//on line
}

int pointpos(Polygon R,Point P)
{
	// function to check if point P lies inside Polygon R or not
	// as all points are taken in counter clockwise direction, for P to be inside R
	// P should be to left of every line segment in R
	int flag=1;
	for(int i=0;i<R.count;i++)
	{
		
		if(side(R.Pt[i],R.Pt[(i+1)%R.count],P)!=1)
		{
			// that is if for any line segment/edge, P does not lie to left of it,
			// we break out of loop and return 0		
			flag=0;
			break;
		}
		
	}
	return flag; // 1 = inside, 0 = outside
}

double tarea(Point A,Point B,Point C)
{
	//function to calculate area of triangle ABC

	double area=0.5*(A.x*(B.y-C.y)+B.x*(C.y-A.y)+C.x*(A.y-B.y));
	return area;
}

double parea(Polygon P,int n)
{
	// function to calculate area of polygon by adding up area of triangles 012,023,034,...0(n-2)(n-1)
	double area=tarea(P.Pt[0],P.Pt[1],P.Pt[2]);
	for(int i=3;i<n;i++)
	{
		area=area+tarea(P.Pt[0],P.Pt[i-1],P.Pt[i]);
	}
	return area;
}

Pixel get_pixel(int index,int N)
{
	// for the graphical method, this function returns Pixel object corresponding
	// to pixel index = index
	
	//calculating row and column number of pixel in grid
	int i=index/N;
	int j=index%N;

	//calculating bounds of the pixel
	double x1=j*1.0/N;
	double x2=(j+1)*1.0/N;
	double y1=i*1.0/N;
	double y2=(i+1)*1.0/N;
	Pixel *P=new Pixel(x1,y1,x2,y2);
	return *P;
}

double aoi1(Polygon R1,Polygon R2,int N)
{
	//Dividing the entire U into pixels numbering starting from bottom right(touching 0,0)
	//as the first pixel and then first moving rightwards and then upwards like
	// 6 7 8
	// 3 4 5
	// 0 1 2

	int count=0;
	for(int i=0;i<N*N;i++)
	{
		Pixel Pix=get_pixel(i,N);
		
		Point Pt=Pix.get_centre();// find the midpoint of pixel Pix
		
		if(pointpos(R1,Pt)==1 && pointpos(R2,Pt)==1)
		{
			//if midpoint of Pix lies inside both the quadrilaterals,then this pixel is considered
			count++;
			
			

		}
		
	}
	return count*1.0/(N*N);
}

Vert_Strip get_strip(int index,int N)
{
	// In Numerical Method, returns Vertical Strip corresponding to given index "index"
	
	//calculating horizontal bounds of the strip
	double x1=index*1.0/N;
	double x2=(index+1)*1.0/N;

	Vert_Strip VS=*(new Vert_Strip(x1,x2));
	return VS;
}
Point_Polygon *sort_polygonpoints(Point_Polygon *IP,int count)
{
	// Takes an array of Point_Polygon objects as inputs
	// Objects will be sorted according to order of y coordinates of their points
	
	for(int i=1;i<count;i++)
	{
		int t=i-1;
		Point_Polygon Val=IP[i];
		while(t>=0 && IP[t].P.y>Val.P.y)
		{
			IP[t+1]=IP[t];
			t--;
			
		}
		IP[t+1]=Val;
		
	}
	return IP;
}

double get_overlap(Point *P1,Point *P2)
{
	// for Numerical method, when the middle line of the vertical strip passes through the
	// two quadrilaterals, it will bisect each quadrilateral in max 2 points, we combine
	// these points(P1,P2) into a combined array of Point_Polygon objects and then get the 
	//	vertical overlap if any
	Point_Polygon *IP=new Point_Polygon[4];
	
	IP[0].P=P1[0];
	IP[0].poly=1;

	IP[1].P=P1[1];
	IP[1].poly=1;

	IP[2].P=P2[0];
	IP[2].poly=2;

	IP[3].P=P2[1];
	IP[3].poly=2;


	// sorting the points vertically
	IP=sort_polygonpoints(IP,4);


	double overlap;
	if(IP[0].poly==IP[1].poly && IP[2].poly==IP[3].poly)
	{//CASE 1:NO OVERLAP
		overlap=0;
	}
	else if(IP[0].poly==IP[3].poly && IP[1].poly==IP[2].poly)
	{//CASE 2:One completely inside another
		overlap=IP[2].P.y-IP[1].P.y;
		
	}
	else if(IP[0].poly==IP[2].poly && IP[1].poly==IP[3].poly)
	{//Case 3: General Overlap;
		overlap=IP[2].P.y-IP[1].P.y;

	}
	return overlap;

}

double aoi2(Polygon R1,Polygon R2,int N)
{

	double area=0;
	for(int i=0;i<N;i++)
	{
		Vert_Strip VS=get_strip(i,N);
		double x=VS.get_midline();// get the middle line for vertical strip VS
		Point *Q1=new Point(x,0);
		Point *Q2=new Point(x,1);
		Point *IP1=R1.get_intersection_with_line(*Q1,*Q2);// getting intersection points of middle line with quadrilateral R1
		Point *IP2=R2.get_intersection_with_line(*Q1,*Q2);// getting intersection points of middle line with quadrilateral R2
		double contrib=(get_overlap(IP1,IP2))*1.0/N;// getting overlap;
		area=area+contrib;


	}
	return area;
}

int get_edge_intersections(Point *IP,int IPcount,Polygon R1,Polygon R2)
{
	//function to compute edge intersections of R1 and R2 and add intersection points in IP 
	int edge_count=0;
	
	for(int i=0;i<R1.count;i++)
	{
		for(int j=0;j<R2.count;j++)
		{
			// we run loops across each of the quadrilateral's edges to get each combination of edges
			// and find the point of intersection for them
			Point *P=segint(R1.Pt[i],R1.Pt[(i+1)%R1.count],R2.Pt[j],R2.Pt[(j+1)%R2.count]);

			if(P!=NULL)
			{
				// if there is a point of intersection,add it to the array IP
				IP[IPcount++]=*P;
				edge_count++;
			}
		}
	}
	cout<<"\nNumber of edge intersections = "<<edge_count<<endl;
	return IPcount;
	
}
int get_corners(Point *IP,int IPcount,Polygon R1,Polygon R2)
{
	// function to find the corner points/ vertices of quadrilaterals R1,R2 which also form a
	//part of the intersection polygon
	
	// if any corner point lies inside the other quadrilateral,then it is a vertex of the
	// intersection polygon
	for(int i=0;i<R1.count;i++)
	{
		if(pointpos(R2,R1.Pt[i])==1)
		{
			IP[IPcount++]=R1.Pt[i];
			
		}
	}
	for(int i=0;i<R2.count;i++)
	{
		if(pointpos(R1,R2.Pt[i])==1)
		{
			IP[IPcount++]=R2.Pt[i];
			
		}
	}
	return IPcount;
}
void sort_anglePoints(Point_Angle *AP,int count)
{
	// Takes as input the array of Point_Angle objects
	// Objects will be sorted according to the order of the y coordinate of their point
	
	for(int i=1;i<count;i++)
	{
		
		int t=i-1;
		Point_Angle Val=AP[i];
		while(t>=0 && AP[t].angle>Val.angle)
		{
			
			
			AP[t+1]=AP[t];
			t--;
			
		}
		AP[t+1]=Val;
		
		
	}
	
}

double aoi3(Polygon R1,Polygon R2)
{
	Point *IP=new Point[8];
	int IPcount=0;

	//get edge intersections
	IPcount=get_edge_intersections(IP,IPcount,R1,R2);

	// find centre for these points
	double centx=0,centy=0;
	for(int i=0;i<IPcount;i++)
	{
		centx+=IP[i].x;
		centy+=IP[i].y;
	}
	centx/=IPcount;
	centy/=IPcount;


	//find angle for these points and sort them to get counterclockwise order
	Point_Angle *PA=new Point_Angle[IPcount];
	for(int i=0;i<IPcount;i++)
	{
		PA[i].P=IP[i];
		PA[i].angle=atan2(IP[i].y-centy,IP[i].x-centx);
	}
	sort_anglePoints(PA,IPcount);

	//print the points
	for(int i=0;i<IPcount;i++)
	{
		PA[i].P.print_Point();
		cout<<endl;	
	}

	//get corners of quadrilaterals which also are corners of intersection polygon
	IPcount=get_corners(IP,IPcount,R1,R2);
	
	// find centre for these points
	centx=0;
	centy=0;
	for(int i=0;i<IPcount;i++)
	{
		centx+=IP[i].x;
		centy+=IP[i].y;
	}
	centx/=IPcount;
	centy/=IPcount;

	//find angle for these points and sort them to get counterclockwise order
	PA=new Point_Angle[IPcount];
	for(int i=0;i<IPcount;i++)
	{
		PA[i].P=IP[i];
		PA[i].angle=atan2(IP[i].y-centy,IP[i].x-centx);
	}
	sort_anglePoints(PA,IPcount);

	// add these points to a polygon object
	Polygon Intersect=*(new Polygon());
	for(int i=0;i<IPcount;i++)
	{
		Intersect.add_point(PA[i].P);
	}
	cout<<"\nNumber of Intersection Points = "<<IPcount<<endl;
	for(int i=0;i<IPcount;i++)
	{
		PA[i].P.print_Point();
		cout<<endl;
	}

	//compute area for that polygon object
	return parea(Intersect,IPcount);


}


int main()
{
	
	
	Polygon *Poly1=new Polygon();
	cout<<"Rectangle 1"<<endl;
	for(int i=0;i<4;i++)
	{
		double x,y;
		cin>>x>>y;
		Point *P1=new Point(x,y);
		Poly1->add_point(*P1);
	}

	Polygon *Poly2=new Polygon();
	cout<<"Rectangle 2"<<endl;
	for(int i=0;i<4;i++)
	{
		double x,y;
		cin>>x>>y;
		Point *P2=new Point(x,y);
		Poly2->add_point(*P2);
	}

	int N;
	double area;
	cout<<"Graphical Method:"<<endl;
	for(int i=1;i<=3;i++)
	{
		N=pow(10,i);
		area=aoi1(*Poly1,*Poly2,N);
		cout<<"N =";
		for(int j=0;j<7-i;j++)
		{
			cout<<" ";
		}
		cout<<N<<". Area of intersection = "<<area<<endl;

	}

	cout<<"\nNumerical Method:"<<endl;
	for(int i=1;i<=6;i++)
	{
		N=pow(10,i);
		area=aoi2(*Poly1,*Poly2,N);
		cout<<"N =";
		for(int j=0;j<7-i;j++)
		{
			cout<<" ";
		}
		cout<<N<<". Area of intersection = "<<area<<endl;

	}

	cout<<"\nGeometric Method:"<<endl;
	area=aoi3(*Poly1,*Poly2);
	cout<<"\nArea of intersection = "<<area<<endl;
}




