#include<stdio.h>
#include<stdlib.h>
#include"functions.h"
#include<unistd.h>
#include<string.h>

int main(int argc, char* argv[]){

	char *filename;
	int opt;
	while((opt=getopt(argc,argv,"i:"))!=-1){
	switch(opt){
		case 'i':
			filename=strdup(optarg);
			break;
		default:
			filename=strdup("default_file.dat");
		}
	}

	int n;
	float **mat, **doormat;
	make_matrix(&n, &mat);
	print_matrix(n, mat);
	write_matrix(filename, n, mat);
	read_matrix(filename, &n, &doormat);
	print_matrix(n, doormat);
	return 0;
}
