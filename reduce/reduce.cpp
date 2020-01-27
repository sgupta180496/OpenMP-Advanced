#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <omp.h>

using namespace std;

#ifdef __cplusplus
extern "C" 
{
	#endif
  	void generateReduceData (int* arr, size_t n);
	#ifdef __cplusplus
}
#endif

int Sum(int i,int j,int N);
int cal_parallel(int i,int j);

//global variables
int n,nbthreads;
int * arr;

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
  
  	if (argc < 3) 
	{
    		std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    		return -1;
  	}

  	n=atoi(argv[1]);  
  	nbthreads=atoi(argv[2]);
  	arr=(int*)malloc(n*sizeof(int));
	// condition to check if the array is a valid one
	if(arr==NULL)
	{
		std::cout<<"Array allocation failed"<<std::endl;
		return 0;
	}
  	generateReduceData(arr,n);
  	int res=0;

	// set number of threads
  	omp_set_num_threads(nbthreads);

	// start calculating time
    	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  	//Recursively defining tasks
  	#pragma omp parallel
  	{
  		#pragma omp single
    		res=Sum(0,n-1,n);
  	}
	// stop calculating time
    	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    	std::chrono::duration<double> elapased_seconds = end-start;
    	//std::cerr.precision(10);
  	std::cerr<<elapased_seconds.count()<<std::endl;
  	std::cout<<res<<endl;
  	delete[] arr;
	return 0;
}

int Sum(int i,int j,int N) 
{ 
    	int mid,a,b,partial_sum;
    	if(j-i+1<=n/(2*nbthreads))
	{
      		partial_sum=cal_parallel(i,j);
      		return partial_sum;
    	}
	// calculate the mid value
    	mid=(i+j)/2;

	// using middle index of every sectioned array to divide and calculate the partial sums parallely and then the values are added at the end and returned
    	#pragma omp task shared(a)
    	a=Sum(i,mid,N);
    	#pragma omp task shared(b)
    	b=Sum(mid+1,j,N);
    	#pragma omp taskwait
    	return a+b;

} 

// in the loop we are parallely adding up different set of values to get partial sums
int cal_parallel(int i,int j)
{
  	int parallel_sum=0;
  	for(int x=i;x<=j;x++)
    		parallel_sum+=arr[x];

  	return parallel_sum;
}

