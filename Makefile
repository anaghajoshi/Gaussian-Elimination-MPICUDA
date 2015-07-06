CFLAGS = -g -Wall -Wstrict-prototypes
CUFLAGS = -arch=sm_20
PROGS = main serial paraleave cuda mpicuda serial2 parallel
OBJECTS1 = main.o functions.o
OBJECTS2 = ge_serial.o functions.o
OBJECTS6 = ge_serial_wo_loc.o functions.o
OBJECTS3 = ge_paraleave.o mpi_functions.o
OBJECTS4 = ge_cuda.o cuda_functions.o
OBJECTS5 = ge_mpi_cuda.o mpi_functions.o cuda_functions2.o
OBJECTS7 = ge_parallel.o mpi_functions.o functions.o
LDFLAGS = -lm
CC = gcc
MCC = mpicc
NCC = nvcc

all: $(PROGS)

main: $(OBJECTS1)
	$(CC) $(LDFLAGS) -o main $(OBJECTS1)
serial: $(OBJECTS2)
	$(CC) $(LDFLAGS) -o serial $(OBJECTS2)
serial2: $(OBJECTS6)
	$(CC) $(LDFLAGS) -o serial2 $(OBJECTS6)
paraleave: $(OBJECTS3)
	$(MCC) $(LDFLAGS) -o paraleave $(OBJECTS3)
parallel: $(OBJECTS7)
	$(MCC) $(LDFLAGS) -o parallel $(OBJECTS7)
cuda: $(OBJECTS4)
	$(NCC) $(LDFLAGS) -o cuda $(OBJECTS4)
mpicuda: $(OBJECTS5)
	$(MCC) $(LDFLAGS) -o mpicuda ge_mpi_cuda.o mpi_functions.o -L /opt/cuda-toolkit/5.5.22/lib64 -lcudart cuda_functions2.o
functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c
mpi_functions.o: mpi_functions.c mpi_functions.h
	$(MCC) $(CFLAGS) -c mpi_functions.c
cuda_functions.o: cuda_functions.cu cuda_functions.h
	$(NCC) $(CUFLAGS) -c cuda_functions.cu
cuda_functions2.o: cuda_functions2.cu cuda_functions2.h
	$(NCC) $(CUFLAGS) -c cuda_functions2.cu
main.o:main.c
	$(CC) $(CFLAGS) -c main.c
ge_serial.o:ge_serial.c
	$(CC) $(CFLAGS) -c ge_serial.c
ge_serial_wo_loc.o:ge_serial_wo_loc.c
	$(CC) $(CFLAGS) -c ge_serial_wo_loc.c
ge_paraleave.o:ge_paraleave.c
	$(MCC) $(CFLAGS) -c ge_paraleave.c
ge_parallel.o:ge_parallel.c
	$(MCC) $(CFLAGS) -c ge_parallel.c
ge_cuda.o:ge_cuda.cu
	$(NCC) $(CUFLAGS) -c ge_cuda.cu
ge_mpi_cuda.o:ge_mpi_cuda.c
	$(MCC) $(CFLAGS) -c ge_mpi_cuda.c
clean:
	rm -f $(PROGS) *.o core*
