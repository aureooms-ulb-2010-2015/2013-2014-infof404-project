#include <iostream>
#include <mpi.h>

#include <vector>
#include <cmath>

#include "lib/eratosthene.hpp"
#include "lib/prime.hpp"
#include "lib/ppm.hpp"
#include "lib/ulam.hpp"
#include "lib/io.hpp"
#include "lib/bits_t.hpp"

int main (int argc, char *argv[]){
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}

	const size_t nth = std::stoull(argv[1]);
	const size_t last = prime::upper_bound(nth);
	size_t nth_val = 0;


	bits_t<unsigned int> prime;

	if(nth < 1) return 0;
	else if(nth == 1){
		nth_val = 2;
	}
	else if(nth == 2){
		nth_val = 3;
	}
	else{
		size_t count = last / 6;

		if(last % 6 <= 4) count = 2 * count + 1;
		else count = 2 * count + 2;

		prime.resize(count, true);

		for(size_t k = 5, l = 2; k * k < last; l += 2) {
			const size_t i = eratosthene::number_to_index_23_1(k * k);
			if(prime[i]){
				eratosthene::go_through(i, 2 * k, count, prime);
				eratosthene::go_through(i + k - l, 2 * k, count, prime);
			}

			k += 2;
			if(k * k >= last) break;
			const size_t j = eratosthene::number_to_index_23_1(k * k);
			if(prime[j]){
				eratosthene::go_through(j, 2 * k, count, prime);
				eratosthene::go_through(j + k + l, 2 * k, count, prime);
			}

			k += 4;
		}

		size_t ith = 2;

		for(size_t i = 0; i < count; ++i){
			if(prime[i]){
				++ith;
				if(ith == nth){
					nth_val = eratosthene::index_to_number_23_0(i);
					break;
				}
			}
			if(i >= count) break;
			++i;
			if(prime[i]){
				++ith;
				if(ith == nth){
					nth_val =  eratosthene::index_to_number_23_1(i);
					break;
				}
			}
		}
	}

	std::cout << nth_val << std::endl;

	// double PI;

	// if(rank > 0) MPI_Recv(&PI, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	// else PI = 3.14159;

	// printf("Hello world from process %d of %d (%f, %d)\n", rank, size, PI, last);


	// if(rank < size - 1){
	// 	MPI_Send(&PI, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
	// }


	MPI_Finalize();

	return 0;
}