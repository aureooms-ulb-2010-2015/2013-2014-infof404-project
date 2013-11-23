#ifndef OS_ALG_HPP
#define OS_ALG_HPP


#include <mpi.h>
#include "os/global.hpp"
#include "os/mpi_size_t.hpp"
#include "lib/eratosthene.hpp"

namespace os{
	namespace alg{

		template<typename F, typename A>
		inline void parallel_eratosthene_23(size_t mpi_rank, size_t mpi_size, F* forward, A& prime, size_t count, size_t last, size_t o){
			size_t k, l;
			bool left = 0, right = 1;
			bool which;
			size_t current = 0, tmp = 0;
			if(mpi_rank > current){
				do{
					MPI_Bcast(&tmp, 1, MPI_SIZE_T, 0, forward[current]);
					if(tmp > current){
						current = tmp;
						continue;
					}
					size_t i;

					MPI_Bcast(&i, 1, MPI_SIZE_T, 0, forward[current]);
					MPI_Bcast(&k, 1, MPI_SIZE_T, 0, forward[current]);
					MPI_Bcast(&l, 1, MPI_SIZE_T, 0, forward[current]);
					MPI_Bcast(&which, 1, MPI_BYTE, 0, forward[current]);


					size_t j;
					if(i < o){
						if(which == left){
							if(i + k - l < o) j = (2 * k - (o - i - k + l) % (2 * k)) % (2 * k);
							else j = i + k - l - o;
						}
						else{
							if(i + k + l < o)j = (2 * k - (o - i - k - l) % (2 * k)) % (2 * k);
							else j = i + k + l - o;
						}
						i = (2 * k - (o - i) % (2 * k)) % (2 * k);
					}
					else{
						i -= o;
						if(which == left) j = i + k - l;
						else j = i + k + l;
					}


					eratosthene::go_through(i, 2 * k, count, prime);
					eratosthene::go_through(j, 2 * k, count, prime);
				}
				while(mpi_rank > current);


				MPI_Recv(&k, 1, MPI_SIZE_T, 0, os::global::SETUP, forward[current - 1], MPI_STATUS_IGNORE);
				MPI_Recv(&l, 1, MPI_SIZE_T, 0, os::global::SETUP, forward[current - 1], MPI_STATUS_IGNORE);
			}
			else{
				k = 5, l = 2;
			}
			for(; k * k < last && l <= count; l += 2) {
				size_t i = eratosthene::number_to_index_23_1(k * k) - o;
				if(prime[l - 2 - o]){
					MPI_Bcast(&mpi_rank, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&i, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&k, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&l, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&left, 1, MPI_BYTE, 0, forward[mpi_rank]);
					if(i < count){
						eratosthene::go_through(i, 2 * k, count, prime);
						eratosthene::go_through(i + k - l, 2 * k, count, prime);
					}
				}

				k += 2;
				if(k * k >= last) break;
				size_t j = eratosthene::number_to_index_23_1(k * k) - o;
				if(prime[l - 1 - o]){
					MPI_Bcast(&mpi_rank, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&j, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&k, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&l, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
					MPI_Bcast(&right, 1, MPI_BYTE, 0, forward[mpi_rank]);
					if(j < count){
						eratosthene::go_through(j, 2 * k, count, prime);
						eratosthene::go_through(j + k + l, 2 * k, count, prime);
					}
				}

				k += 4;
			}


			if(mpi_rank + 1 < mpi_size){
				++current;
				MPI_Bcast(&current, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
				MPI_Send(&k, 1, MPI_SIZE_T, 1, os::global::SETUP, forward[mpi_rank]);
				MPI_Send(&l, 1, MPI_SIZE_T, 1, os::global::SETUP, forward[mpi_rank]);
			}
		}

	}
}



#endif // OS_ALG_HPP