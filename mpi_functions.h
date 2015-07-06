void read_rowblock(char *, int *, float ***, MPI_Comm);
void read_rowinter(char *, int *, float ***, MPI_Comm);
void read_rowintercuda(char *, int *, float **, MPI_Comm);
void print(float* , int , int , int , MPI_Comm);
void print2(float** , int , int , int , MPI_Comm);

