#include <iostream>
#include <cstdlib>
#include <algorithm>
#define EPSILON 0.00000000001
#define FILTER_X 0
#define FILTER_Y 1
using namespace std;


struct node
{
	pair<double, double> value;
	struct node* left;
	struct node* right;
};

struct node* preprocessTree(pair<double,double> p_sx[], pair<double,double> p_sy[], int Nx, int Ny, int depth);
bool sortX (pair<double,double> i,pair<double,double> j);
bool sortY (pair<double,double> i,pair<double,double> j);
void fillNode(int N, pair<double, double> left_s1[], pair <double, double> right_s1[], pair<double, double> original_s1[],
	      pair<double, double> median, pair<double, double> left_s2[], pair <double, double> right_s2[], 
	      pair<double, double> original_s2[], int filter_type);
int double_equal(double a, double b);
int double_lt(double a, double b);
int double_gt(double a, double b);
int main()
{
	//Takes in N points <x,y> and R <x1,x2,y1,y2> Ranges
	//x1<x2; y1<y2
	int N, R, i; 
	cin>>N>>R;
	pair<double,double> p_sx[N], p_sy[N];
	
	pair<double,double> r_x[R];
	pair<double,double> r_y[R];
	double a, b, c, d;
	//Input pointset
	for(i=0;i<N;i++)
	{
		cin>>p_sx[i].first>>p_sx[i].second;
		p_sy[i].first = p_sx[i].first;
		p_sy[i].second = p_sx[i].second;
	
	}
	
	for(i=0;i<R;i++)
	{
		cin>> a >> b >> c >> d;
		
		//Ensuring x1 < x2 & y1 < y2 
		// Handle degenrate case of x1 = x2 / y1 = y2?
		
		if(a<b)
		{
			r_x[i].first = a;
			r_x[i].second = b;
		} 
		else
		{
			r_x[i].first = b;
			r_x[i].second = a;
		}
		
		if(c<d)
		{
			r_y[i].first = c;
			r_y[i].second = d;
		} 
		else
		{
			r_y[i].first = d;
			r_y[i].second = c;
		}
		
	}
	//points sorted by the x coordinate
	
	sort(p_sx, p_sx+N, sortX);
	sort(p_sy, p_sy+N, sortY);
	
	/*for(i=0;i<N;i++)
	{
		cout<<p_sy[i].first<<"\t"<<p_sy[i].second<<endl;
	}
	*/
	struct node* root;
	root = preprocessTree(p_sx, p_sy, N, N, 0);
	
	cout<<"Root\n"<<root->value.first<< " "<<root->value.second;
}

struct node* preprocessTree(pair<double,double> p_sx[], pair<double,double> p_sy[], int Nx, int Ny,int depth)
{
	int i, N;
	if(depth%2 ==0) //We need median of x coordinates
	{
		N = Nx;
		
		if(N <= 0) return NULL;
		
		struct node* root = new node;
		
		pair<double,double>  p_sx_l[N/2];
		pair<double, double> p_sx_r [N/2];
		pair<double, double> p_sy_l[N/2];
		pair<double, double> p_sy_r[N/2];
		
		fillNode(N, p_sx_l, p_sx_r, p_sx, p_sx[N/2], p_sy_l, p_sy_r, p_sy, FILTER_X);
		
		root->value = p_sx[N/2];
		root->left = preprocessTree(p_sx_l, p_sy, N/2, N, depth+1);
		
		if(N%2) // Odd numbered call
			root->right = preprocessTree(p_sx_r, p_sy, N/2, N, depth+1);
		else // even numbered call
			root->right =  preprocessTree(p_sx_r, p_sy, N/2 - 1, N, depth+1);;
		
		return root;
		
	}
	else
	{
		/*N = Ny;
		
		if(N <= 0) return NULL;
		
		struct node* root = new node;
		pair<double,double> p_sy_l[N/2];
		pair<double, double> p_sy_r [N/2];

		fillNode(N, p_sy_l, p_sy_r, p_sy);

		root->value = p_sy[N/2];

		// Left Call
		root->left = preprocessTree(p_sy_l, p_sy, N/2, N, depth+1);
		if(N%2) // Odd numbered call
			root->right = preprocessTree(p_sy_r, p_sy, N/2, N, depth+1);
		else // even numbered call
			root->right =  preprocessTree(p_sy_r, p_sy, N/2 - 1, N, depth+1);;
		
		return root;*/
	
	}
	return NULL;
}


