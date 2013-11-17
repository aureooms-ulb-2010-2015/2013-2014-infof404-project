#include <iostream>
#include <mpi.h>


int main (int argc, char *argv[]){
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double PI;

	if(rank > 0) MPI_Recv(&PI, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	else PI = 3.14159;

	printf("Hello world from process %d of %d (%f)\n", rank, size, PI);


	if(rank < size - 1){
		MPI_Send(&PI, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
	}


	MPI_Finalize();

	return 0;
}