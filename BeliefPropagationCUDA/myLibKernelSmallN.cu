#ifndef MYLIBKERNELSMALLN_CU
#define MYLIBKERNELSMALLN_CU

#include <limits.h>

#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

#include "myLibKernelSmallN.h"

#include "myLib.h"

bool debug = false;

void getNumBlocksAndThreadsSmallN(int n, int maxThreads, int &blocks, int &threads){
        //get device capability, to avoid block/grid size excceed the upbound
	    cudaDeviceProp prop;
	    int device;
		cudaGetDevice(&device);
		cudaGetDeviceProperties(&prop, device);
		
		threads = (n < maxThreads*2) ? nextPow2((n + 1)/ 2) : maxThreads;
        blocks = (n + (threads * 2 - 1)) / (threads * 2);
	    
	    if ((float)threads*blocks > (float)prop.maxGridSize[0] * prop.maxThreadsPerBlock)
	    {
	        printf("size is too large, please choose a smaller number!\n");
	    }
		
		if (blocks > prop.maxGridSize[0])
	    {
	        printf("Grid size <%d> excceeds the device capability <%d>, set block size as %d (original %d)\n",
	               blocks, prop.maxGridSize[0], threads*2, threads);
	        blocks /= 2;
	        threads *= 2;
	    }
}

/*
    This version uses n/2 threads --
    it performs the first level of reduction when reading from global memory.
	Nel primo livello occorre recuperare i dati da g_idata in base agli indici riportati in d_iIndexData
*/
__global__ void
reduce1StepSmallN(double *g_idata, double *g_odata, size_t *d_iIndexData, unsigned int n, unsigned int nArray, bool debug)//, unsigned int fraction)
{	
	// extern serve per rendere l'allocazione della memoria condivisa dinamica 
	// si potrebbe utilizzare quella statica se la dimensione fosse nota a compile time
	// nota: dalla 2.0 in avanti abbiamo 48 KB di shared memory
    extern __shared__ double sdata[];

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
	// legge blocchi di dimensione doppia
	// i scorre tutte le threads di tutti i blocchi pari: blocco0[thread0...threadn] blocco2[thread0...threadn]
//  orig:  unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	// in mySum ogni thread dovrebbe avere il valore dell'indice dell'array che deve analizzare 
	// nota: solo metà degli elementi è presa in considerazione, in quanto 0 <= threadIdx.x <= blockDim (non blockDim*2)
    
	//double mySum = (i < n) ? g_idata[d_iIndexData[i]] : 0;
	// ALE
	double mySum;
	if (i < n && d_iIndexData[i] != SIZE_MAX) { //ULONG_MAX) { //SIZE_MAX) {
		mySum = g_idata[d_iIndexData[i]];
	} else {
		mySum = 0;
	}
	//

	//cuPrintf ("CUPRINTF 1- blockIdx.x = %d mySum = %f \n",blockIdx.x, mySum);

	// nella prima passata ogni thread prende come mySum il valore preso all'istruzione precedente 
	// + il valore preso dal blocco successivo (viene così presa in considerazione l'altra metà degli elementi)
    //if (i + (blockDim.x*4) < n)
    //    mySum += g_idata[i+(blockDim.x*4)];
	
	//if (i + blockDim.x < n)
	//	mySum += g_idata[d_iIndexData[i+blockDim.x]];
	// ALE
    if (i + blockDim.x < n && d_iIndexData[i+blockDim.x] != SIZE_MAX) { //ULONG_MAX) { //SIZE_MAX) {
		mySum += g_idata[d_iIndexData[i+blockDim.x]];
	}

    sdata[tid] = mySum;
    __syncthreads();
	//if(tid < 6 || tid > 250) cuPrintf ("CUPRINTF 2- blockIdx.x = %d mySum = %f \n",blockIdx.x, mySum);

    // do reduction in shared mem
    for (unsigned int s=blockDim.x/2; s >= nArray; s>>=1)
    {
        if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }

        __syncthreads();
    }
	//if((tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 3- sdata[%d] = %f \n", tid, sdata[tid]);

    // write result for this block to global mem
    //if (tid == 0) g_odata[blockIdx.x] = sdata[0];
	if(tid < nArray){
		g_odata[blockIdx.x*nArray + tid] = sdata[tid]; // orig g_odata[blockIdx.x] = sdata[tid];
		if(debug && (tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, tid, g_odata[tid]);
		}
}

