#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include "cuda_functions2.h"
/*This is for the hybrid implementation*/

#define BLOCK_SIZE 8

float *d_A;    //device matrix
float *d_pivot; //device array
int *d_loc;    //device array
float *d_x;    //device array
float *d_y;   //device array

dim3 threadBlock(BLOCK_SIZE,BLOCK_SIZE);
dim3 grid(1,1);	

__global__ void con_up(float* A, float* d_pivot, int* loc, int N, int rank, int size, int row_bound)
{
  int row = blockIdx.y*blockDim.y + threadIdx.y;
  int col = blockIdx.x*blockDim.x + threadIdx.x;
  int col_bound = rank + row*size;  

  if( (row < N/size) && (col >= row_bound) && (col_bound > row_bound))
  {
    float t = A[row*(N+1) + row_bound]/d_pivot[0];

    A[row*(N+1) + col] = A[row*(N+1) + col] - d_pivot[col - row_bound]*t;
    if(col == N-1)
      A[row*(N+1) + (col+1)] = A[row*(N+1) + (col+1)] - d_pivot[(col+1) - row_bound]*t;
    A[row*(N+1)+row_bound] = t;
 }
}

__global__ void reduce1(float* A, int N, int row_bound, int rank, int size,float y)
{
  int row = blockIdx.y*blockDim.y + threadIdx.y;

  int a = rank+row*size;

  if(a < row_bound)
    A[row*(N+1) + N] = A[row*(N+1) + N] - y*A[row*(N+1) + row_bound];
  
}

__global__ void reduce2(float* A, int N, int row_bound, int rank, int size, float x, float* y)
{
  int row = blockIdx.y*blockDim.y + threadIdx.y;
  int a =rank + row*size;

  if(row < row_bound)
    y[a] = y[a] - x*A[row*(N+1) + row_bound];
}


void gpuAlloc(int x, int y)
{
  cudaMalloc((void **)&d_A, x*sizeof(float));
  cudaMalloc((void **)&d_loc, y*sizeof(int));
  cudaMalloc((void **)&d_x, y*sizeof(float));
  cudaMalloc((void **)&d_y, y*sizeof(float));
  dim3 grid(y/BLOCK_SIZE, y/BLOCK_SIZE);	
}

void gpuFree(void)
{
  cudaFree(d_A);
  cudaFree(d_x);
  cudaFree(d_y);
  cudaFree(d_loc);
  cudaFree(d_pivot);
}

void gpuLoopAlloc(int y)
{
  cudaMalloc((void **)&d_pivot, y*sizeof(float));
}

void gpuMemLoad(int x, int y, float** h_A, float** h_x, int** h_loc)
{
  cudaMemcpy(d_A, *h_A, x*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_loc, *h_loc, y*sizeof(int), cudaMemcpyHostToDevice);
}

void gpuMemStore(int x, int y, float** h_A, float** h_x, int** h_loc, float** h_y)
{
  cudaMemcpy(*h_A, d_A, x*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(*h_loc, d_loc, y*sizeof(int), cudaMemcpyDeviceToHost);
  cudaMemcpy(*h_x, d_x, y*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(*h_y, d_y, y*sizeof(float), cudaMemcpyDeviceToHost);
}

void gpuLoopLoad(int x, int y, float** pivot, int ** h_loc)
{
  cudaMemcpy(d_pivot, *pivot, x*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_loc, *h_loc, y*sizeof(int), cudaMemcpyHostToDevice);
}

void gpuLoopStore(int x, float** h_A)
{
  cudaMemcpy(*h_A, d_A, x*sizeof(float), cudaMemcpyDeviceToHost);
}

void gpuLoopStore2(int x, float** y)
{
  cudaMemcpy(*y, d_y, x*sizeof(float), cudaMemcpyDeviceToHost);
}

void gpuConUp(int i, int num, int rank, int size)
{
  con_up<<<grid, threadBlock>>>(d_A, d_pivot, d_loc, num, rank, size, i);
}

void gpuBackSubs(int i, int x, int rank, int size, float temp_result, float** y)
{
  cudaMemcpy(d_y, *y, x*sizeof(float), cudaMemcpyHostToDevice);
  reduce2<<<grid, threadBlock>>>(d_A, x, i, rank, size, temp_result, d_y);
}

void gpuFwdSubs(int i, int x, int rank, int size, float temp_result)
{
  reduce1<<<grid, threadBlock>>>(d_A, x, i, rank, size, temp_result);
}

