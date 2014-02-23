#ifndef MYLIBKERNELBIGN_CU
#define MYLIBKERNELBIGN_CU

#include <limits.h>

#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

#include "myLibKernelBigN.h"

#include "myLib.h"

#include "cublas_v2.h"
#include "windows.h"

const bool debug = false;

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

//#ifndef zeroALE
//#define zeroALE 0.0
//#endif

//////////////////////////////////////////// RIDUZIONE ==> MARGINALIZATION //////////////////////////////////////////////////////

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



void reduceBigNArray(size_t  n,
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

		// ALE 2014-02-23
		/*
		double *gpu_result = (double *) malloc(dimResult);
		// ALE_DOMANDA
		// SERVE??? non viene sovrascritto poi dalla cudaMemCpy???
		for (int i=0; i < dimRisultato; i++) gpu_result[i]=0;		
		//
		*/
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
			
			// ALE CUDA MEM CPY lo faccio dopo visto che lo unisco allo scattering
			/*cudaMemcpy(gpu_result, d_odata, dimResult, cudaMemcpyDeviceToHost);
		  error = cudaGetLastError();
		  if(error != cudaSuccess)
		  {
		    // print the CUDA error message and exit
		    printf("\nCUDA error2: %s\n", cudaGetErrorString(error));
		    exit(-1);
		  }	*/
	    }
		// ALE 2014-02-23
	/*
	return gpu_result;  
	*/

}

