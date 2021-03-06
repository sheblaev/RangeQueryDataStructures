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
	double *y_root;
	int Ny;
};

struct node* preprocessTree(pair<double,double> p_sx[], pair<double,double> p_sy[], int N, int depth);
bool sortX (pair<double,double> i,pair<double,double> j);
bool sortY (pair<double,double> i,pair<double,double> j);
int searchTree(struct node* root, double x1, double x2, double y1, double y2, int depth);
int double_equal(double a, double b);
int double_lt(double a, double b);
int double_gt(double a, double b);
void inorder(struct node* root);
void preorder(struct node* root);
void fillNode(int N, pair<double, double> left_s1[], pair <double, double> right_s1[], pair<double, double> original_s1[],
	      pair<double, double> median, pair<double, double> left_s2[], pair <double, double> right_s2[], 
	      pair<double, double> original_s2[], int filter_type);
int main()
{
	//Takes in N points <x,y> and R <x1,x2,y1,y2> Ranges
	//x1<x2; y1<y2
	int N, R, i; 
	cin>>N>>R;
	pair<double,double> *p_sx; 
	p_sx = (pair<double,double>*)malloc(sizeof(pair<double,double>)*N);
	pair <double, double> *p_sy;
	p_sy = (pair<double,double>*)malloc(sizeof(pair<double,double>)*N);
	
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
		
		if(double_lt(a,b))
		{
			r_x[i].first = a;
			r_x[i].second = b;
		} 
		else
		{
			r_x[i].first = b;
			r_x[i].second = a;
		}
		
		if(double_lt(c,d))
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
	root = preprocessTree(p_sx, p_sy, N, 0);
	free(p_sx);
	free(p_sy);
	//cout<<"Root\n"<<root->value.first<< " "<<root->value.second;
/*	cout<<"Ineorder \n";
	inorder(root);
	cout<<"Preorder \n";
	preorder(root);
	//cout<<r_x[0].first<<" "<<r_x[0].second<<" "<<r_y[0].first<<" "<<r_y[0].second;
*/	for(i=0;i<R;i++)
	{
		cout<<searchTree(root, r_x[i].first, r_x[i].second, r_y[i].first, r_y[i].second, 0);
		cout<<endl;
	}

}

struct node* preprocessTree(pair<double,double> p_sx[], pair<double,double> p_sy[], int N, int depth)
{
	int i;
	if(N <= 0) return NULL;
	struct node* root = new node;
	pair<double,double>  *p_sx_l;
	pair<double, double> *p_sx_r;
	pair<double, double> *p_sy_l;
	pair<double, double> *p_sy_r;
	
	p_sx_l = (pair<double,double>*)malloc(sizeof(pair<double,double>)*(N/2));
	p_sx_r = (pair<double,double>*)malloc(sizeof(pair<double,double>)*(N/2));
	p_sy_l = (pair<double,double>*)malloc(sizeof(pair<double,double>)*(N/2));
	p_sy_r = (pair<double,double>*)malloc(sizeof(pair<double,double>)*(N/2));
	
	if(depth%2 ==0)
	{
		fillNode(N, p_sx_l, p_sx_r, p_sx, p_sx[N/2], p_sy_l, p_sy_r, p_sy, FILTER_X);
		root->value = p_sx[N/2];
		root->left = preprocessTree(p_sx_l, p_sy_l, N/2, depth+1);
		free(p_sx_l);
		free(p_sy_l);
		
		if(N%2)
			root->right = preprocessTree(p_sx_r, p_sy_r, N/2, depth+1);
		else 
			root->right =  preprocessTree(p_sx_r, p_sy_r, N/2 - 1,  depth+1);;
		
		free(p_sx_r);
		free(p_sy_r);
		
		
		return root;
		
	}
	else
	{
		
		struct node* root = new node;
		fillNode(N, p_sy_l, p_sy_r, p_sy, p_sy[N/2], p_sx_l, p_sx_r, p_sx, FILTER_Y);
		root->value = p_sy[N/2];
		root->left = preprocessTree(p_sx_l, p_sy_l, N/2, depth+1);
		free(p_sx_l);
		free(p_sy_l);
	
		if(N%2) 
			root->right = preprocessTree(p_sx_r, p_sy_r, N/2, depth+1);
		else 
			root->right =  preprocessTree(p_sx_r, p_sy_r, N/2 - 1, depth+1);;
		
		free(p_sx_r);
		free(p_sy_r);
		
		
		return root;
	}

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
					//cout<<"qee";
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

