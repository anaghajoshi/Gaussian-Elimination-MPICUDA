#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include"mpi_functions.h"

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int i, j, k;
  int num;
  int ret, opt;
  double elapsed_time;
  double elapsed_exec;

  char *ifile = (void*)malloc(30);
  strncpy(ifile, "i1.dat", 30);
  
  while((opt=getopt(argc,argv,"i:"))!=-1)
  {
    switch(opt)
    {
      case 'i':
	ifile=strdup(optarg);
	break;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  elapsed_time = -MPI_Wtime();

  FILE *fh = fopen(ifile, "rb");
  if(fh != NULL)
  {
    ret = fread(&num, sizeof(int), 1, fh);
    if(ret != 1)
      printf("Improper read operation");
    fclose(fh);
  }
 
  MPI_Bcast(&num, 1, MPI_FLOAT, size-1, MPI_COMM_WORLD);

  float **matrix;

  float *x = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    x[i] = 0;

  float *y = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    y[i] = 0;

  read_rowinter(ifile, &num, &matrix, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  elapsed_exec = -MPI_Wtime();

  for(i = 0; i < num; i++)
  {
    float *pivot_row = (float *) malloc (sizeof(float)*(num-i+1));
    
    if(rank == i%size)
      pivot_row = &matrix[i/size][i];

    MPI_Bcast(pivot_row, num-i+1, MPI_FLOAT, i%size, MPI_COMM_WORLD);

    //Drive to 0 column elements
    int a;
    for(a = 0; a < (num/size); a++)
    {
      j = rank + a*size;
      if(j > i)
      {
        float t = matrix[a][i]/pivot_row[0];

        for(k = i; k < (num+1); k++)
          matrix[a][k] = matrix[a][k] - pivot_row[k - i]*t;
        matrix[a][i] = t;
      }
    }
  }

  //Forward Substitution
  for(i = 0; i < num; i++)
  {
    float temp_result;
    
    if(rank == i%size)
      temp_result = matrix[i/size][num];

    MPI_Bcast(&temp_result, 1, MPI_FLOAT, i%size, MPI_COMM_WORLD);

    y[i] = temp_result;

    for(j = 0; j < (num/size); j++)
    {
      int a;
      a = rank + j*size;
      if(a < i)
        matrix[j][num] = matrix[j][num] - y[i]*matrix[j][i];
    }
  }

  //Back Substitution
  for(i = num-1; i > -1; i--)
  {
    float temp_result;
    
    if(rank == i%size)
      temp_result = (y[i]/matrix[i/size][i]);

    MPI_Bcast(&temp_result, 1, MPI_FLOAT, i%size, MPI_COMM_WORLD);

    x[i] = temp_result;

    for(j = 0; j < (num/size); j++)
    {  
      int a;
      a = rank + j*size;
      if(a < i)
        y[a] = y[a] - x[i]*matrix[j][i];
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  //Calculating the time for the computation phase
  elapsed_exec += MPI_Wtime();
  //Calculating the time for the whole operation
  elapsed_time += MPI_Wtime();

  //Printing the time
  if(rank == 0)
  {
    printf("Total time is:%f\n", elapsed_time);
    fflush(stdout);
    printf("Computation time is:%f\n", elapsed_exec);
    fflush(stdout);
  }

  /*if(rank == 0)
  {
    for(j = 0; j < num; j++)
      printf("x[%d]: %f\n", j, x[j]);
    printf("\n");
  }*/

  free(matrix);
  free(x);
  free(y);

  MPI_Finalize();
  return 0;
}