//This function spilts array original_s1 into twp parts from the center
//It then filters out all the elements from original_s2 into either left_s2 or right_s2 depending on
//filtering from the median value
void fillNode(int N, pair<double, double> left_s1[], pair <double, double> right_s1[], pair<double, double> original_s1[],
	      pair<double, double> median, pair<double, double> left_s2[], pair <double, double> right_s2[], 
	      pair<double, double> original_s2[], int filter_type)
{
		int i, j, r, l;

		if(N%2)
		{
			for(i = 0; i < N/2 ; i++)
			{
				left_s1[i]= original_s1[i];
				right_s1[i]= original_s1[i + N/2 + 1];
				
			}
		}
		else // Even Numbered Copy
		{
			for(i = 0; i < N/2 ; i++)
			{
				left_s1[i] = original_s1[i];
				
				if(i != ((N/2) -1) )
				{
					right_s1[i]= original_s1[i + N/2 + 1];
				}
			}
		}
		
		r=0;
		l=0;
		//filtering y based on median of X
		if(filter_type == FILTER_X)
		{
			
			for(i = 0;i < N; i++)
			{
				//Same record as the median - We dont want to store it!
				if( double_equal(median.first, original_s2[i].first) && 
				    double_equal (median.second, original_s2[i].second)  )
				{
					continue;
				}
				
				// The X's are far separated and the entity original_s2[i] lies on left of median
				else if(double_gt(median.first, original_s2[i].first))
				{
					left_s2[l++] = original_s2[i];
				} 	
				
				else if( double_lt(median.first, original_s2[i].first) )
				{
					right_s2[r++] = original_s2[i];
				} 	
				
				//same X value
				else if( double_equal(original_s2[i].first, median.first) )
				{
					if( double_lt(median.second, original_s2[i].second))
					{
						right_s2[r++] = original_s2[i];
					}
					else
					{
						left_s2[l++] = original_s2[i];
					}
				}
			}
		}

		cout<<"Median x "<<median.first<<" Median y"<< median.second<<endl;

		cout<<"Left x:\n";
		for(i = 0; i <N/2; i++)
			cout<<left_s1[i].first<<" "<<left_s1[i].second<<" | ";
		
		cout<<"\nRight x: \n";
		if(N%2)
		{
			for(i = 0; i <N/2; i++)
			cout<<right_s1[i].first<<" "<<right_s1[i].second<<" | ";
		}	
		else
		{
			for(i = 0; i <N/2 - 1; i++)
			cout<<right_s1[i].first<<" "<<right_s1[i].second<<" | ";
		}
		
		
		cout<<"Left y:\n";
		for(i = 0; i <N/2; i++)
			cout<<left_s2[i].first<<" "<<left_s2[i].second<<" | ";
		
		cout<<"\nRight y: \n";
		if(N%2)
		{
			for(i = 0; i <N/2; i++)
			cout<<right_s2[i].first<<" "<<right_s2[i].second<<" | ";
		}	
		else
		{
			for(i = 0; i <N/2 - 1; i++)
			cout<<right_s2[i].first<<" "<<right_s2[i].second<<" | ";
		
		}
		


}

int double_equal(double a, double b)
{
	if( ((a-b) < EPSILON && (a-b) > 0) || ((b-a) < EPSILON && (b-a) > 0) )
		return 1;
	return 0;
	     
}

int double_lt(double a, double b)
{
	if((b - a) > EPSILON) return 1;
	return 0;
}


int double_gt(double a, double b)
{
	if((a - b) > EPSILON) return 1;
	return 0;
}


bool sortX (pair<double,double> i,pair<double,double> j) 
{ 
	if(i.first < j.first)
		return true;
	else if (i.first == j.first && i.second < j.second)
		return true;
	else return false;	
}

bool sortY (pair<double,double> i,pair<double,double> j) 
{ 
	if(i.second < j.second)
		return true;
	else if (i.second == j.second && i.first < j.first)
		return true;
	else return false;	
}