#ifndef MYLIBKERNELBIGN_CU
#define MYLIBKERNELBIGN_CU

#include <limits.h>

#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

#include "myLibKernelBigN.h"

#include "myLib.h"

const bool debug = false;

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

void getNumBlocksAndThreadsBigN(size_t n, int maxThreads, int &blocks, int &threads){
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

__global__ void
reduce1StepBigN(double *g_idata, double *g_odata, size_t *d_iIndexData, size_t n, size_t halfN, size_t nArray, bool debug)//, unsigned int fraction)
{	
	// i scorre la prima metà dei dati in input
	unsigned int i = blockIdx.x*blockDim.x + threadIdx.x;
	//unsigned const int halfN = n>>1;

	// somma elementi della prima metà con elementi della seconda metà
	int index1 = d_iIndexData[i+halfN];
	int index2 = d_iIndexData[i];
		
    if (i < halfN){ // sta sempre dentro all'if
		//g_odata[i] = g_idata[d_iIndexData[i]] + g_idata[d_iIndexData[i+halfN]];
		double mySum = (index1 != SIZE_MAX) ? g_idata[index1] : 0;
		g_odata[i] = (index2 != SIZE_MAX) ? g_idata[index2] + mySum : mySum;
			if(debug && (threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) 
				cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, threadIdx.x, g_odata[threadIdx.x]);
	}
}

__global__ void														// non so se passarlo come parametro o farlo calcolare alle theads... TESTARE
reduce2StepBigN(double *g_idata, double *g_odata, size_t n, size_t halfN, size_t nArray, bool debug)//, unsigned int fraction)
{	
	// i scorre la prima metà dei dati in input
	unsigned const int i = blockIdx.x*blockDim.x + threadIdx.x;
	//unsigned const int halfN = n>>1;
    //double mySum = (i < half) ? g_idata[i] : 0;
	// somma elementi della prima metà con elementi della seconda metà
    if (i < halfN){ // sta sempre dentro all'if
        //mySum += g_idata[i+blockDim.x];
		//double mySum = g_idata[i] + g_idata[i+blockDim.x];
		g_odata[i] = g_idata[i] + g_idata[i+halfN];
			if(debug && (threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) 
				cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, threadIdx.x, g_odata[threadIdx.x]);
	}
}



double *reduceBigNArray(size_t  n,
				  	size_t nArray,
                 	int  numThreads,
                  	int  numBlocks,
                  	int  maxThreads,
                  	//int  cpuFinalThreshold,
                  	double *h_odata, // si può togliere ??
                  	double *d_idata,
                  	double *d_odata,
					size_t *d_iIndexData,
					size_t dimRisultato)
{
	bool needReadBack = true;
	
    cudaDeviceSynchronize();
    	// (equivalente) a dim3 dimBlock = dim3(numThreads, 1, 1);
		// le threads avranno quindi un'unica dimensione
    	dim3 dimBlock(numThreads, 1, 1);
    	// (equivalente) a dim3 dimGrid = dim3(numBlocks, 1, 1);
		// griglia di un'unica dimensione
	    dim3 dimGrid(numBlocks, 1, 1);

		cudaError_t error;
		
		//size_t dimResult = nArray * sizeof(double);
		size_t dimResult = dimRisultato * sizeof(double);

		double *gpu_result = (double *) malloc(dimResult);
		// ALE_DOMANDA
		// SERVE??? non viene sovrascritto poi dalla cudaMemCpy???
		for (int i=0; i < dimRisultato; i++) gpu_result[i]=0;		
		//

		if(debug) { 
			cudaPrintfInit();
				// execute the kernel
				printf("\nSTART esecuzione:\n %d dimGrid\n %d dimBlock\n",dimGrid.x, dimBlock.x ); }
		reduce1StepBigN<<< dimGrid, dimBlock>>>(d_idata, d_odata, d_iIndexData, n, n>>1, nArray, debug);
		if(debug) { cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); }
		/*
		unsigned int dimResultCHECK = (n>>1) * sizeof(double);
		double *gpu_resultCHECK = (double *) malloc(dimResultCHECK);
		cudaMemcpy(gpu_resultCHECK, d_odata, sizeof(double)*(n>>1), cudaMemcpyDeviceToHost);
		  error = cudaGetLastError();
		  if(error != cudaSuccess)
		  {
		    // print the CUDA error message and exit
		    printf("\nCUDA error0: %s\n", cudaGetErrorString(error));
		    exit(-1);
		  }	
		double prec=gpu_resultCHECK[0];
		for(int i=0; i < nArray ; i++)	{
			if(prec != gpu_resultCHECK[i]) printf("\n---0 ERRORE:  gpu_resultCHECK[%d-1]=%f != gpu_resultCHECK[%d]=%f",i,prec,i, gpu_resultCHECK[i]);
			prec=gpu_resultCHECK[i];
		//    printf("\nGPU result = %f\n", gpu_result[i]);
		}
		int j=0;
		
        printf("\nSTART %d GPU result       \t= \t %f\n", j, prec);
        */


		for(size_t nElements = n>>1; nElements > nArray; nElements>>=1){
			//sleep(1);
			
			int threads = 0, blocks = 0;
            

			getNumBlocksAndThreadsBigN(nElements, maxThreads, blocks, threads);
			
			dim3 dimBlock(threads, 1, 1);
			dim3 dimGrid(blocks, 1, 1);
			
			if(debug) printf("\n%d elementi:\n %d dimGrid\n %d dimBlock\n",nElements,dimGrid.x, dimBlock.x ); 
	
			if(debug) { 
				cudaPrintfInit();
					// execute the kernel
					printf("\n%d esecuzione:\n %d dimGrid\n %d dimBlock\n",nElements,dimGrid.x, dimBlock.x ); }
			reduce2StepBigN<<< dimGrid, dimBlock>>>(d_odata, d_odata, nElements, nElements>>1, nArray, debug);
			if(debug) { cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); }
			
        }
        
		if (needReadBack)
	    {
	        // copy final sum from device to host
          //cudaMemcpy(gpu_result, d_odata, sizeof(double)*nArray, cudaMemcpyDeviceToHost);
			cudaMemcpy(gpu_result, d_odata, dimResult, cudaMemcpyDeviceToHost);
		  error = cudaGetLastError();
		  if(error != cudaSuccess)
		  {
		    // print the CUDA error message and exit
		    printf("\nCUDA error2: %s\n", cudaGetErrorString(error));
		    exit(-1);
		  }	
	    }
	return gpu_result;   
}

