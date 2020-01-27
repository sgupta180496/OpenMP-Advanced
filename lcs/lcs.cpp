#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <chrono>
#include <string.h>

#ifdef __cplusplus
extern "C" 
{
	#endif
  	void generateLCS(char* X, int m, char* Y, int n);
  	void checkLCS(char* X, int m, char* Y, int n, int result);
	#ifdef __cplusplus
}
#endif

int max(int a,int b);
int lcs(char *X,char *Y,int m,int n,int nbthreads);

int main (int argc, char* argv[]) 
{
	//forces openmp to create the threads beforehand
	#pragma omp parallel
	{
		int fd = open (argv[0], O_RDONLY);
		if (fd != -1) 
		{
			close (fd);
		}
		else 
		{
			std::cerr<<"something is amiss"<<std::endl;
		}
	}

	if (argc < 4) 
	{ 
		std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
		return -1;
	}

	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	int nbthreads=atoi(argv[3]);

	// set number of threads
	omp_set_num_threads(nbthreads);

	char *X=new char[m];
	// condition to check if the array is a valid one
	if(X==NULL)
	{
		std::cout<<"Array allocation failed"<<std::endl;
		return 0;
	}

	char *Y=new char[n];
	// condition to check if the array is a valid one
	if(Y==NULL)
	{
		std::cout<<"Array allocation failed"<<std::endl;
		return 0;
	}
	int res=0;
	generateLCS(X,m,Y,n);

	// start calculating time
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	
	#pragma omp parallel
	#pragma omp single nowait
	res=lcs(X,Y,m,n,nbthreads);
	
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elpased_time = end-start;
	checkLCS(X,m,Y,n,res);
	std::cerr<<elpased_time.count()<<std::endl;

	delete[] X;
	delete[] Y;

	return 0;
}

// function used for comparing and returning the maximum value
int max(int x,int y) 
{ 
	if(x>y)
	{
		return x;
	}
	else
	{
		return y;
	}
} 

// function to perform lcs computation and return the result
int lcs(char *X,char *Y,int m,int n,int nbthreads) 
{ 
	int i,j;
	int granularity=500;
	if(n<=10)
	{
		granularity=50;
	}
	else 
	{
		granularity=5*n*0.01;
	}

	int** temp=new int*[m+1];
	for (int k=0;k<=m;++k) 
	{
		temp[k]=new int[n+1];
	}
	
	#pragma omp parallel for schedule(guided,granularity)
	for(i=0;i<=m;i++) 
	{ 
		for(j=0;j<=n;j++) 
		{ 
			if(i==0||j==0)
			{ 
				temp[i][j]=0; 
			}
			else if(X[i-1]==Y[j-1]) 
			{
				temp[i][j]=temp[i-1][j-1]+1; 
			}
			else
			{
				temp[i][j]=max(temp[i-1][j],temp[i][j-1]); 
			}
		} 
	} 

	int result=temp[m][n];
	
	#pragma omp taskwait
	for(int i=0;i<=m;++i) 
	{ 
		delete[] temp[i];
	}
	delete[] temp;
	return result; 
} 



