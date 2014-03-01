#ifndef MARGANDSCATT_CU
#define MARGANDSCATT_CU

#include "../BeliefPropagationCpp/Config.h"

#include <limits.h>

#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

#include "margAndScatt.h"

#include "myLib.h"

#if NORMALIZZA_AD_OGNI_PASSO
#include "cublas_v2.h"
#endif

#ifndef DEBUG_FLAG
//const bool debug = true; //false;
#define DEBUG_FLAG FALSE
#endif

/*
#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif
*/


//////////////////////////////////////////// RIDUZIONE ==> MARGINALIZATION //////////////////////////////////////////////////////

//////////////////////////////////////////// SMALL //////////////////////////////////////////////////////
void getNumBlocksAndThreadsSmallN(size_t n, int maxThreads, int &blocks, int &threads){
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
template <unsigned int blockSize> // forse basta usare blockDim.x
__global__ void
reduce1StepSmallN(double *g_idata, double *g_odata, size_t *d_iIndexData, size_t n, size_t const nArray, size_t dimDataTable)
{	
	// extern serve per rendere l'allocazione della memoria condivisa dinamica 
	// si potrebbe utilizzare quella statica se la dimensione fosse nota a compile time
	// nota: dalla 2.0 in avanti abbiamo 48 KB di shared memory
    extern __shared__ double sdata[];

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    size_t tid = threadIdx.x;
	// legge blocchi di dimensione doppia
	// i scorre tutte le threads di tutti i blocchi pari: blocco0[thread0...threadn] blocco2[thread0...threadn]
//  orig:  unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	size_t i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	// in mySum ogni thread dovrebbe avere il valore dell'indice dell'array che deve analizzare 
	// nota: solo metà degli elementi è presa in considerazione, in quanto 0 <= threadIdx.x <= blockDim (non blockDim*2)
	
	size_t index1 = d_iIndexData[i+blockDim.x];				// DA INSERIRE NELLA VERSIONE ROLLED
	size_t index2 = d_iIndexData[i];
	
    //double mySum = (i < n && index2 != SIZE_MAX) ? g_idata[index2] : 0;
	double mySum = (i < n && index2 < dimDataTable) ? g_idata[index2] : 0;
	//cuPrintf ("CUPRINTF 1- blockIdx.x = %d mySum = %f \n",blockIdx.x, mySum);

	// nella prima passata ogni thread prende come mySum il valore preso all'istruzione precedente 
	// + il valore preso dal blocco successivo (viene così presa in considerazione l'altra metà degli elementi)
    //if (i + (blockDim.x*4) < n)
    //    mySum += g_idata[i+(blockDim.x*4)];
    //if (i + blockDim.x < n && index1 != SIZE_MAX)
	if (i + blockDim.x < n && index1 < dimDataTable)
        mySum += g_idata[index1];

    sdata[tid] = mySum;
    __syncthreads();

    // do reduction in shared mem			32*16=512		(nArray*32)  -> nArray*(1<<2) -> 64   (32/nArray)
	switch(nArray){
	case 1: for (size_t s=blockDim.x/2; s >= nArray*64; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 2: for (size_t s=blockDim.x/2; s >= nArray*32; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 4: for (size_t s=blockDim.x/2; s >= nArray*16; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 8: for (size_t s=blockDim.x/2; s >= nArray*8; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 16: for (size_t s=blockDim.x/2; s >= nArray*4; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 32: for (size_t s=blockDim.x/2; s >= nArray*2; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	default: for (size_t s=blockDim.x/2; s >= nArray; s>>=1){								// ALE LAST CORR
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();

		} break;
		}	
    /*for (unsigned int s=blockDim.x/2; s >= nArray*2; s>>=1) // orig: s >= nArray
    {
        if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }
		if(debug && (tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 2- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);


        __syncthreads();
    }*/
	
	
	

#if DEBUG_FLAG
	if( //debug && 
	   (tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 3- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);
#endif

	if (tid < 32)
    {
        // now that we are using warp-synchronous programming (below)
        // we need to declare our shared memory volatile so that the compiler
        // doesn't reorder stores to it and induce incorrect behavior.
        volatile double *smem = sdata;
    	    if (blockSize >=  64 && nArray < 64) smem[tid] = mySum = mySum + smem[tid + 32]; 

			#if DEBUG_FLAG
			if(//debug && 
				(tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 4- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);
#endif


			if (blockSize >=  32 && nArray < 32) smem[tid] = mySum = mySum + smem[tid + 16];
			
#if DEBUG_FLAG
			if(//debug && 
				(tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 5- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);
#endif

			
			if (blockSize >=  16 && nArray < 16) smem[tid] = mySum = mySum + smem[tid + 8];

			#if DEBUG_FLAG
			if(//debug && 
				(tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 6- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);
#endif

			
	        if (blockSize >=  8 && nArray < 8) smem[tid] = mySum = mySum + smem[tid + 4];

			#if DEBUG_FLAG
			if(//debug && 
				(tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 7- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);
#endif

	
	        if (blockSize >=  4 && nArray < 4) smem[tid] = mySum = mySum + smem[tid + 2];

			#if DEBUG_FLAG
			if(//debug &&
				(tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 8- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);
#endif

	
	        if (blockSize >=  2 && nArray < 2) smem[tid] = mySum = mySum + smem[tid + 1];

			#if DEBUG_FLAG
			if(//debug && 
				(tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 9- sdata[%d] = %f mySum = %f \n", tid, sdata[tid], mySum);
#endif

	
	    }



    // write result for this block to global mem
    //if (tid == 0) g_odata[blockIdx.x] = sdata[0];
	if(tid < nArray){
		g_odata[blockIdx.x*nArray + tid] = sdata[tid]; // orig g_odata[blockIdx.x] = sdata[tid];
		//if(debug && (tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, tid, g_odata[tid]);	
		cuPrintf ("CUPRINTF 10- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, tid, g_odata[tid]);
		}
}

/*
    This version uses n/2 threads --
    - performs the first level of reduction when reading from global memory.
    - unrolls the last warp to avoid synchronization where it
    isn't needed.

Note, this kernel needs a minimum of 64*sizeof(T) bytes of shared memory.
In other words if blockSize <= 32, allocate 64*sizeof(T) bytes.
If blockSize > 32, allocate blockSize*sizeof(T) bytes.

*/
template <unsigned int blockSize> // forse basta usare blockDim.x
__global__ void
reduce2StepSmallN(double *g_idata, double *g_odata, size_t n, size_t const nArray)
{	
	// extern serve per rendere l'allocazione della memoria condivisa dinamica 
	// si potrebbe utilizzare quella statica se la dimensione fosse nota a compile time
	// nota: dalla 2.0 in avanti abbiamo 48 KB di shared memory
    extern __shared__ double sdata[];

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    size_t tid = threadIdx.x;
	// legge blocchi di dimensione doppia
	// i scorre tutte le threads di tutti i blocchi pari: blocco0[thread0...threadn] blocco2[thread0...threadn]
//  orig:  unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	size_t i = blockIdx.x*(blockDim.x*2) + threadIdx.x;
	// in mySum ogni thread dovrebbe avere il valore dell'indice dell'array che deve analizzare 
	// nota: solo metà degli elementi è presa in considerazione, in quanto 0 <= threadIdx.x <= blockDim (non blockDim*2)
    double mySum = (i < n) ? g_idata[i] : 0;
	//cuPrintf ("CUPRINTF 1- blockIdx.x = %d mySum = %f \n",blockIdx.x, mySum);

	// nella prima passata ogni thread prende come mySum il valore preso all'istruzione precedente 
	// + il valore preso dal blocco successivo (viene così presa in considerazione l'altra metà degli elementi)
    if (i + blockDim.x < n)
        mySum += g_idata[i+blockDim.x];

    sdata[tid] = mySum;
    __syncthreads();

	switch(nArray){
	case 1: for (size_t s=blockDim.x/2; s >= nArray*64; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 2: for (size_t s=blockDim.x/2; s >= nArray*32; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 4: for (size_t s=blockDim.x/2; s >= nArray*16; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 8: for (size_t s=blockDim.x/2; s >= nArray*8; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 16: for (size_t s=blockDim.x/2; s >= nArray*4; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	case 32: for (size_t s=blockDim.x/2; s >= nArray*2; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();
	    
		} break;
	default: for (size_t s=blockDim.x/2; s >= nArray; s>>=1){
		if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }    __syncthreads();

		} break;
		}

    // do reduction in shared mem
    /*for (unsigned int s=blockDim.x/2; s >= (1<<(32/nArray)); s>>=1) // orig: s >= nArray
    {
        if (tid < s)
        {
            sdata[tid] = mySum = mySum + sdata[tid + s];
        }
		if(debug && (tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("\nCUPRINTF 1 - ciclo\n");
		
		
        __syncthreads();
    }*/
	//if((tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 3- sdata[%d] = %f \n", tid, sdata[tid]);

#if DEBUG_FLAG
    if(//debug && 
		(tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 12 - mySum = %f\n", mySum);
#endif

	if (tid < 32)
    {
        // now that we are using warp-synchronous programming (below)
        // we need to declare our shared memory volatile so that the compiler
        // doesn't reorder stores to it and induce incorrect behavior.
        volatile double *smem = sdata;
    	    if (blockSize >=  64 && nArray < 64) smem[tid] = mySum = mySum + smem[tid + 32]; 
			if (blockSize >=  32 && nArray < 32) smem[tid] = mySum = mySum + smem[tid +  16];
			if (blockSize >=  16 && nArray < 16) smem[tid] = mySum = mySum + smem[tid + 8];
	        if (blockSize >=  8 && nArray < 8) smem[tid] = mySum = mySum + smem[tid + 4];
	        if (blockSize >=  4 && nArray < 4) smem[tid] = mySum = mySum + smem[tid + 2];
	        if (blockSize >=  2 && nArray < 2) smem[tid] = mySum = mySum + smem[tid + 1];
	
	
	    }



	if(tid < nArray){
		g_odata[blockIdx.x*nArray + tid] = sdata[tid]; // orig g_odata[blockIdx.x] = sdata[tid];
		//if(debug && (tid == 0 || tid == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, tid, g_odata[tid]);	
		cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, tid, g_odata[tid]);
		}
}

void reduceSmallNArray(size_t  n,
				  	size_t nArray,
                 	int  numThreads,
                  	int  numBlocks,
                  	int  maxThreads,
                  	int  cpuFinalThreshold,
                  	//double *h_odata,
                  	double *d_idata,
                  	double *d_odata,
					size_t *d_iIndexData,
					size_t dimRisultato,
					size_t sizeDataTable
					)
{
	//bool needReadBack = true;
	
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
		
		//size_t dimResult = dimRisultato * sizeof(double);
		/* 
		double *gpu_result = (double *) malloc(dimResult);
		for (size_t i=0; i < nArray; i++) gpu_result[i]=0;
		*/

		#if DEBUG_FLAG
			//if(debug) { 
				cudaPrintfInit();
					// execute the kernel
					printf("\n1 esecuzione:\n %d dimGrid\n %d dimBlock\n %d smemSize\n",dimGrid.x, dimBlock.x, smemSize ); 
			//}
#endif

			//reduce1StepSmallN<<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, debug);

			//printf("numThreads: %d", numThreads);
					switch (numThreads){
						case 512:
						reduce1StepSmallN<512><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 256:
						reduce1StepSmallN<256><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 128:
						reduce1StepSmallN<128><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 64:
						reduce1StepSmallN<64><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 32:
						reduce1StepSmallN<32><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 16:
						reduce1StepSmallN<16><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 8:
						reduce1StepSmallN<8><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 4:
						reduce1StepSmallN<4><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 2:
						reduce1StepSmallN<2><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
						case 1:
						reduce1StepSmallN<1><<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, d_iIndexData, n, nArray, sizeDataTable);	break;
					}
			

			#if DEBUG_FLAG
			//if(debug) { 
				cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); 
			//}
			#endif

        
            // sum partial block sums on GPU
            int s=numBlocks;
			int iterDebug = 0;
            
			while (s > cpuFinalThreshold) // abbiamo visto che cpuFinalThreshold è 1
            {
                int threads = 0, blocks = 0;
				size_t nElements = s*nArray;

                getNumBlocksAndThreadsSmallN(nElements, maxThreads, blocks, threads);
				dim3 dimBlock(threads, 1, 1);
				dim3 dimGrid(blocks, 1, 1);
				// when there is only one warp per block, we need to allocate two warps
			    // worth of shared memory so that we don't index shared memory out of bounds
				smemSize = (threads <= 32) ? 2 * threads * sizeof(double) : threads * sizeof(double);

				// do in input al reduce da rielaborare gli stessi dati

				#if DEBUG_FLAG
				//if(debug) { 
					cudaPrintfInit();
					printf("\n\t iter esecuzione:\n %d dimGrid\n %d dimBlock\n %d smemSize\n %d s\n %d n\n",dimGrid.x, dimBlock.x, smemSize, s, n);
					printf("nElements: %d \n",nElements); 
					//}
				#endif


				switch (threads){
					case 512:
					reduce2StepSmallN<512><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 256:
					reduce2StepSmallN<256><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 128:
					reduce2StepSmallN<128><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 64:
					reduce2StepSmallN<64><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 32:
					reduce2StepSmallN<32><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 16:
					reduce2StepSmallN<16><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 8:
					reduce2StepSmallN<8><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 4:
					reduce2StepSmallN<4><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 2:
					reduce2StepSmallN<2><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
					case 1:
					reduce2StepSmallN<1><<< dimGrid, dimBlock, smemSize >>>(d_odata, d_odata, nElements, nArray);	break;
				}
					
/*
				#if DEBUG_FLAG
				//if(debug) { 
					cudaPrintfDisplay (stdout, true);   cudaPrintfEnd (); 
				//}
				#endif
*/
				
				error = cudaGetLastError();
			  	if(error != cudaSuccess)
			  	{
			    // print the CUDA error message and exit
			    printf("\nCUDA kernel error: %s\n", cudaGetErrorString(error));
			    exit(-1);
			  }
				s = blocks;
				iterDebug++;
            }
/*
			#if DEBUG_FLAG
			//if(debug) { 
				printf("\n passato il while... s = %d",s); printf("\niterazioni: %d",iterDebug);
			//}
			#endif
*/
			//if(iterDebug!=0) { printf("\n ci sono iterazioni");exit(-1);}
            /*
			if (s > 1)
            {
                // copy result from device to host
				if(debug) { printf("\n s = %d, nArray = %d",s,nArray);}
                //cudaMemcpy(h_odata, d_odata, s * sizeof(double) * nArray, cudaMemcpyDeviceToHost);
				 cudaMemcpy(h_odata, d_odata, s * sizeof(double) * dimRisultato, cudaMemcpyDeviceToHost);
			   error = cudaGetLastError();
			  if(error != cudaSuccess)
			  {
			    // print the CUDA error message and exit
			    printf("\nCUDA error1: %s\n", cudaGetErrorString(error));
			    exit(-1);
			  }	

				double prec;
                //for (int i=0; i < nArray; i++) 
				for (size_t i=0; i < dimRisultato; i++) 
                {
					for(int j=0; j < s; j++) {
						gpu_result[i] += h_odata[i*s+j];
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
		   */
		/*
		if (needReadBack)
	    {
	        // copy final sum from device to host
          // non devo copiarla e restituisco direttamente il puntatore in GPU
		 
		 cudaMemcpy(gpu_result, d_odata, sizeof(double)*nArray, cudaMemcpyDeviceToHost);
			// in teoria cudaError_t si può togliere perchè è dichiarato su
		  error = cudaGetLastError();
		  if(error != cudaSuccess)
		  {
		    // print the CUDA error message and exit
		    printf("\nCUDA error2: %s\n", cudaGetErrorString(error));
		    exit(-1);
		  }	
		  
	    }
	return gpu_result;  
	*/
}

double* marginalizationSmallN(size_t size, size_t nArray, double *h_idata, size_t *h_iIndexData, size_t dimInput, size_t dimRisultato
							  , long long* elapsedSum, long long* elapsedDivMul
							  ){
	// TESTA CON NVPROF
	//int size = 1<<24; // 1<<24;    // number of elements to reduce -> default:  16777216				// ALE
	//int nArray = 1<<5;	// m = 16384 
	//int m = 1<<6; // 4 7 10 14 17 20 
	//da testare con int nArray = 1<<4;1<<9;
	//int m = 1<<19; 1<<15;
	//size = nArray*m;	
	size_t m = size / nArray;
	//size = 1<<16;																						// ALE
	//nArray = 1<<20;																					// ALE
    //int maxThreads = 512;				// number of threads per block
    int cpuFinalThreshold = 1;				// da testare
    /* STAMPE */
/*
#if DEBUG_FLAG
	printf("%d elements\n", size);
    printf("%d nArray\n", nArray);
    printf("%d m\n", m);
	printf("%d dimCriccaDaSommare\n", dimInput);
	printf("%d dimSeparatore\n", dimRisultato);
#endif
*/
    //printf("%d maxThreads\n", maxThreads);
	/* */

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


	//for(int maxThreads = 128; maxThreads < 1024; maxThreads++){
	/*	float bestAverageTime = 100.0;
		int bestcpuFinalThreshold=1;
		int bestMaxThreads=32; */
		
	//for(int cpuFinalThreshold = 1; cpuFinalThreshold <200; cpuFinalThreshold=cpuFinalThreshold+10){
		//for(int maxThreads = nArray; maxThreads <1024; maxThreads*=2){
		int maxThreads = 512;

			//float total = 0.0;
			//for(int i =0; i<100; i++){
					//float time=0;	// GPU TIME
			        // create input
			        size_t bytesValuesInput = dimInput * sizeof(double);										// ALE
			        //double *h_idata = (double *) malloc(bytesValuesInput);										// ALE
					size_t bytesIndexInput = size * sizeof(size_t);											// ALE
					//int *h_iIndexData = (int *) malloc(bytesIndexInput);										// ALE
		
					//for (int i=0; i<size; i++) { h_idata[i] = 1; h_iIndexData[i]=i; }							// ALE
		
		
	        		int numBlocks = 0;
			        int numThreads = 0;
					getNumBlocksAndThreadsSmallN(size, maxThreads, numBlocks, numThreads);

					if (numBlocks == 1) { cpuFinalThreshold = 1; 
/*
						#if DEBUG_FLAG
						//if(debug) 
							printf("cpuFinalThreshold = 1\n");
						#endif
*/
					}
        
			        // allocate mem for the result on host side
					size_t bytesOutput = sizeof(double)*nArray*numBlocks;
			        //double *h_odata = (double *) malloc(bytesOutput);
/*
					#if DEBUG_FLAG
					//if(debug) 
						printf("%d blocks\n", numBlocks);
					#endif
*/
        
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
				    //printf("\nbytesOutput=%d\n",bytesOutput);
			        //cudaMemcpy(d_odata, h_idata, bytesOutput, cudaMemcpyHostToDevice);			//NON SERVE!!!

#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
					// per i test si può inserire un warm-up
					cudaEvent_t start, stop;
					cudaEventCreate(&start); // vedi http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/
					cudaEventCreate(&stop);	
					cudaEventRecord( start, 0 );
#endif
			        //double *gpu_result;																		// ALE è lungo nArray

/*
#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
	std::chrono::system_clock::time_point begin = std::chrono::high_resolution_clock::now();
#endif
*/
			        //gpu_result = 
					reduceSmallNArray(size, nArray, numThreads, numBlocks, maxThreads,
			                                        cpuFinalThreshold, d_idata, d_odata, d_iIndexData, dimRisultato, dimInput);
/*
#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
	std::chrono::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif
*/
#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
					cudaEventRecord( stop, 0 );
					cudaEventSynchronize( stop );
					cudaEventElapsedTime( &time, start, stop );
					//total+=time;										// ALE 
					*elapsedSum += time * 1000000;
#endif
					//double prec=gpu_result[0];
					/*for(int i=0; i < nArray ; i++)	{
						if(prec != gpu_result[i]) printf("\nERRORE:  gpu_result[%d-1]=%f != gpu_result[%d]=%f\n",i,prec,i, gpu_result[i]);
						prec=gpu_result[i];
					//    printf("\nGPU result = %f\n", gpu_result[i]);
					}*/
					//assert (m==prec);
					//if(prec != m) printf("\nERRORE:  m=%d != prec=%f\n",m,prec);
#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
					cudaEventDestroy( start );													//ALE
					cudaEventDestroy( stop );
#endif													//ALE
		
		
			        //printf("\nGPU result       \t= \t %f\n", prec);
					assert ((size % nArray) == 0);
					//printf("risultato atteso \t= \t %d\n", size / nArray);

					//free(gpu_result);	    
			        //free(h_idata);													//ALE
					//free(h_iIndexData);
			        //free(h_odata);													//ALE
					cudaFree(d_idata);													//ALE
			        //cudaFree(d_odata);													//ALE
					cudaFree(d_iIndexData);
			//}
			/*
			if(bestAverageTime > (total/100) && (!debug)) { bestAverageTime = (total/100); bestMaxThreads = maxThreads;
			// printf("partialBestAverageTime: %f\n bestMaxThreads = %d\n", bestAverageTime, bestMaxThreads); 
			}
			*/
		//}
		//if(!debug)printf("bestAverageTime: %f\n bestMaxThreads = %d\n", bestAverageTime, bestMaxThreads);
			return d_odata;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////// BIG //////////////////////////////////////////////////////

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
reduce1StepBigN(double *g_idata, double *g_odata, size_t *d_iIndexData, size_t n, size_t halfN, size_t nArray, size_t dimDataTable)//, unsigned int fraction)
{	
	// i scorre la prima metà dei dati in input
	size_t i = blockIdx.x*blockDim.x + threadIdx.x;
	//unsigned const int halfN = n>>1;

	// somma elementi della prima metà con elementi della seconda metà
	int index1 = d_iIndexData[i+halfN];
	int index2 = d_iIndexData[i];
		
    if (i < halfN){ // sta sempre dentro all'if
		//g_odata[i] = g_idata[d_iIndexData[i]] + g_idata[d_iIndexData[i+halfN]];
		//double mySum = (index1 != SIZE_MAX) ? g_idata[index1] : 0;
		double mySum = (index1 < dimDataTable) ? g_idata[index1] : 0;
		//g_odata[i] = (index2 != SIZE_MAX) ? g_idata[index2] + mySum : mySum;
		g_odata[i] = (index2 < dimDataTable) ? g_idata[index2] + mySum : mySum;
		#if DEBUG_FLAG
			if(//debug && 
				(threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) 
				cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, threadIdx.x, g_odata[threadIdx.x]);
		#endif
	}
}

__global__ void														// non so se passarlo come parametro o farlo calcolare alle theads... TESTARE
reduce2StepBigN(double *g_idata, double *g_odata, size_t n, size_t halfN, size_t nArray)//, unsigned int fraction)
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
/*
		#if DEBUG_FLAG
			if(//debug && 
				(threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) 
				cuPrintf ("CUPRINTF 4- g_odata[%d*%d + %d] = %f \n", blockIdx.x, nArray, threadIdx.x, g_odata[threadIdx.x]);
		#endif
*/
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
					size_t dimRisultato,
					size_t sizeDataTable
					)
{
	//bool needReadBack = true;
	
    cudaDeviceSynchronize();
    	// (equivalente) a dim3 dimBlock = dim3(numThreads, 1, 1);
		// le threads avranno quindi un'unica dimensione
    	dim3 dimBlock(numThreads, 1, 1);
    	// (equivalente) a dim3 dimGrid = dim3(numBlocks, 1, 1);
		// griglia di un'unica dimensione
	    dim3 dimGrid(numBlocks, 1, 1);

		//cudaError_t error;
		
		//size_t dimResult = nArray * sizeof(double);
		//size_t dimResult = dimRisultato * sizeof(double);

		// ALE 2014-02-23
		/*
		double *gpu_result = (double *) malloc(dimResult);
		// ALE_DOMANDA
		// SERVE??? non viene sovrascritto poi dalla cudaMemCpy???
		for (int i=0; i < dimRisultato; i++) gpu_result[i]=0;		
		//
		*/
/*
		#if DEBUG_FLAG
		//if(debug) { 
			cudaPrintfInit();
				// execute the kernel
				printf("\nSTART esecuzione:\n %d dimGrid\n %d dimBlock\n",dimGrid.x, dimBlock.x ); 
		//}
		#endif
*/

		reduce1StepBigN<<< dimGrid, dimBlock>>>(d_idata, d_odata, d_iIndexData, n, n>>1, nArray, sizeDataTable);
		
/*
		#if DEBUG_FLAG
		//if(debug) { 
			cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); 
		//}
		#endif
*/
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
			
/*
			#if DEBUG_FLAG
			//if(debug) 
				printf("\n%d elementi:\n %d dimGrid\n %d dimBlock\n",nElements,dimGrid.x, dimBlock.x ); 
	
			//if(debug) { 
				cudaPrintfInit();
			
				// execute the kernel
				printf("\n%d esecuzione:\n %d dimGrid\n %d dimBlock\n",nElements,dimGrid.x, dimBlock.x ); 
			//}
			#endif
*/

			reduce2StepBigN<<< dimGrid, dimBlock>>>(d_odata, d_odata, nElements, nElements>>1, nArray);
			
/*
			#if DEBUG_FLAG
			//if(debug) { 
				cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); 
			//}
			#endif
*/
			
        }
        /*
		if (needReadBack)
	    {
		*/
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
	   /*
		}
		*/
		// ALE 2014-02-23
	/*
	return gpu_result;  
	*/

}

//                   dimTabCricca POW2, dimTabSep POW2, tabCricca,     tabIndiciCricca,   dimensione vera tabCricca, dim vera tabSep
double* marginalizationBigN(size_t size, size_t nArray, double *h_idata, size_t *h_iIndexData, size_t dimInput, size_t dimRisultato
							, long long* elapsedSum, long long* elapsedDivMul
							){
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
    //int cpuFinalThreshold = 1;
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

	
	//float total = 0.0;

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
       
	    //if (numBlocks == 1) cpuFinalThreshold = 1;
       
        // allocate mem for the result on host side
		size_t bytesOutput = sizeof(double)*(size/2);											// ALE
        double *h_odata = (double *) malloc(bytesOutput);
/*
#if DEBUG_FLAG
        if(debug) printf("%d blocks\n", numBlocks);
#endif
*/   
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

		#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
		// per i test si può inserire un warm-up
			cudaEvent_t start, stop;
			float time;	// GPU TIME
			cudaEventCreate(&start); // vedi http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/
			cudaEventCreate(&stop);	
		
		
			cudaEventRecord( start, 0 );
		#endif

	        //double *gpu_result;																		// ALE è lungo nArray
			
			//ALE 2014-02-23
			/*
	        gpu_result = 
			*/
/*
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
	std::chrono::system_clock::time_point begin = std::chrono::high_resolution_clock::now();
#endif
*/
			reduceBigNArray(size, nArray, numThreads, numBlocks, maxThreads,
							h_odata, d_idata, d_odata, d_iIndexData, dimRisultato, dimInput);
/*
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
	std::chrono::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	*elapsedSum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif
*/

	#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
			cudaEventRecord( stop, 0 );
			cudaEventSynchronize( stop );
			cudaEventElapsedTime( &time, start, stop );
			//total+=time; // ALE
			*elapsedSum += time * 1000000;
	#endif

			//printf ("\nTime for the kernel: %f ms\n", time);											// ALE 
			//double prec=gpu_result[0];
			/*for(int i=0; i < nArray ; i++)	{
				if(prec != gpu_result[i]) {printf("\nERRORE:  gpu_result[%d-1]=%f != gpu_result[%d]=%f",i,prec,i, gpu_result[i]); exit(-1);}
				prec=gpu_result[i];
			//    printf("\nGPU result = %f\n", gpu_result[i]);
			}*/
//			assert (m==prec);

			#if (!TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA)
			cudaEventDestroy( start );													// ALE
			cudaEventDestroy( stop );													//ALE
			#endif

			
			
	        //printf("\nGPU result       \t= \t %f\n", prec);
			//assert ((size % nArray) == 0);
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
kernelDivVector(double *g_iVector1Data, double *g_iVector2Data, size_t n)
{	
//	if(debug && (threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 1- \n");
	size_t i = blockIdx.x*(blockDim.x) + threadIdx.x;
	if (i < n)
		// ALE
			g_iVector1Data[i] = (g_iVector2Data[i] > ZERO_DIVISIONE) ? g_iVector1Data[i] / g_iVector2Data[i] : 0; // ZERO_DIVISIONE = 0.0
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
kernelMultMatrixVector(double *d_MatrixData, size_t *d_MatrixIndex, double * g_iVector1Data, size_t dimSeparatorePow2, size_t size, size_t sizeDataTable){ // dimSep, size, dimCricca
	size_t i = blockIdx.x*(blockDim.x) + threadIdx.x;
	size_t index = d_MatrixIndex[i];
	//if (index != SIZE_MAX)
	if (index < sizeDataTable)
		d_MatrixData[index] *= g_iVector1Data[i % dimSeparatorePow2]; // dimSepPow2 vs dimSep ???
	
	//if(debug && (threadIdx.x == 0 || threadIdx.x == (blockDim.x-1))&&(blockIdx.x==0 || blockIdx.x==gridDim.x-1)) cuPrintf ("CUPRINTF 3- d_MatrixData[%d] = %f \n", index, d_MatrixData[index]);
}

//           dimTabCricca POW2, dimTabSep POW2, tabCricca, tabIndiciCricca,        dimensione vera tabCricca, dim vera tabSep
void scattering(size_t size,  // dimTabCricca POW2
			size_t n, // dimTabSep POW2
			double *iVector1Data, // double *h_iVector2Data qui ci va il device_v1data
			double *h_iVector2Data, // FI
			double *h_MatrixData, // PSI da aggiornare
			size_t *h_MatrixIndex, // Indexig of PSI da aggiornare

			size_t dimCricca, // dimensione vera tabCricca
			size_t dimSeparatore // dim vera tabSep
			, long long* elapsedSum, long long* elapsedDivMul
			, bool isOnHost
			) {
	// TESTA CON NVPROF
	//unsigned int n = 1<<12;//19;	
	//unsigned int m = 1<<12;//5;
	//unsigned int size = m*n;

	

	//int maxThreadsDiv = 512;				/// ATTENZIONE, QUANDO SI FANNO I TEST BISOGNA VARIARE ENTRAMBI I VALORI!!!
	/*
	size_t m = size / n;
	printf("%d n\n", n);
	printf("%d m\n", m);
	printf("%d size\n", size);
	printf("%d dimSep\n", dimSeparatore);
	printf("%d dimCricca\n", dimCricca);
	*/
	cudaError_t error;

	//float bestAverageTimeMult = 100.0;
	//float bestAverageTimeDiv = 100.0;
	//unsigned int bestMaxThreadsDiv = 0;
	//unsigned int bestMaxThreadsMult = 0;

	
	//for (int maxThreads = 32; maxThreads < 1024; maxThreads*=2) {
	// ALE
	int maxThreads = 256;

	unsigned int maxThreadsDiv = maxThreads;
	unsigned int maxThreadsMult = maxThreads;

	//float totalDiv = 0.0;
	//float totalMult = 0.0;
		
	//for(int i =0; i<100; i++){
	
		//float timeDiv=0;																						// GPU TIME
		// create input
		// BISOGNA TOGLIERE GLI INDICI DALL'ARRAY, (METTERE L'ARRAY NELLA CONSTANT MEMORY)...
		//size_t bytesVector = n * sizeof(double); // ALE

		// ALE MOD
		size_t bytesVector = dimSeparatore * sizeof(double);

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

		//assert(n%numThreads==0);

		// allocate device memory and data
		//double *d_iVector1DataP = NULL;
		double *d_iVector2Data = NULL;
		//cudaMalloc((void **) &d_iVector1DataP, bytesVector);
		cudaMalloc((void **) &d_iVector2Data, bytesVector);

		// copy data directly to device memory
		//cudaMemcpy(d_iVector1Data, h_iVector1Data, bytesVector, cudaMemcpyHostToDevice);
		cudaMemcpy(d_iVector2Data, h_iVector2Data, bytesVector, cudaMemcpyHostToDevice);

		double* d_iVector1Data = NULL;
		if (isOnHost) {
			// devo tirarmelo su dall'host
			cudaMalloc((void**) &d_iVector1Data, bytesVector);
			cudaMemcpy(d_iVector1Data, iVector1Data, bytesVector, cudaMemcpyHostToDevice);
		} else {
			// devo salvarmelo sull'host
			d_iVector1Data = iVector1Data;
			// ALE adesso il valore di h_iVector2Data è sul device e nell'host non mi serve più quindi lo rimpiazzo con d_iVector1Data
			// copy vector from device to host to check errors
			cudaMemcpy(h_iVector2Data, d_iVector1Data, bytesVector, cudaMemcpyDeviceToHost);
			error = cudaGetLastError();
			if(error != cudaSuccess)
			{
				// print the CUDA error message and exit
				printf("\nCUDA error12: %s\n", cudaGetErrorString(error));
				exit(-1);
			}
			//
		}

		

		cudaDeviceSynchronize();
		// (equivalente) a dim3 dimBlock = dim3(numThreads, 1, 1);
		// le threads avranno quindi un'unica dimensione
		dim3 dimBlockDiv(numThreads, 1, 1);
		// (equivalente) a dim3 dimGrid = dim3(numBlocks, 1, 1);
		// griglia di un'unica dimensione
		dim3 dimGridDiv(numBlocks, 1, 1);

/*
		#if DEBUG_FLAG
		//if(debug) { 
			cudaPrintfInit(); 
			printf("\n DIV:\n %d dimGrid\n %d dimBlock\n",dimGridDiv.x, dimBlockDiv.x ); 
		//}
		#endif
*/


		#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
		cudaEvent_t start, stop;
		cudaEventCreate(&start); // vedi http://docs.nvidia.com/cuda/cuda-c-best-practices-guide/
		cudaEventCreate(&stop);	
		cudaEventRecord( start, 0 );
		#endif

/*
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
	std::chrono::system_clock::time_point begin = std::chrono::high_resolution_clock::now();
#endif
*/
		kernelDivVector<<< dimGridDiv, dimBlockDiv >>>(d_iVector1Data, d_iVector2Data, dimSeparatore);	
/*
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
	std::chrono::system_clock::time_point end = std::chrono::high_resolution_clock::now();
	*elapsedDivMul += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif
*/

		#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
		cudaEventRecord( stop, 0 );
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &timeDiv, start, stop );
		*elapsedDivMul += timeDiv * 1000000;
		//totalDiv+=timeDiv;
		#endif

/*
		#if DEBUG_FLAG
		//if(debug) {
			cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); 
		//}
		#endif
*/

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
/*
		#if DEBUG_FLAG
		//if(debug) { 
			cudaPrintfInit(); 
			printf("\n MULT:\n %d dimGrid\n %d dimBlock\n",dimGridMult.x, dimBlockMult.x ); 
		//}
		#endif
*/


		#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
		cudaEventRecord( start, 0 );
		#endif

/*
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
	begin = std::chrono::high_resolution_clock::now();
#endif
*/
		kernelMultMatrixVector<<< dimGridMult, dimBlockMult >>>(d_MatrixData, d_MatrixIndex, d_iVector1Data, n, size, dimCricca);
/*
#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
	end = std::chrono::high_resolution_clock::now();
	*elapsedDivMul += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
#endif
*/
/*
		#if DEBUG_FLAG
		//if(debug) { 
			cudaPrintfDisplay (stdout, true); cudaPrintfEnd (); 
		//}
		#endif
*/
		// risultato atteso moltiplicazione: 4			// ALE

		error = cudaGetLastError();
		if(error != cudaSuccess)
		{
			// print the CUDA error message and exit
			printf("\nCUDA error25: %s\n", cudaGetErrorString(error));
			exit(-1);
		}

// se c'è viene incluso nel conteggio del tempo
#if NORMALIZZA_AD_OGNI_PASSO		
		// FASE DI NORMALIZZAZIONE
		/* PROVA SENZA NORMALIZZAZIONE */
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
		/* */
		//
#endif


		#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
		float timeMult = 0;
		cudaEventRecord( stop, 0 );
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &timeMult, start, stop );
		//totalMult+=timeMult;
		*elapsedDivMul += timeMult * 1000000;
		#endif

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
		#if !TEMPO_COMPLESSIVO && CONSIDERA_MARGINALIZZAZIONE_E_SCATTERING_DIVISE && !CONSIDERA_TRASFERIMENTI_MEMORIA
		cudaEventDestroy( start );													//ALE
		cudaEventDestroy( stop );													//ALE
		#endif

		//totalMult+=timeMult;	

		//for (size_t l = 0; l < dimCricca; l++)
		//	printf("el[%d] = %f\n", l, h_MatrixData[l]);



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

double* recuperaDaGPU(double* tabella, size_t sizeTabella)
{
	size_t bytesTabella = sizeof(double) * sizeTabella;
	double* tabellaCPU = (double*) malloc(bytesTabella);
	cudaMemcpy(tabellaCPU, tabella, bytesTabella, cudaMemcpyDeviceToHost);
	cudaFree(tabella);

	return tabellaCPU;
}

#endif