/*
    This version uses n/2 threads --
    it performs the first level of reduction when reading from global memory.
*/
__global__ void
reduce2StepSmallN(double *g_idata, double *g_odata, unsigned int n, unsigned int nArray, bool debug)//, unsigned int fraction)
{	
	// extern serve per rendere l'allocazione della memoria condivisa dinamica 
	// si potrebbe utilizzare quella statica se la dimensione fosse nota a compile time
	// nota: dalla 2.0 in avanti abbiamo 48 KB di shared memory
    extern __shared__ double sdata[];

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
	// legge blocchi di dimensione doppia
	// i scorre tutte le threads di tutti i blocchi pari: blocco0[thread0...threadn] blocco2[thread0...threadn]
//  orig:  unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	// in mySum ogni thread dovrebbe avere il valore dell'indice dell'array che deve analizzare 
	// nota: solo metà degli elementi è presa in considerazione, in quanto 0 <= threadIdx.x <= blockDim (non blockDim*2)
    double mySum = (i < n) ? g_idata[i] : 0;
	//cuPrintf ("CUPRINTF 1- blockIdx.x = %d mySum = %f \n",blockIdx.x, mySum);

	// nella prima passata ogni thread prende come mySum il valore preso all'istruzione precedente 
	// + il valore preso dal blocco successivo (viene così presa in considerazione l'altra metà degli elementi)
    //if (i + (blockDim.x*4) < n)
    //    mySum += g_idata[i+(blockDim.x*4)];
    if (i + blockDim.x < n)
        mySum += g_idata[i+blockDim.x];

    sdata[tid] = mySum;
    __syncthreads();
	//if(tid < 6 || tid > 250) cuPrintf ("CUPRINTF 2- blockIdx.x = %d mySum = %f \n",blockIdx.x, mySum);

    // do reduction in shared mem
    for (unsigned int s=blockDim.x/2; s >= nArray; s>>=1)
    {
        if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }

        __syncthreads();
    }
	//if((tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 3- sdata[%d] = %f \n", tid, sdata[tid]);

    // write result for this block to global mem
    //if (tid == 0) g_odata[blockIdx.x] = sdata[0];
	if(tid < nArray){
		g_odata[blockIdx.x*nArray + tid] = sdata[tid]; // orig g_odata[blockIdx.x] = sdata[tid];
		if(debug && (tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, tid, g_odata[tid]);
		}
}

