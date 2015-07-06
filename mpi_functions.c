#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_rowblock(char *filename, int *num, float ***B, MPI_Comm comm)
{
  MPI_Status status;
  float *B_1;
  int i, test;
  int rank, size;
  
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);
  
  int n2 = (*num)+1;
  int local_row = (*num)/size;

  FILE *fh=fopen(filename,"rb");
  if(fh != NULL)
  {
    B_1 = (float *)malloc(local_row*n2*sizeof(float));
    (*B) = (float **)malloc(local_row*sizeof(float *));

    for(i=0; i<local_row; i++) 
      (*B)[i] = &B_1[i*n2];

    if(rank == size-1)
    {
      fseek(fh,4,SEEK_SET);
      int k;
      for(k = 0; k < size-1; k++)
      {
	test = fread(B_1, sizeof(float), local_row*n2, fh);
	MPI_Send(B_1, local_row*n2, MPI_FLOAT, k, 0, comm);
      }
      test = fread(B_1, sizeof(float), local_row*n2, fh);
    }
    else
    {
      MPI_Recv(B_1, local_row*n2, MPI_FLOAT, size-1, 0, comm, &status);
    }

    if(test!=1)
    {
      test=1;
      fflush(stdout);
    }
  }
  fclose(fh);
}

void read_rowinter(char *filename, int *num, float ***B, MPI_Comm comm)
{
  MPI_Status status;
  float *B_1;
  int i, test;
  int rank, size;
	
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  int n2 = (*num)+1;
  int local_row = (*num)/size;

  FILE *fh=fopen(filename,"rb");
  if(fh != NULL)
  {
    B_1 = (float *)malloc(local_row*n2*sizeof(float));
    (*B) = (float **)malloc(local_row*sizeof(float *));

    for(i=0; i<local_row; i++) (*B)[i] = &B_1[i*n2];
    if(rank == size-1)
    {
      fseek(fh,4,SEEK_SET);

      float *A_1 = (float *)malloc((*num)*n2*sizeof(float));
      float **A = (float **)malloc((*num)*sizeof(float *));
      
      for(i=0; i<(*num); i++) 
        A[i] = &A_1[i*n2];

      test = fread(A_1, sizeof(float), (*num)*n2, fh);

      int a, temp_row;
      temp_row = 0;

      for(a = 0; a < local_row; a++)
      {
	int k;
	for(k = 0; k < size-1; k++)
	MPI_Send(&A_1[(temp_row+k)*n2], n2, MPI_FLOAT, k, 0, comm);
	(*B)[a] = &A_1[(temp_row+size-1)*n2];
	temp_row = temp_row+size;
      }
    }
    else
    {
      int a;
      for(a = 0; a < local_row; a++)
	MPI_Recv(&B_1[a*n2], n2, MPI_FLOAT, size-1, 0, comm, &status);
    }

    if(test!=1)
    {
      test=1;
      fflush(stdout);
    }
  }
  fclose(fh);
}

void read_rowintercuda(char *filename, int *num, float **B_1, MPI_Comm comm)
{
  MPI_Status status;
  //float **B;
  int i, test;
  int rank, size;
	
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  int n2 = (*num)+1;
  int local_row = (*num)/size;

  FILE *fh=fopen(filename,"rb");
  if(fh != NULL)
  {
    (*B_1) = (float *)malloc(local_row*n2*sizeof(float));
    //B = (float **)malloc(local_row*sizeof(float *));

    //for(i=0; i<local_row; i++)
      //B[i] = &(*B_1)[i*n2];

    if(rank == size-1)
    {
      fseek(fh,4,SEEK_SET);

      float *A_1 = (float *)malloc((*num)*n2*sizeof(float));
      float **A = (float **)malloc((*num)*sizeof(float *));
      
      for(i=0; i<(*num); i++) 
        A[i] = &A_1[i*n2];

      test = fread(A_1, sizeof(float), (*num)*n2, fh);

      int a, temp_row;
      temp_row = 0;

      for(a = 0; a < local_row; a++)
      {
	int k;
	for(k = 0; k < size-1; k++)
	  MPI_Send(&A_1[(temp_row+k)*n2], n2, MPI_FLOAT, k, 0, comm);
        for(k = 0; k < n2; k++)
	  (*B_1)[a*n2+k] = A_1[(temp_row+size-1)*n2+k];
	temp_row = temp_row+size;
      }
    }
    else
    {
      int a;
      for(a = 0; a < local_row; a++)
	MPI_Recv(&(*B_1)[a*n2], n2, MPI_FLOAT, size-1, 0, comm, &status);
    }

    if(test!=1)
    {
      test=1;
      fflush(stdout);
    }
  }
  fclose(fh);
}

void print(float* matrix, int rank, int size, int num, MPI_Comm comm)
{
  int i, j, k;
  for(k = 0; k < size; k++)
  {
    if(rank == k)
    {
      for(i = 0; i < (num/size); i++)
      {
        for(j = 0; j < (num+1); j++)
          printf("%f\t", matrix[i*(num+1)+j]);
        printf("\n");
      }
    }
    MPI_Barrier(comm);
  }
}

void print2(float** matrix, int rank, int size, int num, MPI_Comm comm)
{
  int i, j, k;
  for(k = 0; k < size; k++)
  {
    if(rank == k)
    {
      for(i = 0; i < (num/size); i++)
      {
        for(j = 0; j < (num+1); j++)
          printf("%f\t", matrix[i][j]);
        printf("\n");
      }
    }
    MPI_Barrier(comm);
  }
}

