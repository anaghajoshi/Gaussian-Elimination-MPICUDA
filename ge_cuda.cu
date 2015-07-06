#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include "cuda_functions.h"

int main(int argc, char *argv[])
{
  int i;
  int num, matSize;
  int ret, opt;

  char *ifile = (char*)malloc(30);
  strncpy(ifile, "default_file.dat", 30);
  
  while((opt=getopt(argc,argv,"i:"))!=-1)
  {
    switch(opt)
    {
      case 'i':
	ifile=strdup(optarg);
	break;
    }
  }

  cudaEvent_t start, stop, exec_start, exec_stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventCreate(&exec_start);
  cudaEventCreate(&exec_stop);

  cudaEventRecord(start);

  FILE *fh = fopen(ifile, "rb");
  if(fh != NULL)
  {
    ret = fread(&num, sizeof(int), 1, fh);
    if(ret != 1)
      printf("Improper read operation");
    fclose(fh);
  }

  float *h_A;  //host matrix

  int *h_loc = (int *) malloc (sizeof(int)*num);
  for(i = 0; i < num; i++)
    h_loc[i] = i;

  float *h_x = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    h_x[i] = 0;
  
  float *h_y = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    h_y[i] = 0;
  
  matSize = num*(num+1);

  read_matrix(ifile, &num, &h_A);

  cudaEventRecord(start);

  //Allocating memory in device
  gpuAlloc(matSize, num);

  //Copy Data to Device
  gpuMemLoad(matSize, num, &h_A, &h_x, &h_loc);

  for(i = 0; i < num; i++)
  {
    //Kernel Call 1
    gpuConUp(num, i);
  }

 for(i = 0; i < num; i++)
  {
    //Kernel Call 2
    gpuFwdSubs(num, i);
  }

  for(i = num-1; i > -1; i--)
  {
    //Kernel Call 3
    gpuBackSubs(num, i);
  }

  gpuMemStore(matSize, num, &h_A, &h_x, &h_y);

  cudaEventRecord(stop);
  cudaEventRecord(exec_stop);
//  printf("LU is:\n");
//  print_matrix(num,h_A);

/*  
printf("Result Y  is:\n");
  for(i = 0; i < num; i++)
    printf("y[%d]:%f\n", i, h_y[i]);

  printf("Result array is:\n");
  for(i = 0; i < num; i++)
    printf("x[%d]:%f\n", i, h_x[i]);
*/
  cudaEventSynchronize(stop);
  cudaEventSynchronize(exec_stop);
  float milliseconds = 0;
  float execmilliseconds = 0;
  cudaEventElapsedTime(&milliseconds, start, stop);
  cudaEventElapsedTime(&execmilliseconds, exec_start, exec_stop);
  printf("Time:%f\n", milliseconds);
  printf("Computation Time:%f\n", execmilliseconds);

  free(h_A);
  free(h_loc);
  free(h_x);

  //Free Device memory
  gpuFree();

  return 0;
}
