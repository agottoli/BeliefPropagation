/*
    Parallel reduction

    This sample shows how to perform a reduction operation on an array of values
    to produce a single value.

    Reductions are a very common computation in parallel algorithms.  Any time
    an array of values needs to be reduced to a single value using a binary
    associative operator, a reduction can be used.  Example applications include
    statistics computaions such as mean and standard deviation, and image
    processing applications such as finding the total luminance of an
    image.

    It assumes the input size is a power of 2. <-----

    COMMAND LINE ARGUMENTS

    "--shmoo":         Test performance for 1 to 32M elements with each of the 7 different kernels
    "--n=<N>":         Specify the number of elements to reduce (default 1048576)
    "--threads=<N>":   Specify the number of threads per block (default 128)
    "--kernel=<N>":    Specify which kernel to run (0-6, default 6)
    "--maxblocks=<N>": Specify the maximum number of thread blocks to launch (kernel 6 only, default 64)
    "--cpufinal":      Read back the per-block results and do final sum of block sums on CPU (default false)
    "--cputhresh=<N>": The threshold of number of blocks sums below which to perform a CPU final reduction (default 1)
    "-type=<T>":       The datatype for the reduction, where T is "int", "float", or "double" (default int)
*/

// CUDA Runtime
#include <cuda_runtime.h>

//#include "cuPrintf.cuh"
#include "cuPrintf.cu"

// Utilities and system includes
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// serve per linkare il kernel nvidia, in un file separato
//#include "reduction.h"

////////// OTHER NVIDIA CUDA FUNCTIONS (EDITED) //////////
#ifndef MAX
#define MAX(a,b) (a > b ? a : b)
#endif

#define MAX_BLOCK_DIM_SIZE 65535

// forse si può togliere
//#ifdef WIN32
//#define strcasecmp strcmpi
//#endif

extern "C"
	bool isPow2(unsigned int x)
	{
	    return ((x&(x-1))==0);
	}
	
	size_t nextPow2(size_t x)
	{
	    --x;
	    x |= x >> 1;
	    x |= x >> 2;
	    x |= x >> 4;
	    x |= x >> 8;
	    x |= x >> 16;
	    return ++x;
	}
	
//#include "myLibKernelSmallN.h"
//#include "myLibKernelBigN.h"

// Beginning of GPU Architecture definitions
inline int _ConvertSMVer2Cores(int major, int minor)
{
    // Defines for GPU Architecture types (using the SM version to determine the # of cores per SM
    typedef struct
    {
        int SM; // 0xMm (hexidecimal notation), M = SM Major version, and m = SM minor version
        int Cores;
    } sSMtoCores;

    sSMtoCores nGpuArchCoresPerSM[] =
    {
        { 0x10,  8 }, // Tesla Generation (SM 1.0) G80 class
        { 0x11,  8 }, // Tesla Generation (SM 1.1) G8x class
        { 0x12,  8 }, // Tesla Generation (SM 1.2) G9x class
        { 0x13,  8 }, // Tesla Generation (SM 1.3) GT200 class
        { 0x20, 32 }, // Fermi Generation (SM 2.0) GF100 class
        { 0x21, 48 }, // Fermi Generation (SM 2.1) GF10x class
        { 0x30, 192}, // Kepler Generation (SM 3.0) GK10x class
        { 0x35, 192}, // Kepler Generation (SM 3.5) GK11x class
        {   -1, -1 }
    };

    int index = 0;

    while (nGpuArchCoresPerSM[index].SM != -1)
    {
        if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor))
        {
            return nGpuArchCoresPerSM[index].Cores;
        }

        index++;
    }

    // If we don't find the values, we default use the previous one to run properly
    printf("MapSMtoCores for SM %d.%d is undefined.  Default to use %d Cores/SM\n", major, minor, nGpuArchCoresPerSM[7].Cores);
    return nGpuArchCoresPerSM[7].Cores;
}

// This function returns the best GPU (with maximum GFLOPS)
inline int gpuGetMaxGflopsDeviceId()
{
    int current_device     = 0, sm_per_multiproc  = 0;
    int max_compute_perf   = 0, max_perf_device   = 0;
    int device_count       = 0, best_SM_arch      = 0;
    cudaDeviceProp deviceProp;
    cudaGetDeviceCount(&device_count);

    if (device_count == 0)
    {
        fprintf(stderr, "gpuGetMaxGflopsDeviceId() CUDA error: no devices supporting CUDA.\n");
        exit(EXIT_FAILURE);
    }

    // Find the best major SM Architecture GPU device
    while (current_device < device_count)
    {
        cudaGetDeviceProperties(&deviceProp, current_device);

        // If this GPU is not running on Compute Mode prohibited, then we can add it to the list
        if (deviceProp.computeMode != cudaComputeModeProhibited)
        {
            if (deviceProp.major > 0 && deviceProp.major < 9999)
            {
                best_SM_arch = MAX(best_SM_arch, deviceProp.major);
            }
        }

        current_device++;
    }

    // Find the best CUDA capable GPU device
    current_device = 0;

    while (current_device < device_count)
    {
        cudaGetDeviceProperties(&deviceProp, current_device);

        // If this GPU is not running on Compute Mode prohibited, then we can add it to the list
        if (deviceProp.computeMode != cudaComputeModeProhibited)
        {
            if (deviceProp.major == 9999 && deviceProp.minor == 9999)
            {
                sm_per_multiproc = 1;
            }
            else
            {
                sm_per_multiproc = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);
            }

            int compute_perf  = deviceProp.multiProcessorCount * sm_per_multiproc * deviceProp.clockRate;

            if (compute_perf  > max_compute_perf)
            {
                // If we find GPU with SM major > 2, search only these
                if (best_SM_arch > 2)
                {
                    // If our device==dest_SM_arch, choose this, or else pass
                    if (deviceProp.major == best_SM_arch)
                    {
                        max_compute_perf  = compute_perf;
                        max_perf_device   = current_device;
                    }
                }
                else
                {
                    max_compute_perf  = compute_perf;
                    max_perf_device   = current_device;
                }
            }
        }

        ++current_device;
    }

    return max_perf_device;
}

inline int findCudaDevice() //int argc, const char **argv)
{
    cudaDeviceProp deviceProp;
    int devID = 0;

        //  pick the device with highest Gflops/s
        devID = gpuGetMaxGflopsDeviceId();
        cudaSetDevice(devID);
        cudaGetDeviceProperties(&deviceProp, devID);
        printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n", devID, deviceProp.name, deviceProp.major, deviceProp.minor);

    return devID;
}



