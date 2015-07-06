#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include"functions.h"

int main(int argc, char *argv[])
{
  int i,j;
  int num;
  int ret, opt;
  clock_t time1, time2;
  clock_t exec_time1, exec_time2;

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

  time1 = clock();

  FILE *fh = fopen(ifile, "rb");
  if(fh != NULL)
  {
    ret = fread(&num, sizeof(int), 1, fh);
    if(ret != 1)
      printf("Improper read operation");
    fclose(fh);
  }

  float **matrix;

  float *x = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    x[i] = 0;

  float *y = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    y[i] = 0;

  read_matrix(ifile, &num, &matrix);
  //print_matrix(num, matrix);

  //printf("\n");

  exec_time1 = clock();

  for(i = 0; i < num; i++)
  {
    //drive to 0 column i elements in rows i+1 through n-1
    for(j = i+1; j < num; j++)
    {
      float t = (matrix[j][i])/(matrix[i][i]);

      int k;
      for(k = i; k < num+1; k++)
        matrix[j][k] = matrix[j][k] - matrix[i][k]*t;
      matrix[j][i] = t;
      //print_matrix(num, matrix);
    }
  }
//printf("LU is: \n");
//print_matrix(num, matrix);
 

 for(i=0;i<num;i++)
  {
    y[i] = matrix[i][num];
    for(j=0;j<i;j++) 
      matrix[j][num] = matrix[j][num] - y[i]*matrix[j][i]; 	
  }

 

  for(i=(num-1);i>=0;--i)
  {
    x[i] = y[i]/matrix[i][i];
    for(j=0;j<i;j++) 
      y[j] = y[j] - x[i]*matrix[j][i];
  }
/*
printf("AFTER FS Y array is:\n");
  for(j = 0; j < num; j++)
    printf("y[%d]:%f\n", j, y[j]);
*/ 
 time2 = clock();
  exec_time2 = clock();
/*
  printf("Result array is:\n");
  for(j = 0; j < num; j++)
    printf("x[%d]:%f\n", j, x[j]);
*/
  printf("Time:%f\n", ((double)(time2-time1))/CLOCKS_PER_SEC);
  fflush(stdout);
  printf("Computation Time:%f\n", ((double)(exec_time2-exec_time1))/CLOCKS_PER_SEC);
  fflush(stdout);

  free(matrix);
  free(x);

  return 0;
}
