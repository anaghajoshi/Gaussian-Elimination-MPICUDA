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

  /*FILE *fh = fopen(ifile, "rb");
  if(fh != NULL)
  {
    ret = fread(&num, sizeof(int), 1, fh);
    if(ret != 1)
      printf("Improper read operation");
    fclose(fh);
  }*/

  /*num = 4;

  //float **matrix;
  float matrix[num][num+1]; 
  matrix[0][0] = 7;
  matrix[0][1] = 4;
  matrix[0][2] = 3;
  matrix[0][3] = 1;
  matrix[0][4] = 4;
  matrix[1][0] = 2;
  matrix[1][1] = 1;
  matrix[1][2] = 0;
  matrix[1][3] = 4;
  matrix[1][4] = 2;
  matrix[2][0] = 8;
  matrix[2][1] = 5;
  matrix[2][2] = 7;
  matrix[2][3] = 3;
  matrix[2][4] = 1;
  matrix[3][0] = 4;
  matrix[3][1] = 2;
  matrix[3][2] = 6;
  matrix[3][3] = 8;
  matrix[3][4] = 3;*/

  num = 3;

  //float **matrix;
  float matrix[num][num+1]; 
  matrix[0][0] = 2.0;
  matrix[0][1] = 4.0;
  matrix[0][2] = -4.0;
  matrix[0][3] = 12.0;
  matrix[1][0] = 1.0;
  matrix[1][1] = -4.0;
  matrix[1][2] = 3.0;
  matrix[1][3] = -21.0;
  matrix[2][0] = -6.0;
  matrix[2][1] = -9.0;
  matrix[2][2] = 10.0;
  matrix[2][3] = -24.0;

  int *loc = (int *) malloc (sizeof(int)*num);
  for(i = 0; i < num; i++)
    loc[i] = i;

  float *x = (float *) malloc (sizeof(float)*num);
  for(i = 0; i < num; i++)
    x[i] = 0;

  //read_matrix(ifile, &num, &matrix);
  //print_matrix(num, matrix);

  exec_time1 = clock();

  for(i = 0; i < num; i++)
  {
    //find pivot row picked
    float magnitude = 0;
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
      float t = (matrix[loc[j]][i])/(matrix[loc[i]][i]);

      for(k = i; k < num+1; k++)
        matrix[loc[j]][k] = matrix[loc[j]][k] - matrix[loc[i]][k]*t;
    }
  }

  for(i = num-1; i > -1; i--)
  {
    x[i] = matrix[loc[i]][num]/matrix[loc[i]][i];
    for(j = 0; j < i; j++)
      matrix[loc[j]][num] = matrix[loc[j]][num] - x[i]*matrix[loc[j]][i];
  }

  time2 = clock();
  exec_time2 = clock();

  printf("Result array is:\n");
  for(j = 0; j < num; j++)
    printf("x[%d]:%f\n", j, x[j]);

  printf("Time:%f\n", ((double)(time2-time1))/CLOCKS_PER_SEC);
  fflush(stdout);
  printf("Computation Time:%f\n", ((double)(exec_time2-exec_time1))/CLOCKS_PER_SEC);
  fflush(stdout);

  free(matrix);
  free(loc);
  free(x);

  return 0;
}
