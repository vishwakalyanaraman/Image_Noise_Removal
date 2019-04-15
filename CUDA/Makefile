all:
	gcc -c -o ppmFile.o ppmFile.c
	nvcc --device-c -o filter.o filter.cu
	nvcc -o imageFilters.x ppmFile.o filter.o

clean:
	rm -f imageFilters.x filter.o ppmFile.o
