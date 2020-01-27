#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>

#ifdef __cplusplus
extern "C" 
{
	#endif
  	void generateMergeSortData (int* arr, size_t n);
  	void checkMergeSortResult (int* arr, size_t n);
	#ifdef __cplusplus

}
#endif

void merge(int * arr,int * temp,int start, int mid, int end);
void mergesort(int * arr,int * temp,int n,int start,int end);

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
	
	long n=atol(argv[1]);
	int nbthreads=atoi(argv[2]);
	int * arr=new int [n];
	// condition to check if the array is a valid one
	if(arr==NULL)
	{
		std::cout<<"Array allocation failed"<<std::endl;
		return 0;
	}

	int * temp=new int [n];
	// condition to check if the array is a valid one
	if(arr==NULL)
	{
		std::cout<<"Array allocation failed"<<std::endl;
		return 0;
	}
        //set number of threads
	omp_set_num_threads(nbthreads);
	
	generateMergeSortData (arr,n);

	//start calculating time
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	#pragma omp parallel
	{
		#pragma omp single
		{
			mergesort(arr,temp,n,0,n-1); 
		}
	}
	//stop calculating time 
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elpased_time = end-start;
	std::cerr<<elpased_time.count()<<std::endl;
	checkMergeSortResult(arr,n);

	delete[] arr;
	delete[] temp;
	return 0;
}

// comparing elements and merging them back using third variable to swap
void merge(int * arr,int * temp,int start,int mid,int end) 
{
	int i,j,k;
	int n=mid-start;
	if(start==end) 
		return;
	if(end-start==1) 
	{
		if(arr[start]>arr[end]) 
		{
			int x=arr[start];
			arr[start]=arr[end];
			arr[end]=x;
		}
		return;
	}
	
	for(i=0;i<n;++i)
	{
	    temp[start+i]=arr[start+i];
        }
	i=0;
	j=mid;
	k=start;

	while(i<n&&j<=end) 
	{
		if(temp[start+i]<=arr[j]) 
		{
			arr[k++]=temp[start+i];
			i++;
		} 
		else 
		{
			arr[k++]=arr[j++];
		}
	}
	while(i<n) 
	{
		arr[k++]=temp[start+i];
		i++;
	}
}

//partitioning array using recursive calls using tasking construct
void mergesort(int * arr,int * temp,int n,int start,int end) 
{
	long granularity=300;
	if(n<=300)  
		granularity=300;
	else if(n>=300 && n<1000)
		granularity=5*n*0.1;
	else if(n>=1000 && n<100000)
		granularity=5*n*0.01;
	else if(n==100000)
		granularity=5*n*0.01;
	else if(n==1000000)
		granularity=5*n*0.001;
	else if(n==10000000)
		granularity=5*n*0.0001;
	else
		granularity=5000;
	if(end-start>granularity) 
	{
		int mid=(start+end)/2;
		#pragma omp task
		mergesort(arr,temp,n,start,mid);
		#pragma omp task
		mergesort(arr,temp,n,mid+1,end);
		#pragma omp taskwait
		merge(arr,temp,start,mid+1,end);
	}
	else
	{
		if(start<end) 
		{
		    int mid=(start+end)/2;
		    mergesort(arr,temp,n,start,mid);
		    mergesort(arr,temp,n,mid+1,end);
		    merge(arr,temp,start,mid+1,end);
		}
	}
}


