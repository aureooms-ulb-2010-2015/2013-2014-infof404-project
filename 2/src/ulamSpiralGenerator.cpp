#include <iostream>
#include <mpi.h>

#include <cmath>
#include <cstdio>
#include <string>
#include <memory>
#include <chrono>

#include "os/global.hpp"
#include "os/config.hpp"
#include "os/output.hpp"
#include "os/index.hpp"
#include "os/mpi_size_t.hpp"
#include "os/alg.hpp"
#include "os/stat.hpp"

#include "lib/eratosthene.hpp"
#include "lib/prime.hpp"
#include "lib/ppm.hpp"
#include "lib/ulam.hpp"
#include "lib/io.hpp"
#include "lib/bits_t.hpp"
#include "lib/exception.hpp"
#include "lib/pixel_generator.hpp"

int main (int argc, char *argv[]){

	MPI_Init(&argc, &argv);
	int tmp_rank, tmp_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &tmp_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &tmp_size);

	os::global::start = MPI_Wtime();

	size_t mpi_rank = tmp_rank, mpi_size = tmp_size;

	os::config::fill(argc, argv);

	if(os::global::help){
		if(mpi_rank == 0) os::config::help();
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


		if(mpi_rank == mpi_size - 1) os::global::start_c = MPI_Wtime();


		if(nth == 0) last = 0, count = 0, size = 0;
		else if(nth == 1) last = 4, count = 0, size = 2;
		else if(nth == 2) last = 4, count = 0, size = 2;
		else{

			MPI_Comm* forward = new MPI_Comm[mpi_size];
			for(size_t i = 0; i < mpi_size; ++i){
				MPI_Group group, orig;
				int range[3] = {int(i), tmp_size - 1, 1};
				MPI_Comm_group(MPI_COMM_WORLD, &orig);
				MPI_Group_range_incl(orig, 1, &range, &group);
				MPI_Comm_create(MPI_COMM_WORLD, group, &forward[i]);
			}

			prime.resize(count, true);

			os::global::checkpoint = hrclock::now();
			os::alg::parallel_eratosthene_23(mpi_rank, mpi_size, forward, prime, count, last, o);
			os::global::duration += hrclock::now() - os::global::checkpoint;

			delete[] forward;
		}

		std::cout << "Process " << mpi_rank << " : " << os::global::duration.count() << " ns" << std::endl;

		if(mpi_rank == mpi_size - 1){
			os::global::stop_c = MPI_Wtime();
			std::cout << "Computation time : " << (os::global::stop_c - os::global::start_c) << " sec" << std::endl;
		}

		if(!os::global::speed){

			if(mpi_rank == 0) std::cout << "Begin writing to file" << std::endl;

			uint16_t max;
			pixel::generator<ppm::pixel_t> *painter_p, *painter_c;
			os::output::create_painters(painter_p, painter_c, max);

			const std::string prefix = os::global::params[1];
			const size_t pixels = size * size;

			std::string tmp_file_name = prefix + std::to_string(size) + ".tmp";
			MPI_File file;
			MPI_File_open(MPI_COMM_WORLD, (char *) tmp_file_name.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
			MPI_Status status;

			size_t offset = ppm::write_header(file, '6', size, size, max, status), prime_n;

			if(os::global::ssd){
				size_t tmp = os::output::apply_write_strategy_random(mpi_rank, file, status, offset, count, o, size, pixels, painter_p, painter_c, prime);
				MPI_Reduce(&tmp, &prime_n, 1, MPI_SIZE_T, MPI_SUM, 0, MPI_COMM_WORLD);
			}

			else{
				prime_n = os::output::apply_write_strategy_sequential(mpi_rank, mpi_size, file, status, offset, nth, base_count, o, r, size, pixels, painter_p, painter_c, prime, tmp_file_name);
			}

			MPI_File_close(&file);

			if(mpi_rank == 0){
				std::string file_name = prefix + std::to_string(prime_n) + ".ppm";
				std::rename(tmp_file_name.c_str(), file_name.c_str());
			}

			delete painter_p;
			delete painter_c;

			if(mpi_rank == 0) os::stat::print(last, prime_n);
		}

		MPI_Barrier(MPI_COMM_WORLD);

		os::global::stop = MPI_Wtime();

		if(mpi_rank == 0) std::cout << "Total time : " << (os::global::stop - os::global::start) << " sec" << std::endl;

		MPI_Finalize();

	}
	catch(const std::exception& e){
		MPI_Finalize();
		std::cout << "error -> " << e.what() << std::endl;
		return 1;
	}

	return 0;
}