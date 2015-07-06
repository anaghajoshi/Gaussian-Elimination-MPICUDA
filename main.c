#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include"functions.h"

int main(int argc, char* argv[])
{
  char *filename = (void*)malloc(30);
  strncpy(filename, "default_file.dat", 30);
  int opt;
  
  while((opt=getopt(argc,argv,"i:"))!=-1)
  {
    switch(opt)
    {
      case 'i':
	filename=strdup(optarg);
	break;
    }
  }

  int n;
  float **mat;
  
  make_matrix(&n, &mat);
  write_matrix(filename, n, mat);
  
  return 0;
}