//                   dimTabCricca POW2, dimTabSep POW2, tabCricca,     tabIndiciCricca,   dimensione vera tabCricca, dim vera tabSep
double* marginalizationBigN(size_t size, size_t nArray, double *h_idata, size_t *h_iIndexData, size_t dimInput, size_t dimRisultato){
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
    /*  STAMPE * /
	printf("%d elements\n", size);
    printf("%d nArray\n", nArray);
	printf("%d nArrayInput\n", dimInput);
	printf("%d nArrayRisultato\n", dimRisultato);
    printf("%d m\n", m);
    printf("%d maxThreads\n", maxThreads);
	/ * */

	//double *gpu_result;	

	
	if (m == 1) {
		// ALE 2014-02-23
		/*
		gpu_result = (double *) malloc(dimRisultato * sizeof(double));
		for (size_t i = 0; i < dimInput; i++)
			gpu_result[i] = h_idata[i];
		return gpu_result;
		*/
		double *d_odata = NULL;
		size_t bytesValuesInput = sizeof(double)*dimInput;
		cudaMalloc((void **) &d_odata, bytesValuesInput);
		// copy data directly to device memory
        cudaMemcpy(d_odata, h_idata, bytesValuesInput, cudaMemcpyHostToDevice);
		return d_odata;
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
			
			//ALE 2014-02-23
			/*
	        gpu_result = 
			*/
			reduceBigNArray(size, nArray, numThreads, numBlocks, maxThreads,
										h_odata, d_idata, d_odata, d_iIndexData, dimRisultato);


			cudaEventRecord( stop, 0 );
			cudaEventSynchronize( stop );
			cudaEventElapsedTime( &time, start, stop );
			total+=time;										// ALE 
			//printf ("\nTime for the kernel: %f ms\n", time);											// ALE 
			//double prec=gpu_result[0];
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
	        //cudaFree(d_odata);													//ALE
			cudaFree(d_iIndexData);
		//} 
		/*
		printf("average time: %f\n", (total/100));
		*/
		// ALE_GOT
		//return gpu_result;
			return d_odata;
		//


		
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////// MOLTIPLICAZIONI ==> SCATTERING /////////////////////////////////////////////////////

void getNumBlocksAndThreadsDivMult(size_t n, int maxThreads, int &blocks, int &threads){
        //get device capability, to avoid block/grid size excceed the upbound
	    cudaDeviceProp prop;
	    int device;
		cudaGetDevice(&device);
		cudaGetDeviceProperties(&prop, device);
		
		threads = (n < maxThreads) ? nextPow2(n) : maxThreads;
		blocks = n / threads; //(n + (threads * 2 - 1)) / (threads * 2);
	    
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



// (possibile ottimizzazione: far si che ogni blocco faccia le operazioni sul doppio dei dati
// per avere la coalescenza andrebbe fatto non a livello di thread (th0 legge dato0 e dato1)
// ma th0 legge dato 0 e dato 0+dimBlocco
__global__ void
kernelDivVector(double *g_iVector1Data, double *g_iVector2Data, size_t n, const bool debug)
{	
//	if(debug && (threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 1- \n");
	size_t i = blockIdx.x*(blockDim.x) + threadIdx.x;
	if (i < n)
		// ALE
		g_iVector1Data[i] = (g_iVector2Data[i] > 0.0) ? g_iVector1Data[i] / g_iVector2Data[i] : 0; // zeroALE = 0.0
}

/*void getNumBlocksAndThreadsMultMatrixVector(int n, int maxThreads, int &blocks, int &threads){
        //get device capability, to avoid block/grid size excceed the upbound
	    cudaDeviceProp prop;
	    int device; 
		cudaGetDevice(&device);
		cudaGetDeviceProperties(&prop, device);
		
		threads = (n < maxThreads) ? nextPow2(n) : maxThreads;
		blocks = n/threads; //(n + (threads * 2 - 1)) / (threads * 2);
	    
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
}*/

	__global__ void
kernelMultMatrixVector(double *d_MatrixData, size_t *d_MatrixIndex, double * g_iVector1Data, size_t n, size_t size, const bool debug){
	size_t i = blockIdx.x*(blockDim.x) + threadIdx.x;
	size_t index = d_MatrixIndex[i];
	if (index != SIZE_MAX)
		d_MatrixData[index] *= g_iVector1Data[i % n];
	
	//if(debug && (threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 3- d_MatrixData[%d] = %f \n", index, d_MatrixData[index]);
}

//           dimTabCricca POW2, dimTabSep POW2, tabCricca, tabIndiciCricca,        dimensione vera tabCricca, dim vera tabSep
void scattering(size_t size,  // dimTabCricca POW2
			size_t n, // dimTabSep POW2
			double *d_iVector1Data, // double *h_iVector2Data qui ci va il device_v1data
			double *h_iVector2Data, // FI
			double *h_MatrixData, // PSI da aggiornare
			size_t *h_MatrixIndex, // Indexig of PSI da aggiornare

			size_t dimCricca, // dimensione vera tabCricca
			size_t dimSeparatore // dim vera tabSep
			) {
	// TESTA CON NVPROF
	//unsigned int n = 1<<12;//19;	
	//unsigned int m = 1<<12;//5;
	//unsigned int size = m*n;

	size_t m = size / n;

	//int maxThreadsDiv = 512;				/// ATTENZIONE, QUANDO SI FANNO I TEST BISOGNA VARIARE ENTRAMBI I VALORI!!!
	/*
	printf("%d n\n", n);
	printf("%d m\n", m);
	printf("%d size\n", size);
	printf("%d dimSep\n", dimSeparatore);
	printf("%d dimCricca\n", dimCricca);
	*/
	cudaError_t error;

	float bestAverageTimeMult = 100.0;
	float bestAverageTimeDiv = 100.0;
	unsigned int bestMaxThreadsDiv = 0;
	unsigned int bestMaxThreadsMult = 0;

	
	//for (int maxThreads = 32; maxThreads < 1024; maxThreads*=2) {
	// ALE
	int maxThreads = 256;

	unsigned int maxThreadsDiv = maxThreads;
	unsigned int maxThreadsMult = maxThreads;

	float totalDiv = 0.0;
	float totalMult = 0.0;
		
	//for(int i =0; i<100; i++){
	
		float timeDiv=0;																						// GPU TIME
		// create input
		// BISOGNA TOGLIERE GLI INDICI DALL'ARRAY, (METTERE L'ARRAY NELLA CONSTANT MEMORY)...
		size_t bytesVector = n * sizeof(double);										// ALE
		// FI* (il risultato della divisione lo salva nel g_ di questo)
		// passo direttamente il g_
		//double *h_iVector1Data = (double *) malloc(bytesVector);									// ALE
		// FI
		//double *h_iVector2Data = (double *) malloc(bytesVector);									// ALE

		size_t bytesDataMatrix = dimCricca * sizeof(double);										// ALE
		//double *h_MatrixData = (double *) malloc(bytesDataMatrix);									// ALE
		size_t bytesIndexMatrix = size * sizeof(size_t);									// ALE
		//unsigned int *h_MatrixIndex = (unsigned int *) malloc(bytesIndexMatrix);					// ALE


		/*
		for (unsigned int i=0; i<n; i++) { h_iVector1Data[i] = 10; h_iVector2Data[i] = 5; } 	// risultato atteso divisione: 2	// ALE
		for (unsigned int i=0; i<size; i++) { 
			h_MatrixIndex[i] = i; 
			h_MatrixData[i] = 2; 
		} // risultato atteso moltiplicazione: 4			// ALE
		*/

		int numBlocks = 0;
		int numThreads = 0;
		getNumBlocksAndThreadsDivMult(n, maxThreadsDiv, numBlocks, numThreads);

		assert(n%numThreads==0);

		// allocate device memory and data
		//double *d_iVector1DataP = NULL;
		double *d_iVector2Data = NULL;
		//cudaMalloc((void **) &d_iVector1DataP, bytesVector);
		cudaMalloc((void **) &d_iVector2Data, bytesVector);

		// copy data directly to device memory
		//cudaMemcpy(d_iVector1Data, h_iVector1Data, bytesVector, cudaMemcpyHostToDevice);
		cudaMemcpy(d_iVector2Data, h_iVector2Data, bytesVector, cudaMemcpyHostToDevice);

		// ALE adesso il valore di h_iVector2Data è sul device e nell'host non mi serve più quindi lo rimpiazzo con d_iVector1Data
		// copy vector from device to host to check errors
		cudaMemcpy(h_iVector2Data, d_iVector1Data, sizeof(double) * dimSeparatore, cudaMemcpyDeviceToHost);
		error = cudaGetLastError();
		if(error != cudaSuccess)
		{
			// print the CUDA error message and exit
			printf("\nCUDA error12: %s\n", cudaGetErrorString(error));
			exit(-1);
		}
		//

		cudaDeviceSynchronize();
		// (equivalente) a dim3 dimBlock = dim3(numThreads, 1, 1);
		// le threads avranno quindi un'unica dimensione
		dim3 dimBlockDiv(numThreads, 1, 1);
		// (equivalente) a dim3 dimGrid = dim3(numBlocks, 1, 1);
		// griglia di un'unica dimensione
		dim3 dimGridDiv(numBlocks, 1, 1);

		if(debug) { cudaPrintfInit(); 
		printf("\n DIV:\n %d dimGrid\n %d dimBlock\n",dimGridDiv.x, dimBlockDiv.x ); }

		cudaEvent_t start, stop;
		cudaEventCreate(&start); // vedi http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/
		cudaEventCreate(&stop);	
		cudaEventRecord( start, 0 );

		kernelDivVector<<< dimGridDiv, dimBlockDiv >>>(d_iVector1Data, d_iVector2Data, n, debug);			

		cudaEventRecord( stop, 0 );
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &timeDiv, start, stop );
		//totalDiv+=timeDiv;

		if(debug) { cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); }
		error = cudaGetLastError();
		if(error != cudaSuccess)
		{
			// print the CUDA error message and exit
			printf("\nCUDA error11: %s\n", cudaGetErrorString(error));
			exit(-1);
		}
		/*
		// copy vector from device to host to check errors
		cudaMemcpy(h_iVector2Data, d_iVector1Data, sizeof(double)*n, cudaMemcpyDeviceToHost);
		error = cudaGetLastError();
		if(error != cudaSuccess)
		{
		// print the CUDA error message and exit
		printf("\nCUDA error12: %s\n", cudaGetErrorString(error));
		exit(-1);
		}

		double prec=h_iVector1Data[0];
		for(int i=0; i < n ; i++)	{
		if(prec != h_iVector1Data[i]) printf("\nERRORE:  h_iVector1Data[%d-1]=%f != h_iVector1Data[%d]=%f\n",i,prec,i, h_iVector1Data[i]);
		prec=h_iVector1Data[i];
		}
		if(prec != 2) printf("\nERRORE:  prec=%f\n",prec);
		*/

		cudaFree(d_iVector2Data);	//ALE
		// ALE questo non devo cancellarlo perché c'è il nuovo separatore salvato
		//free(h_iVector2Data);													//ALE
		// ********************** inizio moltiplicazione di h_MatrixData *********************
		// allocate device memory and data	
		double *d_MatrixData = NULL;
		size_t *d_MatrixIndex = NULL;
		cudaMalloc((void **) &d_MatrixData, bytesDataMatrix);
		cudaMalloc((void **) &d_MatrixIndex, bytesIndexMatrix);

		// copy data directly to device memory 		
		// se d_iVector1Data è piccolo si può provare a dichiararlo come costante (non dall'inizio, perchè prima è modificato)
		// analogamente per d_iVector1Data nella fase precedente
		cudaMemcpy(d_MatrixData, h_MatrixData, bytesDataMatrix, cudaMemcpyHostToDevice);
		cudaMemcpy(d_MatrixIndex, h_MatrixIndex, bytesIndexMatrix, cudaMemcpyHostToDevice);
		getNumBlocksAndThreadsDivMult(size, maxThreadsMult, numBlocks, numThreads);
		cudaDeviceSynchronize();
		// (equivalente) a dim3 dimBlock = dim3(numThreads, 1, 1);
		// le threads avranno quindi un'unica dimensione
		dim3 dimBlockMult(numThreads, 1, 1);
		// (equivalente) a dim3 dimGrid = dim3(numBlocks, 1, 1);
		// griglia di un'unica dimensione
		dim3 dimGridMult(numBlocks, 1, 1);
		if(debug) { cudaPrintfInit(); 
		printf("\n MULT:\n %d dimGrid\n %d dimBlock\n",dimGridMult.x, dimBlockMult.x ); }

		cudaEventRecord( start, 0 );

		kernelMultMatrixVector<<< dimGridMult, dimBlockMult >>>(d_MatrixData, d_MatrixIndex, d_iVector1Data, n, size, debug);
		if(debug) { cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); }
		// risultato atteso moltiplicazione: 4			// ALE

		error = cudaGetLastError();
		if(error != cudaSuccess)
		{
			// print the CUDA error message and exit
			printf("\nCUDA error25: %s\n", cudaGetErrorString(error));
			exit(-1);
		}

		float timeMult = 0;
		cudaEventRecord( stop, 0 );
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &timeMult, start, stop );
		totalMult+=timeMult;
		
		// FASE DI NORMALIZZAZIONE
		/* PROVA SENZA NORMALIZZAZIONE * /
		cublasHandle_t handle;
		cublasCreate(&handle);
		double sum; // TODO dopo metto d_sum = cudaMalloc(...)
		cublasDasum(handle, dimCricca, d_MatrixData, 1, &sum);
		//printf("la somma degli elementi: %f\n", sum);
		if (sum != 1.0) {
			// normalizzo solo se necessario
			sum = 1.0 / sum;
			cublasDscal(handle, dimCricca, &sum, d_MatrixData, 1);
			//getchar();
		}
		cublasDestroy(handle);
		/ * */
		//

		// copy final matrix from device to host
		cudaMemcpy(h_MatrixData, d_MatrixData, bytesDataMatrix, cudaMemcpyDeviceToHost);
		error = cudaGetLastError();
		if(error != cudaSuccess)
		{
			// print the CUDA error message and exit
			printf("\nCUDA error27: %s\n", cudaGetErrorString(error));
			exit(-1);
		}

		/*
		// check matrix results
		int precM=h_MatrixData[0];
		for(int i=0; i < size ; i++)	{
			if(precM != h_MatrixData[i]) printf("\nERRORE:  h_MatrixData[%d-1]=%f != h_MatrixData[%d]=%f\n",i,precM,i, h_MatrixData[i]);
			precM=h_MatrixData[i];
		}
		//assert (m==prec);
		if(precM != 4) printf("\nERRORE:  prec=%f\n",precM);
		*/

		// 64000/8 = 8000 elementi nella constant memory

		cudaEventDestroy( start );													//ALE
		cudaEventDestroy( stop );													//ALE

		totalMult+=timeMult;	

		//free(h_iVector1Data);											//ALE
		//free(h_iVector2Data);
		//free(h_MatrixData);
		//free(h_MatrixIndex);
		cudaFree(d_iVector1Data);													//ALE
		//cudaFree(d_iVector2Data);													//ALE
		cudaFree(d_MatrixData);													//ALE
		cudaFree(d_MatrixIndex);													//ALE
		error = cudaGetLastError();
		if(error != cudaSuccess)
		{
			// print the CUDA error message and exit
			printf("\nCUDA error29: %s\n", cudaGetErrorString(error));
			exit(-1);
		}
	//} 
	/*
	if(bestAverageTimeDiv > (totalDiv/100) && (!debug)) { bestAverageTimeDiv = (totalDiv/100); bestMaxThreadsDiv = maxThreadsDiv; 
	printf("bestPartialAverageTimeDiv: %f\n bestPartialMaxThreads = %d\n", bestAverageTimeDiv, bestMaxThreadsDiv);
	}
	if(bestAverageTimeMult > (totalMult/100) && (!debug)) { bestAverageTimeMult = (totalMult/100); bestMaxThreadsMult = maxThreadsMult;
	printf("bestPartialAverageTimeMult: %f\n bestPartialMaxThreads = %d\n", bestAverageTimeMult, bestMaxThreadsMult);
	}		
//}
if(!debug){
	printf("bestAverageTimeDiv: %f\n bestMaxThreadsDiv = %d\n", bestAverageTimeDiv, bestMaxThreadsDiv);
	printf("bestAverageTimeMult: %f\n bestMaxThreadsMult = %d\n", bestAverageTimeMult, bestMaxThreadsMult);
}
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TUTTO INSIEME
//void margAndScatt(size_t sizeCriccaPOW2, size_t sizeSeparatorePOW2, double *tabellaCricca, size_t *tabellaIndiciCricca, size_t dimCricca, size_t dimSeparatore, // dati per marginalization
//			size_t sizeCriccaMulPOW2, double* tabellaSeparatore, double* tabellaCriccaMul, size_t* tabellaIndiciCriccaMul, size_t dimCriccaMul // dati per scattering
//			) {
//
//	double* fiStarOnGpu = marginalizationBigN(sizeCriccaPOW2, sizeSeparatorePOW2, tabellaCricca, tabellaIndiciCricca, dimCricca, dimSeparatore);
//	scattering(sizeCriccaMulPOW2,  // dimTabCricca POW2
//			sizeSeparatorePOW2, // dimTabSep POW2
//			fiStarOnGpu, // double *h_iVector2Data qui ci va il device_v1data
//			tabellaSeparatore, // FI
//			tabellaCriccaMul, // PSI da aggiornare
//			tabellaIndiciCriccaMul, // Indexig of PSI da aggiornare
//
//			dimCriccaMul, // dimensione vera tabCricca
//			dimSeparatore // dim vera tabSe,p
//			);
//}




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