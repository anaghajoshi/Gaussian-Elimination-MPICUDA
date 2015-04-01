//testing
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include"functions.h"

/* Genrates matrix for specified size with random numbers between 0 and 10 */
void make_matrix(int *n, float ***B){
        float *B_1;
        int i,j;
        printf("Enter matrix size:  ");
        scanf("%d",&(*n));
        printf("\n\n");
        srand(time(NULL));
        B_1 = (float *)malloc((*n)*(*n)*sizeof(float));
        (*B) = (float **)malloc((*n)*sizeof(float *));
        for(i=0; i<(*n); i++) (*B)[i] = &B_1[i*(*n)];
        for(i=0;i<(*n);i++)
                for(j=0; j<(*n); j++) (*B)[i][j] = (float)rand()/(float)(RAND_MAX/10.0);
}

/* Reads matrix from a file */
void read_matrix(char* filename, int *n, float ***B){
        float *B_1;
        int i,test,j;
        FILE *fh=fopen(filename,"rb");
        if(fh != NULL){
                test=fread(n,sizeof(int),1,fh);
                B_1 = (float *)malloc((*n)*(*n)*sizeof(float));
                (*B) = (float **)malloc((*n)*sizeof(float *));
                for(i=0; i<(*n); i++) (*B)[i] = &B_1[i*(*n)];
                fseek(fh,4,SEEK_SET);
                for(i=0;i<(*n);i++)
                        for(j=0; j<(*n); j++) test=fread(&(*B)[i][j],sizeof(float),1,fh);
        }
        if(test!=1){
                test=1;
                fflush(stdout);
        }
        fclose(fh);
}

/* Writes matrix to a file */
void write_matrix(char* filename, int n, float **B){
        int i,j;
        FILE *fh=fopen(filename,"wb");
        if(fh!=NULL){
                fwrite(&n,sizeof(int),1,fh);
                fseek(fh,4,SEEK_SET);
                for(i=0;i<n;i++)
                        for(j=0; j<n; j++) fwrite(&B[i][j],sizeof(float),1,fh);
        }
        fclose(fh);
}

/* Prints matrix */
void print_matrix(int n, float **B){
        int i,j;
        for (i=0; i<n; i++){
                for(j=0; j<n; j++) printf("%f\t",B[i][j]);
                printf("\n\n");
        }
        printf("\n\n");
}

