#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include"functions.h"

//Generates matrix for specified size with random numbers between 0 and 10
void make_matrix(int *n, float ***B)
{
  float *B_1;
  int i,j;

  printf("Enter matrix size:  ");
  scanf("%d",&(*n));

  int n2 = (*n)+1;
        
  srand(time(NULL));

  B_1 = (float *)malloc((*n)*n2*sizeof(float));
  (*B) = (float **)malloc((*n)*sizeof(float *));

  for(i=0; i<(*n); i++) 
    (*B)[i] = &B_1[i*n2];

  for(i=0;i<(*n);i++)
  {
    for(j=0; j < n2; j++) 
      (*B)[i][j] = (float)rand()/(float)(RAND_MAX/10.0);
  }
}

//Reads matrix from a file
void read_matrix(char* filename, int *n, float ***B)
{
  float *B_1;
  int i,test;
  int n2 = (*n)+1;

  FILE *fh=fopen(filename,"rb");
  if(fh != NULL)
  {
    test=fread(n,sizeof(int),1,fh);

    B_1 = (float *)malloc((*n)*n2*sizeof(float));
    (*B) = (float **)malloc((*n)*sizeof(float *));

    for(i=0; i<(*n); i++) 
      (*B)[i] = &B_1[i*n2];
   
    fseek(fh,4,SEEK_SET);
    test = fread(B_1, sizeof(float), (*n)*n2, fh);
  }
  if(test!=1)
  {
    test=1;
    fflush(stdout);
  }
  fclose(fh);
}

//Writes matrix to a file
void write_matrix(char* filename, int n, float **B)
{
  int i,j;

  FILE *fh=fopen(filename,"wb");
  if(fh!=NULL)
  {
    fwrite(&n,sizeof(int),1,fh);
    fseek(fh,4,SEEK_SET);

    for(i=0;i<n;i++)
    {
      for(j=0; j<(n+1); j++)
	fwrite(&B[i][j],sizeof(float),1,fh);
    }
  }
  fclose(fh);
}

//Prints matrix
void print_matrix(int n, float **B)
{
  int i,j;
  for (i=0; i<n; i++)
  {
    for(j=0; j<n+1; j++) 
      printf("%f\t",B[i][j]);
    printf("\n");
  }
  printf("\n");
}