double *reduceSmallNArray(int  n,
				  	int nArray,
                 	int  numThreads,
                  	int  numBlocks,
                  	int  maxThreads,
                  	int  cpuFinalThreshold,
                  	double *h_odata,
                  	double *d_idata,
                  	double *d_odata,
					size_t *d_iIndexData) //, int nArrayVero)
{
	bool needReadBack = true;
	
    cudaDeviceSynchronize();
    	// (equivalente) a dim3 dimBlock = dim3(numThreads, 1, 1);
		// le threads avranno quindi un'unica dimensione
    	dim3 dimBlock(numThreads, 1, 1);
    	// (equivalente) a dim3 dimGrid = dim3(numBlocks, 1, 1);
		// griglia di un'unica dimensione
	    dim3 dimGrid(numBlocks, 1, 1);
		// when there is only one warp per block, we need to allocate two warps
    	// worth of shared memory so that we don't index shared memory out of bounds
    	int smemSize = (numThreads <= 32) ? 2 * numThreads * sizeof(double) : numThreads * sizeof(double);

		cudaError_t error;
		
		unsigned int dimResult = nArray * sizeof(double);
		//unsigned int dimResult = nArrayVero * sizeof(double);
		double *gpu_result = (double *) malloc(dimResult);
        for (int i=0; i < nArray; i++) gpu_result[i]=0;	
		//for (int i=0; i < nArrayVero; i++) gpu_result[i]=0;

			if(debug) { assert (nArray%dimBlock.x == 0); cudaPrintfInit();
					// execute the kernel
					printf("\n1 esecuzione:\n %d dimGrid\n %d dimBlock\n %d smemSize\n",dimGrid.x, dimBlock.x, smemSize ); }
			reduce1StepSmallN<<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, debug);//, fraction);
			if(debug) { cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); }
        
        if (true)
        {
	        int s=numBlocks;

            // sum partial sums from each block on CPU
            // copy result from device to host
              cudaMemcpy(h_odata, d_odata, s * sizeof(double) * nArray, cudaMemcpyDeviceToHost);
			  error = cudaGetLastError();
			  if(error != cudaSuccess)
			  {
			    // print the CUDA error message and exit
			    printf("\nCUDA error1: %s\n", cudaGetErrorString(error));
			    exit(-1);
			  }	

			double prec;
              for (int i=0; i < nArray; i++) 
			//for (int i=0; i < nArrayVero; i++) 
			{
					for(int j=0; j < s; j++) {
						gpu_result[i] += h_odata[i*s+j];
					}
					prec=gpu_result[0];
					if(debug && (prec != gpu_result[i])) {
						printf("\n CPU ERRORE:  gpu_result[%d-1]=%f != gpu_result[%d]=%f",i,prec,i, gpu_result[i]); 
						printf("\n CPU          gpu_result[%d]=%f     s = %d, i = %d", i, gpu_result[i], s, i);
					}
					prec=gpu_result[i];
              }
			if(debug) { 	printf("\n CPU gpu_result[0]=%f     s = %d", gpu_result[0], s); }

            needReadBack = false;
        }
        else
        {
            // sum partial block sums on GPU
            int s=numBlocks;

            while (s > cpuFinalThreshold)
            {
                int threads = 0, blocks = 0;
				int nElements = s*nArray;

				// le righe seguenti si potrebbero accorpare in una funzione
                getNumBlocksAndThreadsSmallN(nElements, maxThreads, blocks, threads);	//forse è 2*s //orig:s
				dim3 dimBlock(threads, 1, 1);
				dim3 dimGrid(blocks, 1, 1);
				// when there is only one warp per block, we need to allocate two warps
			    // worth of shared memory so that we don't index shared memory out of bounds
				smemSize = (threads <= 32) ? 2 * threads * sizeof(double) : threads * sizeof(double);

				// do in input al reduce da rielaborare gli stessi dati
				if(debug) { cudaPrintfInit();
					printf("\n\t iter esecuzione:\n %d dimGrid\n %d dimBlock\n %d smemSize\n %d s\n %d n\n",dimGrid.x, dimBlock.x, smemSize, s, n);
					printf("nElements: %d \n",nElements); }
				reduce2StepSmallN<<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray, debug);	
														// con i numeri piccoli funziona con nElements=n*nArray
				if(debug) { cudaPrintfDisplay (stdout, true);   cudaPrintfEnd (); }
				s = blocks; // orig: s = (s + (threads*2-1)) / (threads*2);
            }
			if(debug) { printf("\n passato il while... s = %d",s); }
            if (s > 1)
            {
                // copy result from device to host
				if(debug) { printf("\n s = %d, nArray = %d",s,nArray);}
                cudaMemcpy(h_odata, d_odata, s * sizeof(double) * nArray, cudaMemcpyDeviceToHost);
			   error = cudaGetLastError();
			  if(error != cudaSuccess)
			  {
			    // print the CUDA error message and exit
			    printf("\nCUDA error1: %s\n", cudaGetErrorString(error));
			    exit(-1);
			  }	

				double prec;
                for (int i=0; i < nArray; i++) 
				//for (int i=0; i < nArrayVero; i++)
                {
					for(int j=0; j < s; j++) {
						gpu_result[i] += h_odata[i*s+j];
						//printf("\n FINAL h_odata[%d*%d+%d]=%f", i,s,j, h_odata[i*s+j]); 
					}
					prec=gpu_result[0];
					if(debug && (prec != gpu_result[i])) {
						printf("\n GPU ERRORE:  gpu_result[%d-1]=%f != gpu_result[%d]=%f",i,prec,i, gpu_result[i]); 
						printf("\n GPU          gpu_result[%d]=%f     s = %d, i = %d", i, gpu_result[i], s, i);
					}
					prec=gpu_result[i];
                }
				if(debug) { printf("\n GPU FINAL gpu_result[0]=%f     s = %d", gpu_result[0], s); }
                needReadBack = false;
           }
}
		if (needReadBack)
	    {
	        // copy final sum from device to host
          cudaMemcpy(gpu_result, d_odata, sizeof(double)*nArray, cudaMemcpyDeviceToHost);
			//cudaMemcpy(gpu_result, d_odata, sizeof(double)*nArrayVero, cudaMemcpyDeviceToHost);
		  cudaError_t error = cudaGetLastError();
		  if(error != cudaSuccess)
		  {
		    // print the CUDA error message and exit
		    printf("\nCUDA error2: %s\n", cudaGetErrorString(error));
		    exit(-1);
		  }	
	    }
	return gpu_result;   
}

