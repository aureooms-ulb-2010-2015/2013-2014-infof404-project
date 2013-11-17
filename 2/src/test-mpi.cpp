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

const int SETUP = 0;

int main (int argc, char *argv[]){
	MPI_Init(&argc, &argv);

	int mpi_rank, mpi_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);


	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}

	const size_t nth = std::stoull(argv[1]);
	const size_t last = prime::upper_bound(nth);
	size_t nth_val = 0;


	std::vector<bool> prime;

	if(nth == 0) last = 0;
	else if(nth == 1) last = 4;
	else if(nth == 2) last = 4;
	else{
		size_t count = last / 6;

		if(last % 6 <= 4) count = 2 * count + 1;
		else count = 2 * count + 2;

		size_t r = count % mpi_size;
		count /= mpi_size;


		prime.resize(count, true);
		size_t k, l, o;
		if(r > mpi_rank){
			++count;
			o = mpi_rank * count;
		}
		else{
			o = r * (count + 1) + (mpi_rank - r) * count;
		}
		const bool left = 0, right = 1;
		bool which;
		int current = 0, tmp = 0;
		if(mpi_rank > current){
			do{
				MPI_Bcast(&tmp, 1, MPI_INT, current, MPI_COMM_WORLD);
				if(tmp > current){
					current = tmp;
					continue;
				}
				size_t i;

				MPI_Bcast(&i, 1, MPI_UNSIGNED, current, MPI_COMM_WORLD);
				MPI_Bcast(&k, 1, MPI_UNSIGNED, current, MPI_COMM_WORLD);
				MPI_Bcast(&l, 1, MPI_UNSIGNED, current, MPI_COMM_WORLD);
				MPI_Bcast(&which, 1, MPI_BOOL, current, MPI_COMM_WORLD);


				i = 2 * k - [(o - i) % (2 * k)];

				if(which == left){
					eratosthene::go_through(i, 2 * k, count, prime);
					eratosthene::go_through(i + k - l, 2 * k, count, prime);
				}
				else{
					eratosthene::go_through(i, 2 * k, count, prime);
					eratosthene::go_through(i + k + l, 2 * k, count, prime);
				}
			}
			while(mpi_rank > current);

			MPI_Recv(&k, 1, MPI_UNSIGNED, mpi_rank - 1, SETUP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&l, 1, MPI_UNSIGNED, mpi_rank - 1, SETUP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else{
			k = 5, l = 2;
		}

		for(; k * k < last; l += 2) {
			const size_t i = eratosthene::number_to_index_23_1(k * k) - o;
			if(i >= count) break;
			if(prime[i]){
				MPI_Bcast(&mpi_rank, 1, MPI_INT, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&i, 1, MPI_UNSIGNED, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&k, 1, MPI_UNSIGNED, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&l, 1, MPI_UNSIGNED, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&left, 1, MPI_BOOL, mpi_rank, MPI_COMM_WORLD);
				eratosthene::go_through(i, 2 * k, count, prime);
				eratosthene::go_through(i + k - l, 2 * k, count, prime);
			}

			k += 2;
			if(k * k >= last) break;
			const size_t j = eratosthene::number_to_index_23_1(k * k) - o;
			if(j >= count) break;
			if(prime[j]){
				MPI_Bcast(&mpi_rank, 1, MPI_INT, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&j, 1, MPI_UNSIGNED, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&k, 1, MPI_UNSIGNED, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&l, 1, MPI_UNSIGNED, mpi_rank, MPI_COMM_WORLD);
				MPI_Bcast(&right, 1, MPI_BOOL, mpi_rank, MPI_COMM_WORLD);
				eratosthene::go_through(j, 2 * k, count, prime);
				eratosthene::go_through(j + k + l, 2 * k, count, prime);
			}

			k += 4;
		}

		if(mpi_rank + 1 < mpi_size){
			MPI_Send(&k, 1, MPI_UNSIGNED, mpi_rank + 1, SETUP, MPI_COMM_WORLD);
			MPI_Send(&l, 1, MPI_UNSIGNED, mpi_rank + 1, SETUP, MPI_COMM_WORLD);
		}
	}

	std::string file_name = "ppm/mpi.ppm";
	MPI_File file;
	MPI_File_open(MPI_COMM_WORLD, (char *) file_name.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_Status status;

	// MPI_File_seek(file, offset, MPI_SEEK_SET);
	// MPI_File_write(file, &nth_val, 1, MPI_UNSIGNED, &status);

	const size_t size = std::sqrt(last - 1) + 1;

	MPI_Offset offset = ppm::write_header(file, 3, size, size, 1, status);
	size_t k = eratosthene::index_to_number_23_0(o) - 1;
	for(size_t i = 0; i < count; ++i){
		
		size_t j = ulam::ltos(k, size);
		MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
		if(prime[i]) ppm::write(file, ppm::pixel_t(1, 1, 1), status);
		else ppm::write(file, ppm::pixel_t(0, 0, 0), status);
		++k;

		for(size_t t = 0; t < 3; ++t){
			j = ulam::ltos(k, size);
			MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
			ppm::write(file, ppm::pixel_t(0, 0, 0), status);
			++k;
		}

		++i;
		if(i == count) break;
		j = ulam::ltos(k, size);
		MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
		if(prime[i]) ppm::write(file, ppm::pixel_t(1, 1, 1), status);
		else ppm::write(file, ppm::pixel_t(0, 0, 0), status);
		++k;

		j = ulam::ltos(k, size);
		MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
		ppm::write(file, ppm::pixel_t(0, 0, 0), status);

		++k;
	}







	MPI_File_close(&file);
	MPI_Finalize();

	return 0;
}