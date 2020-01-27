#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <chrono>
using namespace std;
#ifdef __cplusplus
extern "C" 
{
	#endif
  	void generateMergeSortData (int* arr, size_t n);
  	void checkMergeSortResult (const int* arr, size_t n);
	#ifdef __cplusplus
}
#endif
 
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

  	int n= atoi(argv[1]);
  	int nbthreads = atoi(argv[2]);
  	int * arr = new int [n];
	// condition to check if the array is a valid one
	if(arr==NULL)
	{
		std::cout<<"Array allocation failed"<<std::endl;
		return 0;
	}
  	generateMergeSortData (arr, n);
	// set number of threads
  	omp_set_num_threads(nbthreads);
	// start calculating time
  	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	// in this for loop we will compare values and swap using a temp variable
	for(int i=0;i<n;i++)
   	{
		// using odd-even sorting technique
     		if(i%2==0)
		{
       			#pragma omp parallel for
       			for(int j=0;j<=n-2;j+=2)
       			{
         			if(arr[j]>arr[j+1])
				{
           				int temp=arr[j];
           				arr[j]=arr[j+1];
           				arr[j+1]=temp;
        			}
      			}
       
     		}
     		else
		{
       			#pragma omp parallel for
       			for(int j=1;j<=n-2;j+=2)
       			{
        			if(arr[j]>arr[j+1])
				{
           				int temp = arr[j];
           				arr[j] = arr[j+1];
           				arr[j+1] = temp;
        			}              
      			}
    		}
  
	}
	// stop calculating time
  	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  	std::chrono::duration<double> elapsed_seconds = end-start;
  	std::cerr<<elapsed_seconds.count()<<endl; 
  	checkMergeSortResult (arr, n);
  	delete[] arr;
	return 0;
}
