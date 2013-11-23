#include <iostream>
#include <mpi.h>

#include <cmath>
#include <string>
#include <memory>

#include "os/global.hpp"
#include "os/config.hpp"
#include "os/output.hpp"
#include "os/index.hpp"
#include "os/mpi_size_t.hpp"

#include "lib/eratosthene.hpp"
#include "lib/prime.hpp"
#include "lib/ppm.hpp"
#include "lib/ulam.hpp"
#include "lib/io.hpp"
#include "lib/bits_t.hpp"
#include "lib/exception.hpp"
#include "lib/pixel_generator.hpp"

void help();

int main (int argc, char *argv[]){

	os::global::start = MPI_Wtime();

	MPI_Init(&argc, &argv);

	int tmp_rank, tmp_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &tmp_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &tmp_size);


	size_t mpi_rank = tmp_rank, mpi_size = tmp_size;

	MPI_Comm* forward = new MPI_Comm[mpi_size];
	for(size_t i = 0; i < mpi_size; ++i){
		MPI_Group group, orig;
		int range[3] = {int(i), tmp_size - 1, 1};
		MPI_Comm_group(MPI_COMM_WORLD, &orig);
		MPI_Group_range_incl(orig, 1, &range, &group);
		MPI_Comm_create(MPI_COMM_WORLD, group, &forward[i]);
	}


	os::config::fill(argc, argv);

	if(os::global::help){
		if(mpi_rank == 0) help();
		MPI_Finalize();
		return 0;
	}

	try{
		os::config::check();

		const size_t nth = std::stoull(os::global::params[0]);
		size_t size = os::index::size(nth);
		size_t last = os::index::last(size);
		const size_t total_count = os::index::total_count(last);

		const size_t r = total_count % mpi_size;
		const size_t base_count = total_count / mpi_size;

		size_t o;
		size_t count = os::index::local_count(mpi_rank, r, base_count, o);

		bits_t<size_t> prime;

		if(nth == 0) last = 0, count = 0, size = 0;
		else if(nth == 1) last = 4, count = 0, size = 2;
		else if(nth == 2) last = 4, count = 0, size = 2;
		else{
			prime.resize(count, true);
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


		delete[] forward;

		if(!os::global::speed){

			uint16_t max;
			pixel::generator<ppm::pixel_t> *painter_p, *painter_c;
			os::output::create_painters(painter_p, painter_c, max);

			const std::string prefix = os::global::params[1];
			const size_t pixels = size * size;

			std::string file_name = prefix + std::to_string(size) + ".ppm";
			MPI_File file;
			MPI_File_open(MPI_COMM_WORLD, (char *) file_name.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
			MPI_Status status;

			size_t offset = ppm::write_header(file, '6', size, size, max, status);

			if(os::global::ssd){
				os::output::apply_write_strategy_random(mpi_rank, file, status, offset, count, o, size, pixels, painter_p, painter_c, prime);
			}

			else{
				os::output::apply_write_strategy_sequential(mpi_rank, mpi_size, file, status, offset, nth, base_count, o, r, size, pixels, painter_p, painter_c, prime, file_name);
			}

			MPI_File_close(&file);

			delete painter_p;
			delete painter_c;
		}


		os::global::stop = MPI_Wtime();

		std::cout << "Process " << mpi_rank << " : " << (os::global::stop - os::global::start) << " sec" << std::endl;


		MPI_Finalize();

	}
	catch(const std::exception& e){
		MPI_Finalize();
		std::cout << "error -> " << e.what() << std::endl;
		return 1;
	}

	return 0;
}



void help(){
	std::cout << "helping you" << std::endl;
}