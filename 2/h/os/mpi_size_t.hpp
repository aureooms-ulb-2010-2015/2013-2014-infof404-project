#ifndef OS_MPI_SIZE_T_HPP
#define OS_MPI_SIZE_T_HPP

#include <mpi.h>

inline MPI_Datatype GET_MPI_SIZE_T(){
	if(sizeof(size_t) == 1) return MPI_INTEGER1;
	if(sizeof(size_t) == 2) return MPI_INTEGER2;
	if(sizeof(size_t) == 4) return MPI_INTEGER4;
	else return MPI_INTEGER8;
}

const MPI_Datatype MPI_SIZE_T = GET_MPI_SIZE_T();



#endif // OS_MPI_SIZE_T_HPP