double * runSmallN(int size, int nArray, double *h_idata, size_t *h_iIndexData) { //, int nArrayVERO){
	//int size = 1<<24;    // number of elements to reduce -> default:  16777216						// ALE
	//int nArray = 1024;	// m = 16384 																// ALE
	//size = 4096;																					// ALE
	//nArray = 1;																						// ALE
    int maxThreads = 256;  // number of threads per block
    int cpuFinalThreshold = 1;
    printf("%d elements\n", size);
    printf("%d nArray\n", nArray);
    printf("%d maxThreads\n", maxThreads);

        // create input
        unsigned int bytesValuesInput = size * sizeof(double);										// ALE
        //double *h_idata = (double *) malloc(bytesValuesInput);										// ALE
		unsigned int bytesIndexInput = size * sizeof(double);										// ALE
		//int *h_iIndexData = (int *) malloc(bytesIndexInput);										// ALE
		
		//for (int i=0; i<size; i++) { h_idata[i] = 1; h_iIndexData[i]=i; }							// ALE
		
        int numBlocks = 0;
        int numThreads = 0;
		getNumBlocksAndThreadsSmallN(size, maxThreads, numBlocks, numThreads);
        
	    if (numBlocks == 1) cpuFinalThreshold = 1;
        
        // allocate mem for the result on host side
		unsigned int bytesOutput = sizeof(double)*nArray*numBlocks;
        double *h_odata = (double *) malloc(bytesOutput);
        printf("%d blocks\n\n", numBlocks);
        
		// allocate device memory and data
        double *d_idata = NULL;
		size_t *d_iIndexData = NULL;
        double *d_odata = NULL;
        cudaMalloc((void **) &d_idata, bytesValuesInput);
        cudaMalloc((void **) &d_iIndexData, bytesIndexInput);
        cudaMalloc((void **) &d_odata, bytesOutput);

        // copy data directly to device memory
        cudaMemcpy(d_idata, h_idata, bytesValuesInput, cudaMemcpyHostToDevice);
        cudaMemcpy(d_iIndexData, h_iIndexData, bytesIndexInput, cudaMemcpyHostToDevice);
	    printf("\nbytesOutput=%d\n",bytesOutput);
        cudaMemcpy(d_odata, h_idata, bytesOutput, cudaMemcpyHostToDevice);

		// per i test si può inserire un warm-up

		cudaEvent_t start, stop;
		float time;	// GPU TIME
		cudaEventCreate(&start); // vedi http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/
		cudaEventCreate(&stop);	
		cudaEventRecord( start, 0 );
		
        double *gpu_result;																		// ALE è lungo nArray

        gpu_result = reduceSmallNArray(size, nArray, numThreads, numBlocks, maxThreads,
			cpuFinalThreshold, h_odata, d_idata, d_odata, d_iIndexData); //, nArrayVERO);

		cudaEventRecord( stop, 0 );
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &time, start, stop );
		printf ("\nTime for the kernel: %f ms\n", time);											// ALE 
		double prec=gpu_result[0];
		for(int i=0; i < nArray ; i++)	{
			if(prec != gpu_result[i]) printf("\nERRORE:  gpu_result[%d-1]=%f != gpu_result[%d]=%f",i,prec,i, gpu_result[i]);
			prec=gpu_result[i];
		//    printf("\nGPU result = %f\n", gpu_result[i]);
		}
        printf("\nGPU result       \t= \t %f\n", prec);
		assert ((size % nArray) == 0);
		printf("risultato atteso \t= \t %d\n", size / nArray);
	    
        //free(h_idata);
        //free(h_odata);
		cudaFree(d_idata);
        cudaFree(d_odata);
		cudaEventDestroy( start );
		cudaEventDestroy( stop );
		
		return gpu_result;
}

void selectGPU()
{
	cudaDeviceProp deviceProp;
    deviceProp.major = 1;

		// da fare nel caso dei double (perchè necessitano di maggiori risorse), altrimenti sarebbero state settate a 1 e 10
	    deviceProp.minor = 3;
	    int minimumComputeVersion = 13;

	    int dev = findCudaDevice();

	    cudaGetDeviceProperties(&deviceProp, dev);

	    if ((deviceProp.major * 10 + deviceProp.minor) >= minimumComputeVersion)
	    {
	        printf("Using Device %d: %s\n\n", dev, deviceProp.name);
	        cudaSetDevice(dev);
	    }
	    else
	    {
	        printf("Error: the selected device does not support the minimum compute capability of %d.%d.\n\n",
	               minimumComputeVersion / 10, minimumComputeVersion % 10);

	        cudaDeviceReset();
	        exit(EXIT_FAILURE);
	    }
}

void resetGPU()
{
	cudaDeviceReset();
}

#endif
