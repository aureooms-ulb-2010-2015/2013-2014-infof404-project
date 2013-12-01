#ifndef OS_OUTPUT_HPP
#define OS_OUTPUT_HPP

#include "os/global.hpp"
#include "os/index.hpp"
#include "os/mpi_size_t.hpp"

#include "lib/bits_t.hpp"
#include "lib/ppm.hpp"
#include "lib/pixel_generator.hpp"
#include "lib/ulam.hpp"
#include "lib/eratosthene.hpp"

#include <sys/stat.h>
#include <string>
#include <mpi.h>
#include <memory>

namespace os{
	namespace output{

		template<typename P>
		inline void create_painters(P*& painter_p, P*& painter_c, uint16_t& max){
			const int mode = os::global::avoid_overflow ? pixel::AVOID_OVERFLOW : pixel::NORMAL;
			if(os::global::prime_filter){
				std::string file_name = os::global::options["--prime-filter"][0];
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

				painter_p = new pixel::square_generator<image_t, ppm::pixel_t>(array, height, width, mode);
			}
			else{
				max = 1;
				ppm::pixel_t pixel;
				if(os::global::prime_color){
					pixel = ppm::pixel_t(
						std::stoi(os::global::options["--prime-color"][0]),
						std::stoi(os::global::options["--prime-color"][1]),
						std::stoi(os::global::options["--prime-color"][2])
					);
					max = 255;
				}
				else pixel = ppm::pixel_t(0, 0, 0);
				painter_p = new pixel::unique_generator<ppm::pixel_t>(pixel);
			}
			if(os::global::composite_filter){
				std::string file_name = os::global::options["--composite-filter"][0];
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
				painter_c = new pixel::square_generator<image_t, ppm::pixel_t>(array, height, width, mode);
			}
			else{
				ppm::pixel_t pixel;
				if(os::global::composite_color) pixel = ppm::pixel_t(
					std::stoi(os::global::options["--composite-color"][0]),
					std::stoi(os::global::options["--composite-color"][1]),
					std::stoi(os::global::options["--composite-color"][2])
				);
				else pixel = ppm::pixel_t(1, 1, 1);
				painter_c = new pixel::unique_generator<ppm::pixel_t>(pixel);
			}
		}


		template<typename F, typename S, typename P, typename A>
		inline size_t apply_write_strategy_random(size_t mpi_rank, F& file, S& status, size_t offset, size_t count, size_t o, size_t size, size_t pixels, P* painter_p, P* painter_c, A& prime){
			size_t prime_n = 0;
			if(mpi_rank == 0){
				MPI_File_set_size(file, offset + size * size * 3);
				if(pixels >= 2){
					for(size_t i = 1; i < 3; ++i){
						size_t j = ulam::ltos(i, size);
						MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
						ppm::write(file, painter_p->get(j / size, j % size, size, size), status);
						++prime_n;
					}
					for(size_t i = 0; i < 4; i += 3){
						size_t j = ulam::ltos(i, size);
						MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
						ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
					}
				}

			}
			size_t k = eratosthene::index_to_number_23_0(o) - 1;
			for(size_t i = 0; i < count; ++i){
				if(k >= pixels) break;
				size_t j = ulam::ltos(k, size);
				MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
				if(prime[i]){
					++prime_n;
					ppm::write(file, painter_p->get(j / size, j % size, size, size), status);
				}
				else{
					ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
				}
				++k;

				if(k >= pixels) break;
				j = ulam::ltos(k, size);
				MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
				ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
				++k;


				if(k >= pixels) break;
				++i;
				if(i == count) break;
				j = ulam::ltos(k, size);
				MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
				if(prime[i]){
					++prime_n;
					ppm::write(file, painter_p->get(j / size, j % size, size, size), status);
				}
				else{
					ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
				}
				++k;


				for(size_t t = 0; t < 3; ++t){
					if(k >= pixels) break;
					j = ulam::ltos(k, size);
					MPI_File_seek(file, offset + j * 3, MPI_SEEK_SET);
					ppm::write(file, painter_c->get(j / size, j % size, size, size), status);
					++k;
				}
			}

			prime.clear();

			return prime_n;
		}


		template<typename F, typename S, typename P, typename A>
		inline size_t apply_write_strategy_sequential(size_t mpi_rank, size_t mpi_size, F& file, S& status, size_t offset, size_t nth, size_t base_count, size_t o, size_t r, size_t size, size_t pixels, P* painter_p, P* painter_c, A& prime, std::string& file_name){
			size_t prime_n = 0;
			if(mpi_rank == 0){
				bits_t<size_t>* gather = new bits_t<size_t>[mpi_size];
				gather[0].size = prime.size;
				gather[0].blocks = prime.blocks;
				gather[0].data = prime.data;
				if(nth > 2){
					for(size_t i = 1; i < mpi_size; ++i){
						MPI_Recv(&gather[i].blocks, 1, MPI_SIZE_T, i, os::global::GATHER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						gather[i].data = new size_t[gather[i].blocks];
						MPI_Recv(gather[i].data, gather[i].blocks, MPI_SIZE_T, i, os::global::GATHER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					}
				}

				struct stat attrib;
				stat(file_name.c_str(), &attrib);
				const size_t block_size = attrib.st_blksize;

				ppm::pixel_t* buffer = new ppm::pixel_t[block_size];
				size_t* partition = new size_t[mpi_size];
				partition[0] = o;
				for(size_t i = 1; i < mpi_size; ++i) gather[i].size = os::index::local_count(i, r, base_count, partition[i]);

				const size_t first_block_size = std::min(block_size - offset % block_size, pixels);
				size_t current_block_size = first_block_size;
				
				const size_t blocks = (pixels > first_block_size) ? (pixels - first_block_size - 1) / block_size + 2 : 1;
				size_t pt = 0;
				for(size_t j = 0; j < blocks; ++j){
					for(size_t i = 0; i < current_block_size; ++i, ++pt){
						size_t k = ulam::stol(pt, size) + 1;
						if(k == 1) buffer[i] = painter_c->get(pt / size, pt % size, size, size);
						else if(k == 2){
							buffer[i] = painter_p->get(pt / size, pt % size, size, size);
							++prime_n;
						}
						else if(k == 3){
							buffer[i] = painter_p->get(pt / size, pt % size, size, size);	
							++prime_n;
						}
						else if(k % 2 == 0) buffer[i] = painter_c->get(pt / size, pt % size, size, size);
						else if(k % 3 == 0) buffer[i] = painter_c->get(pt / size, pt % size, size, size);
						else if(k % 6 == 5){
							size_t j = eratosthene::number_to_index_23_0(k);
							size_t who = os::index::who(j, partition, mpi_size), where = j - partition[who];
							if(gather[who][where]){
								buffer[i] = painter_p->get(pt / size, pt % size, size, size);
								++prime_n;
							}
							else buffer[i] = painter_c->get(pt / size, pt % size, size, size);
						}
						else if(k % 6 == 1){
							size_t j = eratosthene::number_to_index_23_1(k);
							size_t who = os::index::who(j, partition, mpi_size), where = j - partition[who];
							if(gather[who][where]){
								buffer[i] = painter_p->get(pt / size, pt % size, size, size);
								++prime_n;
							}
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
				MPI_Send(&prime.blocks, 1, MPI_SIZE_T, 0, os::global::GATHER, MPI_COMM_WORLD);
				MPI_Send(prime.data, prime.blocks, MPI_SIZE_T, 0, os::global::GATHER, MPI_COMM_WORLD);
				prime.clear();
			}
			return prime_n;
		}


	}
}




#endif // OS_OUTPUT_HPP