double* runBigN(size_t size, size_t nArray, double *h_idata, size_t *h_iIndexData, size_t dimInput, size_t dimRisultato){
	// ALE_GOT
	//size = 1048576;
	//nArray = 16384;
	//

	//int size = 1<<24; // 1<<24;    // number of elements to reduce -> default:  16777216				// ALE
	//int nArray = 1<<4;	// m = 16384 
	//int m = 1<<20; // 4 7 10 14 17 20 
	size_t m = size / nArray;																					// ALE
	//size = 1<<16;																						// ALE
	//nArray = 1<<20;																					// ALE
    int maxThreads = 256;				// number of threads per block
    int cpuFinalThreshold = 1;
    /*  STAMPE */
	printf("%d elements\n", size);
    printf("%d nArray\n", nArray);
	printf("%d nArrayInput\n", dimInput);
	printf("%d nArrayRisultato\n", dimRisultato);
    printf("%d m\n", m);
    printf("%d maxThreads\n", maxThreads);
	/* */

	double *gpu_result;	

	if (m == 1) {
		gpu_result = (double *) malloc(dimRisultato * sizeof(double));
		for (size_t i = 0; i < dimInput; i++)
			gpu_result[i] = h_idata[i];
		return gpu_result;
	}
	
	float total = 0.0;

	//for(int i =0; i<100; i++){

        // create input
	size_t bytesValuesInput = dimInput * sizeof(double);										// ALE
        // ALE_GOT
		//double *
		//	h_idata = (double *) malloc(bytesValuesInput);										// ALE
		//
		size_t bytesIndexInput = size * sizeof(size_t);										// ALE
		// ALE_GOT
		//size_t *
		//	h_iIndexData = (size_t *) malloc(bytesIndexInput);										// ALE
		//
		// ALE_GOT
		//for (size_t i=0; i<size; i++) { h_idata[i] = 1; h_iIndexData[i]=i; }							// ALE
		//

       	int numBlocks = 0;
        int numThreads = 0;
		getNumBlocksAndThreadsBigN(size, maxThreads, numBlocks, numThreads);
       
	    if (numBlocks == 1) cpuFinalThreshold = 1;
       
        // allocate mem for the result on host side
		size_t bytesOutput = sizeof(double)*(size/2);											// ALE
        double *h_odata = (double *) malloc(bytesOutput);
        if(debug) printf("%d blocks\n", numBlocks);
       
		// allocate device memory and data
        double *d_idata = NULL;
		size_t *d_iIndexData = NULL;
        double *d_odata = NULL;
	
	

        cudaMalloc((void **) &d_idata, bytesValuesInput);
        cudaMalloc((void **) &d_iIndexData, bytesIndexInput);
        cudaMalloc((void **) &d_odata, bytesOutput);

        // copy data directly to device memory
        cudaMemcpy(d_idata, h_idata, bytesValuesInput, cudaMemcpyHostToDevice);

		// ALE_GOT
		/*
		for (size_t l = 0; l < dimInput; l++)
				printf("%f ", h_idata[l]);
			printf("\n");
			for (size_t l = 0; l < size; l++)
				printf("%d ", h_iIndexData[l]);
		*/
		//	getchar();
		
		//


        cudaMemcpy(d_iIndexData, h_iIndexData, bytesIndexInput, cudaMemcpyHostToDevice);
	    //printf("\nbytesOutput=%d\n",bytesOutput);
        //cudaMemcpy(d_odata, h_idata, bytesOutput, cudaMemcpyHostToDevice);			//NON SERVE!!!

		// per i test si può inserire un warm-up
			cudaEvent_t start, stop;
			float time;	// GPU TIME
			cudaEventCreate(&start); // vedi http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/
			cudaEventCreate(&stop);	
		
		
			cudaEventRecord( start, 0 );
		
	        //double *gpu_result;																		// ALE è lungo nArray

	        gpu_result = reduceBigNArray(size, nArray, numThreads, numBlocks, maxThreads,
										h_odata, d_idata, d_odata, d_iIndexData, dimRisultato);


			cudaEventRecord( stop, 0 );
			cudaEventSynchronize( stop );
			cudaEventElapsedTime( &time, start, stop );
			total+=time;										// ALE 
			//printf ("\nTime for the kernel: %f ms\n", time);											// ALE 
			double prec=gpu_result[0];
			/*for(int i=0; i < nArray ; i++)	{
				if(prec != gpu_result[i]) {printf("\nERRORE:  gpu_result[%d-1]=%f != gpu_result[%d]=%f",i,prec,i, gpu_result[i]); exit(-1);}
				prec=gpu_result[i];
			//    printf("\nGPU result = %f\n", gpu_result[i]);
			}*/
//			assert (m==prec);
			cudaEventDestroy( start );													// ALE
			cudaEventDestroy( stop );													//ALE
			
			
	        //printf("\nGPU result       \t= \t %f\n", prec);
			assert ((size % nArray) == 0);
			//printf("risultato atteso \t= \t %d\n", size / nArray);


			//free(gpu_result);	    
			//free(h_iIndexData);    
	        //free(h_idata);													//ALE
	        //free(h_odata);													//ALE
			cudaFree(d_idata);													//ALE
	        cudaFree(d_odata);													//ALE
			cudaFree(d_iIndexData);
		//} 
		printf("average time: %f\n", (total/100));

		// ALE_GOT
		return gpu_result;
		//


		
}



///////////////////
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
///////////////////

#endif