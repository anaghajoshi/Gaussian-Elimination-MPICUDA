#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"functions.h"

int main(int argc, char *argv[])
{
  int i,j;
  int num;

  /*int num = 8;
  struct{
  int value;
  int index;
  }global, local;*/

  int ret;

  char *ifile = (void*)malloc(30);
  strncpy(ifile, "input1.dat", 30);

  FILE *fh = fopen(ifile, "rb");
  if(fh != NULL)
  {
    ret = fread(&num, sizeof(int), 1, fh);
    if(ret != 1)
      printf("Improper read operation");
    fclose(fh);
  }

  double **matrix = (double **) malloc(sizeof(double *)*num);
  for(i = 0; i < num; i++)
    matrix[i] = (double *) malloc(sizeof(double)*(num+1));

  int *loc = (int *) malloc (sizeof(int)*num);
  for(i = 0; i < num; i++)
    loc[i] = i;

  double *x = (double *) malloc (sizeof(double)*num);
  for(i = 0; i < num; i++)
    x[i] = 0;

  read_matrix(ifile, &num, &matrix);
  printf("Read matrix\n");
  fflush(stdout);
  print_matrix(num, matrix);

  printf("Printing Loc vector");
  for(i = 0; i < num; i++)
    printf(" %d", loc[i]);
  printf("\n");
  fflush(stdout); 

  for(i = 0; i < num; i++)
  {
    //find pivot row picked
    double magnitude = 0;
    int picked;

    for(j = i; j < num; j++)
    {
      if(abs(matrix[loc[j]][i]) > magnitude)
      {
        magnitude = abs(matrix[loc[j]][i]);
        picked = j;
      }
    }
    int temp = loc[i];
    loc[i] = loc[picked];
    loc[picked] = temp;

    int k;

    //drive to 0 column loc[i] elements in rows i+1 through n-1
    for(j = i+1; j < num; j++)
    {
      double t = (matrix[loc[j]][i])/(matrix[loc[i]][i]);
      for(k = i; k < num+2; k++)
        matrix[loc[j]][k] = matrix[loc[j]][k] - matrix[loc[i]][k]*t;
    }
  }

  printf("Before backsubstitution\n");
  print_matrix(num, matrix);
  printf("Printing Loc vector");
  fflush(stdout);
  for(i = 0; i < num; i++)
    printf(" %d", loc[i]);
  printf("\n");
  fflush(stdout); 

  for(i = num-1; i > -1; i--)
  {
    x[i] = matrix[loc[i]][num]/matrix[loc[i]][i];
    for(j = 0; j < i; j++)
      matrix[loc[j]][num] = matrix[loc[j]][num] - x[i]*matrix[loc[j]][i];
  }

  printf("After backsubstitution\n");
  print_matrix(num, matrix);
  printf("Printing Loc vector");
  fflush(stdout);
  for(i = 0; i < num; i++)
    printf(" %d", loc[i]);
  printf("\n");
  fflush(stdout); 

  return 0;
}
