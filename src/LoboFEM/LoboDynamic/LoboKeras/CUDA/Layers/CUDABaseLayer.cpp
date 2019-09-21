#include "CUDABaseLayer.h"
#include <cuda.h>
#include <cuda_runtime.h>


Lobo::CUDABaseLayer::CUDABaseLayer()
{

}

Lobo::CUDABaseLayer::~CUDABaseLayer()
{
    free(cpu_input);
    free(cpu_output);
    cudaFree(cuda_input);
    cudaFree(cuda_output);
}

void Lobo::CUDABaseLayer::allocBuffer()
{
    cpu_input = (float*)malloc(sizeof(float)*n_input);
    cpu_output = (float*)malloc(sizeof(float)*n_output);

    cudaMalloc((void**)&cuda_input, sizeof(float) * n_input);
    cudaMalloc((void**)&cuda_output, sizeof(float) * n_output);
}