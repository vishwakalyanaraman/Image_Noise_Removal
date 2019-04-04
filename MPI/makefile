CFLAGS=-g

CC = mpicc

PROGRAM_NAME= MPI_Executable
OBJS = main.o readwritePPM.o processImage.o

$(PROGRAM_NAME): $(OBJS)
	$(CC) -o $@ $?

clean:
	rm  *.o $(PROGRAM_NAME)
