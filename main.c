#include <stdio.h>
#include "a1.h"
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    //Image and Filtered Image
    RGB *image, *filteredImage;

    int global_height, global_width;
    int width, height, max;

    Dimension *my_dim;

    int rank, size;

    int dest, source;
    int offset;


    //MPI Stuff
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    int windowSize = atoi(argv[3]);
    int tag = 0;

    if(rank == 0){
        image = readPPM(argv[1], &global_width, &global_height, &max);
        offset = 0;
        
        for(dest = 1; dest < size; dest++){
            int rowsToCompute = global_height / size;
            if(dest < global_height % size)
                rowsToCompute++;
            offset += rowsToCompute;

            //Making sure borders are calculated properly
            
            if(dest == size - 1)
                rowsToCompute += (windowSize / 2);
            else
                rowsToCompute += (windowSize / 2) * 2;

            Dimension *dim = (Dimension *) malloc(sizeof(Dimension));

            //Allocate Height and Rows accordingly and send to each process
            dim->width = global_width;
            dim->height = rowsToCompute;
            MPI_Send(dim, 2, MPI_INT, dest, tag, MPI_COMM_WORLD);
        }


        //Set own size attributes
        width = global_width;
        height = global_height / size;
        if(global_height % size != 0)
            height++;
        height += (windowSize / 2);
    }

    //Receive size attributes
    else{
        my_dim = (Dimension *) malloc(sizeof(Dimension));

        MPI_Recv(my_dim, 2, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        width = my_dim->width;
        height = my_dim->height;

        image = (RGB*) malloc(sizeof(RGB) * height * width);
    }

    //Process 0 distributes rows
    if(rank == 0){
        offset = global_height / size;
        if(global_height % size != 0)
            offset += 1;

        offset += (windowSize/2);
        for(dest = 1; dest < size; dest++){
            int rowsToCompute = global_height/size;
            if(dest < global_height % size)
                rowsToCompute++;

            if(dest == size-1)
                rowsToCompute += (windowSize/2);
            else
                rowsToCompute += (windowSize/2)*2;
        
            if(dest == size-1)
                offset -= (windowSize/2)*2;
            else
                offset -= (windowSize/2)*2;

            MPI_Send(image+offset*width, 3*rowsToCompute*width, MPI_UNSIGNED_CHAR, dest, tag, MPI_COMM_WORLD);

            offset += rowsToCompute;
        }
    }

    //Receive Rows
    
    else
        MPI_Recv(image, height*width*3, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, &status);

    // Process Image  (argv[4] value)

    filteredImage = processImage(width, height, image, windowSize, argv[4]);

    //Send Processed Image to Process0
    
    if(rank != 0 && rank != size-1)
        MPI_Send(image+(windowSize/2)*width, (height-2*(windowSize/2))*width*3, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
    
    else if(rank == size-1)
        MPI_Send(image+(windowSize/2)*width, (height-(windowSize/2))*width*3, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
        
    else{
        offset = global_height / size;
        if(global_height % size != 0)
            offset++;
        for(source = 1; source < size; source++){
            int rowsToCompute = global_height/size;
            if(source < global_height/size)
                rowsToCompute++;

            MPI_Recv(image+offset*width, rowsToCompute*width*3, MPI_UNSIGNED_CHAR, source, tag, MPI_COMM_WORLD, &status);

            offset += rowsToCompute;
        }
    }

    //Process0 Writes to Output PPM File

    if(rank == 0){
        writePPM(argv[2], &global_width, &global_height, max, filteredImage);
        free(image);
    }

    MPI_Finalize();
    return(0);
}


