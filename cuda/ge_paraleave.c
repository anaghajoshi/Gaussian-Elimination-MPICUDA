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

  struct
  {
    float value;
    int index;
  }global, local;

  char *ifile = (void*)malloc(30);
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

  int *loc = (int *) malloc (sizeof(int)*num);
  for(i = 0; i < num; i++)
    loc[i] = 0;

  float *x = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    x[i] = 0;

  int *gloc = (int *) malloc (sizeof(int)*num);

  read_rowinter(ifile, &num, &matrix, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  elapsed_exec = -MPI_Wtime();

  //print2(matrix, rank, size, num, MPI_COMM_WORLD);
  for(i = 0; i < num; i++)
  {
    float *pivot_row = (float *) malloc (sizeof(float)*(num-i+1));

    local.value = 0;
    local.index = 0;

    int a, picked;
    for(a = 0; a < (num/size); a++)
    {
      j = rank + a*size;
      if(( fabs(matrix[a][i]) > local.value ) && (loc[j] == 0))
      {
        local.value = fabs(matrix[a][i]);
        local.index = j;
        picked = a;
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Allreduce(&local, &global, 1, MPI_FLOAT_INT, MPI_MAXLOC, MPI_COMM_WORLD);

    loc[global.index] = 1;
    gloc[i] = global.index;

    if(rank == global.index%size)
      pivot_row = &matrix[picked][i];

    MPI_Bcast(pivot_row, num-i+1, MPI_FLOAT, global.index%size, MPI_COMM_WORLD);

    //Drive to 0 column elements
    for(a = 0; a < (num/size); a++)
    {
      j = rank + a*size;
      if(loc[j] == 0)
      {
        float t = matrix[a][i]/pivot_row[0];

        for(k = i; k < (num+1); k++)
          matrix[a][k] = matrix[a][k] - pivot_row[k - i]*t;
      }
    }
    //if(rank == 0)
      //printf("\n");
    //print2(matrix, rank, size, num, MPI_COMM_WORLD);
  }

  //Back Substitution
  for(i = num-1; i >= 0; i--)
  {
    float temp_result;
    
    if(rank == gloc[i]%size)
      temp_result = (matrix[gloc[i]/size][num]/matrix[gloc[i]/size][i]);

    MPI_Bcast(&temp_result, 1, MPI_FLOAT, gloc[i]%size, MPI_COMM_WORLD);

    x[i] = temp_result;

    for(j = 0; j < (num/size); j++)
    {
      matrix[j][num] = matrix[j][num] - x[i]*matrix[j][i];
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
  free(gloc);
  free(loc);
  free(x);

  MPI_Finalize();
  return 0;
}
