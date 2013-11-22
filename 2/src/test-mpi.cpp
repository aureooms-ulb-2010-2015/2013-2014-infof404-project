#include <iostream>
#include <mpi.h>

#include <vector>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <memory>

#include <sys/stat.h>

#include "lib/eratosthene.hpp"
#include "lib/prime.hpp"
#include "lib/ppm.hpp"
#include "lib/ulam.hpp"
#include "lib/io.hpp"
#include "lib/bits_t.hpp"
#include "lib/pinput.hpp"
#include "lib/exception.hpp"
#include "lib/pixel_generator.hpp"

const int SETUP = 0;
const int GATHER = 1;

void show_help();
inline size_t compute_local_count(size_t, size_t, size_t, size_t&);

template<typename A>
inline size_t compute_who(size_t, const A&, size_t);

inline MPI_Datatype GET_MPI_SIZE_T();

const MPI_Datatype MPI_SIZE_T = GET_MPI_SIZE_T();

int main (int argc, char *argv[]){

	double start = MPI_Wtime();

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

	std::vector<std::string> params;
	std::map<std::string, std::vector<std::string>> options;
	std::set<std::string> flags;
	std::set<std::string> option_set = {
		"--filter"
	};
	std::set<std::string> flag_set = {
		"-h", "--help",
		"--speed",
		"--nobuffer",
	};

	pinput::parse(argc, argv, params, options, flags, option_set, flag_set);

	try{

		// check input
		const bool help = flags.count("-h") || flags.count("--help");
		if(help){
			if(mpi_rank == 0) show_help();
			MPI_Finalize();
			return 0;
		}



		const bool speed = flags.count("--speed");
		const bool nobuffer = flags.count("--nobuffer");
		const bool filter = options.count("--filter") && options["--filter"].size() > 0;
		const bool mix = filter && options["--filter"].size() > 1;
		if(params.size() < 1) throw lib::exception("#0 missing");
		if(params.size() < 2 && !speed) throw lib::exception("#1 missing");
		const size_t nth = std::stoull(params[0]);
		size_t last = prime::upper_bound(nth);
		size_t size = (last == 0) ? 0 : std::sqrt(last - 1) + 1;
		last = size * size;

		bits_t<size_t> prime;
		// std::vector<bool> prime;
		// std::cout << last << std::endl;
		size_t count = last / 6;

		if(last % 6 <= 4) count = 2 * count + 1;
		else count = 2 * count + 2;

		// std::cout << "total size := " << count << std::endl;
		const size_t total_count = count;
		size_t r = count % mpi_size;
		count /= mpi_size;
		const size_t base_count = count;

		size_t o;
		count = compute_local_count(mpi_rank, r, base_count, o);

		// std::cout << mpi_rank << " my size := " << count << ", my offset :=" << o  << '(' << eratosthene::index_to_number_23_0(o) << ')' << std::endl;

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

					// std::cout << "received, " << tmp << ',' << i << ',' << k << ',' << l << ',' << which << std::endl;

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

					// std::cout << "computed i : " << i << std::endl;
					// std::cout << "computed j : " << j << std::endl;

					// std::cout << eratosthene::index_to_number_23_1(i + o) << std::endl;
					// std::cout << eratosthene::index_to_number_23_0(j + o) << std::endl;
					eratosthene::go_through(i, 2 * k, count, prime);
					eratosthene::go_through(j, 2 * k, count, prime);
				}
				while(mpi_rank > current);

				// std::cout << "change current" << std::endl;

				MPI_Recv(&k, 1, MPI_SIZE_T, 0, SETUP, forward[current - 1], MPI_STATUS_IGNORE);
				MPI_Recv(&l, 1, MPI_SIZE_T, 0, SETUP, forward[current - 1], MPI_STATUS_IGNORE);
				// std::cout << "change current" << std::endl;
			}
			else{
				k = 5, l = 2;
			}
			// std::cout << "I'm the leader with " << k << ", " << l << std::endl;
			for(; k * k < last && l <= count; l += 2) {
				size_t i = eratosthene::number_to_index_23_1(k * k) - o;
				// std::cout << i << std::endl;
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
				// std::cout << j << std::endl;
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

			// std::cout << "I finished with " << k << ',' << last << std::endl;

			if(mpi_rank + 1 < mpi_size){
				++current;
				MPI_Bcast(&current, 1, MPI_SIZE_T, 0, forward[mpi_rank]);
				MPI_Send(&k, 1, MPI_SIZE_T, 1, SETUP, forward[mpi_rank]);
				MPI_Send(&l, 1, MPI_SIZE_T, 1, SETUP, forward[mpi_rank]);
			}
		}

		// std::cout << "hllo" << std::endl;

		delete[] forward;

		if(!speed){

			uint16_t max;
			pixel::generator<ppm::pixel_t> *painter_p, *painter_c;
			if(filter){
				std::string file_name = options["--filter"][0];
				MPI_File file;
				MPI_File_open(MPI_COMM_WORLD, (char *) file_name.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
				MPI_Status status;

				int magic;
				size_t width;
				size_t height;

				ppm::read_header(file, magic, width, height, max, status);
				typedef std::shared_ptr<std::vector<ppm::pixel_t>> image_t; 
				image_t array = image_t(new std::vector<ppm::pixel_t>(width * height));
				ppm::load(file, width * height, *array, status);
				MPI_File_close(&file);

				painter_p = new pixel::square_generator<image_t, ppm::pixel_t>(array, height, width);
				if(mix){
					std::string file_name = options["--filter"][1];
					MPI_File file;
					MPI_File_open(MPI_COMM_WORLD, (char *) file_name.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
					MPI_Status status;

					int magic;
					size_t width;
					size_t height;

					ppm::read_header(file, magic, width, height, max, status);
					typedef std::shared_ptr<std::vector<ppm::pixel_t>> image_t; 
					image_t array = image_t(new std::vector<ppm::pixel_t>(width * height));
					ppm::load(file, width * height, *array, status);
					MPI_File_close(&file);
					painter_c = new pixel::square_generator<image_t, ppm::pixel_t>(array, height, width);
				}
				else{
					painter_c = new pixel::unique_generator<ppm::pixel_t>(ppm::pixel_t(0, 0, 0));
				}
			}
			else{
				max = 1;
				painter_p = new pixel::unique_generator<ppm::pixel_t>(ppm::pixel_t(1, 1, 1));
				painter_c = new pixel::unique_generator<ppm::pixel_t>(ppm::pixel_t(0, 0, 0));
			}

			const std::string prefix = params[1];
			const size_t pixels = size * size;

			std::string file_name = prefix + std::to_string(size) + ".ppm";
			MPI_File file;
			MPI_File_open(MPI_COMM_WORLD, (char *) file_name.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
			MPI_Status status;

			size_t offset = ppm::write_header(file, '6', size, size, max, status);
			// std::cout << offset << ',' << count << std::endl;

			if(nobuffer){

				if(mpi_rank == 0){
					MPI_File_set_size(file, offset + size * size * 3);
					if(pixels >= 2){
						for(size_t i = 1; i < 3; ++i){
							size_t j = ulam::ltos(i, size);
							MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
							ppm::write(file, painter_p->get(j / size, j % size, size, size), status);
						}
						for(size_t i = 0; i < 4; i += 3){
							size_t j = ulam::ltos(i, size);
							MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
							ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
						}
					}

				}
				size_t k = eratosthene::index_to_number_23_0(o) - 1;
				// std::cout << "k : " << k << std::endl;
				for(size_t i = 0; i < count; ++i){
					if(k >= pixels) break;
					size_t j = ulam::ltos(k, size);
					// std::cout << mpi_rank << " writes " << j << std::endl;
					MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
					if(prime[i]){
						// std::cout << (k + 1) << " is prime" << std::endl;
						ppm::write(file, painter_p->get(j / size, j % size, size, size), status);
					}
					else{
						// std::cout << (k + 1) << " is not prime" << std::endl;
						ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
					}
					++k;

					if(k >= pixels) break;
					j = ulam::ltos(k, size);
					// std::cout << mpi_rank << " writes " << j << std::endl;
					MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
					ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
					++k;


					if(k >= pixels) break;
					++i;
					if(i == count) break;
					j = ulam::ltos(k, size);
					// std::cout << mpi_rank << " writes " << j << std::endl;
					MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
					if(prime[i]){
						// std::cout << (k + 1) << " is prime" << std::endl;
						ppm::write(file, painter_p->get(j / size, j % size, size, size), status);
					}
					else{
						// std::cout << (k + 1) << " is not prime" << std::endl;
						ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
					}
					++k;


					for(size_t t = 0; t < 3; ++t){
						if(k >= pixels) break;
						j = ulam::ltos(k, size);
						// std::cout << mpi_rank << " writes " << j << std::endl;
						MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
						ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
						++k;
					}
				}
			}

			else{
				if(mpi_rank == 0){
					bits_t<size_t>* gather = new bits_t<size_t>[mpi_size];
					gather[0].size = prime.size;
					gather[0].blocks = prime.blocks;
					gather[0].data = prime.data;
					if(nth > 2){
						for(size_t i = 1; i < mpi_size; ++i){
							// std::cout << "recv " << i << std::endl;
							MPI_Recv(&gather[i].blocks, 1, MPI_SIZE_T, i, GATHER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							// std::cout << gather[i].blocks << std::endl;
							gather[i].data = new size_t[gather[i].blocks];
							MPI_Recv(gather[i].data, gather[i].blocks, MPI_SIZE_T, i, GATHER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							// std::cout << "done" << std::endl;
						}
					}

					struct stat attrib;
					stat(file_name.c_str(), &attrib);
					const size_t block_size = attrib.st_blksize;

					ppm::pixel_t* buffer = new ppm::pixel_t[block_size];
					size_t* partition = new size_t[mpi_size];
					partition[0] = o;
					for(size_t i = 1; i < mpi_size; ++i) gather[i].size = compute_local_count(i, r, base_count, partition[i]);

					const size_t first_block_size = std::min(block_size - offset % block_size, pixels);
					size_t current_block_size = first_block_size;
					
					const size_t blocks = (pixels > first_block_size) ? (pixels - first_block_size - 1) / block_size + 2 : 1;
					size_t pt = 0;
					for(size_t j = 0; j < blocks; ++j){
						// std::cout << "block n°" << j  << " (" << current_block_size << ")" << std::endl;
						for(size_t i = 0; i < current_block_size; ++i, ++pt){
							// std::cout << "i := " << i  << " / " << current_block_size << std::endl;
							size_t k = ulam::stol(pt, size) + 1;
							if(k == 1) buffer[i] = painter_c->get(pt / size, pt % size, size, size);
							else if(k == 2) buffer[i] = painter_p->get(pt / size, pt % size, size, size);
							else if(k == 3) buffer[i] = painter_p->get(pt / size, pt % size, size, size);
							else if(k % 2 == 0) buffer[i] = painter_c->get(pt / size, pt % size, size, size);
							else if(k % 3 == 0) buffer[i] = painter_c->get(pt / size, pt % size, size, size);
							else if(k % 6 == 5){
								size_t j = eratosthene::number_to_index_23_0(k);
								size_t who = compute_who(j, partition, mpi_size), where = j - partition[who];
								// std::cout << "who " << who << std::endl;
								// std::cout << "what ???? 1" << std::endl;
								// std::cout << "what ???? who " << who << std::endl;
								// std::cout << "what ???? where " << where << std::endl;
								// std::cout << "what ???? is_prime " << gather[who][where] << std::endl;
								if(gather[who][where]) buffer[i] = painter_p->get(pt / size, pt % size, size, size);
								else buffer[i] = painter_c->get(pt / size, pt % size, size, size);
							}
							else if(k % 6 == 1){
								size_t j = eratosthene::number_to_index_23_1(k);
								size_t who = compute_who(j, partition, mpi_size), where = j - partition[who];
								// std::cout << "who " << who << " " << sizeof(who) << std::endl;
								// std::cout << "what ???? 1" << std::endl;
								// std::cout << "what ???? who " << who << std::endl;
								// std::cout << "what ???? where " << where << std::endl;
								// std::cout << "what ???? is_prime " << gather[who][where] << std::endl;
								if(gather[who][where]) buffer[i] = painter_p->get(pt / size, pt % size, size, size);
								else buffer[i] = painter_c->get(pt / size, pt % size, size, size);
							}
							else buffer[i] = painter_c->get(pt / size, pt % size, size, size);
						}

						ppm::flush(file, current_block_size, buffer, status);
						current_block_size = std::min(block_size, pixels - (j * block_size) - first_block_size);
					}
					delete[] buffer;
					delete[] partition;
					gather[0].data = nullptr;
					delete[] gather;
				}
				else if(nth > 2){
					MPI_Send(&prime.blocks, 1, MPI_SIZE_T, 0, GATHER, MPI_COMM_WORLD);
					MPI_Send(prime.data, prime.blocks, MPI_SIZE_T, 0, GATHER, MPI_COMM_WORLD);
				}
			}

			MPI_File_close(&file);

			delete painter_p;
			delete painter_c;
		}


		double stop = MPI_Wtime();

		std::cout << "Process " << mpi_rank << " : " << (stop - start) << " sec" << std::endl;


		MPI_Finalize();

	}
	catch(const std::exception& e){
		MPI_Finalize();
		std::cout << "error -> " << e.what() << std::endl;
		return 1;
	}

	return 0;
}



void show_help(){

}

inline size_t compute_local_count(size_t mpi_rank, size_t r, size_t count, size_t& o){
	if(r > mpi_rank){
		if(count % 2 == 1){
			++count;
			o = mpi_rank * count;
		}
		else if(mpi_rank % 2 == 0){
			count += 2;
			o = mpi_rank * (count - 1);
		}
		else{
			o = mpi_rank * (count + 1) + 1;
		}
		
	}
	else{
		if(count % 2 == 1){
			if((mpi_rank - r) % 2 == 0){
				o = r * (count + 1) + (mpi_rank - r) * count;
				++count;
			}
			else{
				o = r * (count + 1) + (mpi_rank - r) * count + 1;
				--count;
			}
		}
		else if(r % 2 == 0){
			o = r * (count + 1) + (mpi_rank - r) * count;
		}
		else{
			o = r * (count + 1) + (mpi_rank - r) * count + 1;
		}
		
	}

	return count;
}

template<typename A>
inline size_t compute_who(size_t i, const A& partition, size_t n){
	for(size_t k = n - 1; k > 0; --k){
		if(partition[k] <= i) return k;
	}
	return 0;
}

inline MPI_Datatype GET_MPI_SIZE_T(){
	if(sizeof(size_t) == 1) return MPI_INTEGER1;
	if(sizeof(size_t) == 2) return MPI_INTEGER2;
	if(sizeof(size_t) == 4) return MPI_INTEGER4;
	else return MPI_INTEGER8;
}