		else if(filter_type == FILTER_Y)
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
				else if(double_gt(median.second, original_s2[i].second))
				{
					left_s2[l++] = original_s2[i];
				} 	
				
				else if( double_lt(median.second, original_s2[i].second) )
				{

					right_s2[r++] = original_s2[i];
				} 	
				
				//same X value
				else if( double_equal(original_s2[i].second, median.second) )
				{
				
					if( double_lt(median.first, original_s2[i].first))
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
		
		/*cout<<"Original S2";
		for(i=0;i<N;i++)
		{
			cout<<original_s2[i].first<<"\t"<<original_s2[i].second<<endl;
		}
		*/
	/*	cout<<"Median x "<<median.first<<" Median y"<< median.second<<endl;

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
		
		
		cout<<"\nLeft y:\n";
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
		
		cout<<"\n************\n";
*/

}

int searchTree(struct node* root, double x1, double x2, double y1, double y2, int depth)
{
	double node_x, node_y;
	int left_ans;
	int right_ans;
	
	if(root == NULL) return 0;

	node_x = root->value.first;
	node_y = root->value.second;	

	if(depth%2 == 0)
	{
		if( (double_gt(node_x, x1) && double_lt(node_x,x2)) || 
		     double_equal(node_x, x1) || double_equal(node_x, x2) )
		{
			
			left_ans = searchTree(root->left, x1, x2, y1, y2, depth+1);
			right_ans = searchTree(root->right, x1, x2, y1, y2, depth+1);
			
			if( (double_gt(node_y, y1) && double_lt(node_y,y2) ) || 
		     	     double_equal(node_y, y1) || double_equal(node_y, y2) )
		     		
				return 1 + left_ans + right_ans;
			
			else return left_ans + right_ans;
		}
		
		if(double_lt(node_x, x1))
		{
			return searchTree(root->right, x1, x2, y1, y2, depth+1);
		}
		else if(double_gt(node_x, x2))
		{
			return searchTree(root->left, x1, x2, y1, y2, depth+1);
		}
	}
	else
	{
		if( (double_gt(node_y, y1) && double_lt(node_y,y2)) || 
		     double_equal(node_y, y1) || double_equal(node_y, y2) )
		{
			
			left_ans = searchTree(root->left, x1, x2, y1, y2, depth+1);
			right_ans = searchTree(root->right, x1, x2, y1, y2, depth+1);
			
			if( (double_gt(node_x, x1) && double_lt(node_x,x2) ) || 
		     	     double_equal(node_x, x1) || double_equal(node_x, x2) )
		     		
				return 1 + left_ans + right_ans;
			
			else return left_ans + right_ans;
		}
		
		if(double_lt(node_y, y1))
		{
			return searchTree(root->right, x1, x2, y1, y2, depth+1);
		}
		else if(double_gt(node_y, y2))
		{
			return searchTree(root->left, x1, x2, y1, y2, depth+1);
		}
	}
}

void preorder(struct node* root)
{
	if (!root) return;
	preorder(root->left);
	
	cout<< "X "<<root->value.first<<" Y "<<root->value.second<<endl;
	
	preorder(root->right); 
}

void inorder(struct node* root)
{
	if (!root) return;
	cout<< "X "<<root->value.first<<" Y "<<root->value.second<<endl;
	inorder(root->left);	
	inorder(root->right); 
}

int double_equal(double a, double b)
{
	if( ((a-b) < EPSILON && (a-b) >= 0) || ((b-a) < EPSILON && (b-a) >= 0) )
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
