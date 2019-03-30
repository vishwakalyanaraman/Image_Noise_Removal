#include <stdlib.h>
#include "a1.h"
#include <mpi.h>
#include <stdio.h>
int main(int argc, char** argv){
	
	// Image and Filtered Image
	RGB *image, *filteredImage;
	
	int global_width, global_height;
	int width, height, max;
	Dimension *my_dim;
	
	int rank, size;
	
	int dest, source;
	int offset;
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int tag = 0;
	int windowSize = atoi(argv[3]);
	
	// Process 0 parses the PPM file and distributes size attributes
	if (rank == 0){
		image = readPPM(argv[1], &global_width, &global_height, &max);
		offset = 0;
		for (dest = 1; dest < size; dest++){
			int rowsToCompute = global_height/size;
			if (dest < global_height % size)
				rowsToCompute++;
			offset += rowsToCompute;
			
			if (dest == size-1)
				rowsToCompute += (windowSize/2);

			else
				rowsToCompute += (windowSize/2)*2;

			Dimension *dim = (Dimension*) malloc(sizeof(Dimension));
			dim->width = global_width;
			dim->height = rowsToCompute;
			MPI_Send(dim, 2, MPI_INT, dest, tag, MPI_COMM_WORLD);
		}
		
		// Set own size attributes
		width = global_width;
		height = global_height / size;
		if (global_height % size != 0)
			height += 1;

		height += (windowSize/2);
	}

	// Receive size attributes
	else {	
		my_dim = (Dimension*)malloc(sizeof(Dimension));
		MPI_Recv(my_dim, 2, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		width = my_dim->width;
		height = my_dim->height;
		image = (RGB*)malloc(height*width*sizeof(RGB));
	}

	
	// Process 0 distributes rows
	if (rank == 0){
		offset = global_height / size;
		if (global_height % size != 0){
			offset += 1;
		}
		offset += (windowSize/2);
		for (dest = 1; dest < size; dest++){
			int rowsToCompute = global_height/size;
			if (dest < global_height % size)
				rowsToCompute++;

			if (dest == size-1)
                rowsToCompute += (windowSize/2);
            else
                rowsToCompute += (windowSize/2)*2;

			if (dest == size-1)
				offset -= (windowSize/2)*2;
			else
				offset -= (windowSize/2)*2;
		
			
			MPI_Send(image + offset*width, 3*rowsToCompute*width, MPI_UNSIGNED_CHAR, dest, tag, MPI_COMM_WORLD);
			offset += rowsToCompute;
		}		
	}
	
	// Receive rows
	else{

		MPI_Recv(image, height*width*3, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, &status);
	}
	
	
	// Process image    argv[4] decides Mean or Median Filtering	
	filteredImage = processImage(width, height, image, windowSize, argv[4]);
	
	
	// Send processed data back to P0
	if (rank != 0 && rank != size-1)
		MPI_Send(image + (windowSize/2)*width, (height-2*(windowSize/2))*width*3, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);

	else if (rank == size-1)
		MPI_Send(image + (windowSize/2)*width, (height-(windowSize/2))*width*3, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);

	else{
		offset = global_height / size;
                if (global_height % size != 0){
                        offset += 1;
                }
		for (source = 1; source < size; source++){
			int rowsToCompute = global_height/size;
			if (source < global_height % size)
				rowsToCompute++;

			MPI_Recv(image+offset*width, rowsToCompute*width*3, MPI_UNSIGNED_CHAR, source, tag, MPI_COMM_WORLD, &status);
			
			offset += rowsToCompute;
		}

	}

	// Process 0 writes processed PPM file  argv[2] decides output.ppm file
	if (rank == 0){
		writePPM(argv[2], global_width, global_height, max, filteredImage);
		free(image);
    }
    
	MPI_Finalize();
	return(0);
}
