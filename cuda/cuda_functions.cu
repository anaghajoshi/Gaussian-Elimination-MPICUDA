#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include "cuda_functions.h"
/*This is for the CUDA only code implemented by using the exact algorithm described by the book*/

#define BLOCK_SIZE 8

float *d_A;    //device matrix
float *d_x;    //device array
int *d_loc;    //device array
float *d_y;   //device array

dim3 threadBlock(BLOCK_SIZE,BLOCK_SIZE);
dim3 grid(1,1);	

//Reads matrix from a file
void read_matrix(char* filename, int *n, float **B_1)
{
  float **B;
  int i,test;
  int n2 = (*n)+1;

  FILE *fh=fopen(filename,"rb");
  if(fh != NULL)
  {
    test=fread(n,sizeof(int),1,fh);
    (*B_1) = (float *)malloc((*n)*n2*sizeof(float));
    B = (float **)malloc((*n)*sizeof(float *));

    for(i=0; i<(*n); i++) 
      B[i] = &(*B_1)[i*n2];

    fseek(fh,4,SEEK_SET);
      test = fread((*B_1), sizeof(float), (*n)*n2, fh);
  }

  if(test!=1)
  {
    test=1;
    fflush(stdout);
  }
  fclose(fh);
}

//Prints matrix
void print_matrix(int n, float *B)
{
  int i,j;
  for (i=0; i<n; i++)
  {
    for(j=0; j<n+1; j++) 
      printf("%f\t",B[i*(n+1)+j]);
    printf("\n");
  }
  printf("\n");
}

__global__ void print(float* A, int N)
{
  int iter = threadIdx.x;

  if(iter == 1)
  {
    int i,j;
    for (i=0; i<N; i++)
    {
      for(j=0; j<N+1; j++) 
        printf("%f\t",A[i*(N+1)+j]);
      printf("\n");
    }
  }
  printf("\n");
}

__global__ void find_max(float* A, int* loc, int N, int row_bound)
{
  //find pivot row picked
  float magnitude = 0;
  int picked, j;

  int iter = threadIdx.x;

  if(iter == 1)
  {
    for(j = row_bound; j < N; j++)
    {
      if( abs(A[loc[j]*(N+1) + row_bound]) > magnitude)
      {
        magnitude = abs(A[loc[j]*(N+1) + row_bound]);
        picked = j;
      }
    }
    int temp = loc[row_bound];
    loc[row_bound] = loc[picked];
    loc[picked] = temp;
  }
}

__global__ void con_up(float* A, int N, int row_bound)
{
  int row = blockIdx.y*blockDim.y + threadIdx.y;
  int col = blockIdx.x*blockDim.x + threadIdx.x;

  if( (row > row_bound) && (col >= row_bound) )
  {
    float t = A[row*(N+1) + row_bound]/A[row_bound*(N+1) + row_bound];
    //printf("Num:%f Den:%f\n", A[row*(N+1) + row_bound], A[row_bound*(N+1) + row_bound]);
    //printf("\n");

    //if(row == (row_bound+1))
    //  printf("%f\n", t);

    A[row*(N+1) + col] = A[row*(N+1) + col] - A[row_bound*(N+1) + col]*t;
    
if(col == N-1)
      A[row*(N+1) + (col+1)] = A[row*(N+1) + (col+1)] - A[row_bound*(N+1) + (col+1)]*t;
    A[row*(N+1)+row_bound] = t;
  }
}

__global__ void reduce1(float* A, float* x, float* y, int N, int row_bound)
{
  y[row_bound] = A[row_bound*(N+1) + N];
int row = blockIdx.y*blockDim.y + threadIdx.y;
if(row < row_bound)
    A[row*(N+1) + N] = A[row*(N+1) + N] - y[row_bound]*A[row*(N+1) + row_bound];
 
}

__global__ void reduce2(float* A, float* x, float* y, int N, int row_bound)
{
  x[row_bound] = y[row_bound]/A[row_bound*(N+1) + row_bound];
  //printf("%f\n", x[row_bound]);

  int row = blockIdx.y*blockDim.y + threadIdx.y;
  //printf("%d\n", row);

  if(row < row_bound)
    y[row] = y[row] - x[row_bound]*A[row*(N+1) + row_bound];
}


__global__ void printvf(float* A, int N)
{
  int iter = threadIdx.x;

  if(iter == 1)
  {
    int i;
    for (i=0; i<N; i++)
      printf("%f\n",A[i]);
  }
}

__global__ void printv(int* A, int N)
{
  int iter = threadIdx.x;

  if(iter == 1)
  {
    int i;
    for (i=0; i<N; i++)
      printf("%d\n",A[i]);
  }
  printf("\n");
}

void gpuAlloc(int x, int y)
{
  cudaMalloc((void **)&d_A, x*sizeof(float));
  cudaMalloc((void **)&d_x, y*sizeof(float));
  cudaMalloc((void **)&d_y, y*sizeof(float));
  cudaMalloc((void **)&d_loc, y*sizeof(int));
  dim3 grid(y/BLOCK_SIZE, y/BLOCK_SIZE);	
}

void gpuFree(void)
{
  cudaFree(d_A);
  cudaFree(d_x);
  cudaFree(d_y);
  cudaFree(d_loc);
}

void gpuMemLoad(int x, int y, float** h_A, float** h_x, int** h_loc)
{
  cudaMemcpy(d_A, *h_A, x*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_x, *h_x, y*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_loc, *h_loc, y*sizeof(int), cudaMemcpyHostToDevice);
}

void gpuMemStore(int x, int y, float** h_A, float** h_x, float** h_y)
{
  cudaMemcpy(*h_A, d_A, x*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(*h_x, d_x, y*sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(*h_y, d_y, y*sizeof(float), cudaMemcpyDeviceToHost);
}

void gpuLoopStore(int x, float** h_A)
{
  cudaMemcpy(*h_A, d_A, x*sizeof(float), cudaMemcpyDeviceToHost);
}

void gpuFindMax(int x, int i)
{
  find_max<<<1,2>>>(d_A, d_loc, x, i);
}

void gpuConUp(int x, int i)
{
  con_up<<<grid, threadBlock>>>(d_A, x, i);
}

void gpuBackSubs(int x, int i)
{
  reduce2<<<grid, threadBlock>>>(d_A, d_x, d_y,x, i);
}
void gpuFwdSubs(int x, int i)
{
  reduce1<<<grid, threadBlock>>>(d_A, d_x, d_y, x, i);
}

