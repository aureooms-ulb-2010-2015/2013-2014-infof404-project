#ifndef PPM_HPP
#define PPM_HPP

#include <mpi.h>
#include <string>
#include <algorithm>

namespace ppm{


	typedef struct pixel_t{
		uint8_t r;
		uint8_t g;
		uint8_t b;

		pixel_t(){};
		pixel_t(const uint8_t r, const uint8_t g, const uint8_t b) : r(r), g(g), b(b) {}

	} pixel_t;


	/**
	 * Function template to write a pixel to a ppm stream. (4)
	 *
	 *
	 */

	template<typename S>
	S& operator<<(S& out, const pixel_t& c){
		if(out.flags() & std::ios::binary) return out << c.r << c.g << c.b;
		else return out << (int)c.r << ' ' << (int)c.g << ' ' << (int)c.b;
	}

	/**
	 * Function template to write a pixel to a ppm binary file over MPI. (4)
	 *
	 *
	 */

	template<typename F, typename S>
	void write(F& file, pixel_t c, S& status){
		MPI_File_write(file, &c.r, 1, MPI_BYTE, &status);
		MPI_File_write(file, &c.g, 1, MPI_BYTE, &status);
		MPI_File_write(file, &c.b, 1, MPI_BYTE, &status);
	}

	/**
	 * Function template to read a pixel from a ppm stream. (2)
	 *
	 *
	 */

	template<typename S>
	S& operator>>(S& in, const pixel_t& c){
		return in >> c.r >> c.g >> c.b;
	}

	/**
	 * Function template to flush an array to a ppm stream. (2)
	 *
	 *
	 */

	template<typename S, typename A>
	S& flush(S& out, const A& array, const size_t n){
		if(out.flags() & std::ios::binary) for(size_t i = 0; i < n; ++i) out << array[i];
		else for(size_t i = 0; i < n; ++i) out << array[i] << ' ';
		return out;
	}

	/**
	 * Function template to load an array from a ppm stream. (4)
	 *
	 *
	 */

	template<typename S, typename A>
	S& load(S& in, const size_t n, A& array){
		for(size_t i = 0; i < n; ++i) in >> array[i];
		return in;
	}

	/**
	 * Function template to load an array from a ppm binary file over MPI. (4)
	 *
	 *
	 */

	template<typename F, typename S, typename A>
	F& load(F& file, const size_t n, A& array, S& status){
		for(size_t i = 0; i < n; ++i){
			MPI_File_read(file, &array[i].r, 1, MPI_CHAR, &status);
			MPI_File_read(file, &array[i].g, 1, MPI_CHAR, &status);
			MPI_File_read(file, &array[i].b, 1, MPI_CHAR, &status);
		};
		return file;
	}

	/**
	 * Function template to write a ppm header to a ppm stream. (2)
	 *
	 *
	 */
	template<typename S>
	S& write_header(S& out, const int magic, const size_t width, const size_t height, const uint16_t max, const char sep = ' '){
		return out << 'P' << magic << sep << width << sep << height << sep << max << sep;
	}

	/**
	 * Function template to write a ppm header to a ppm binary file over MPI. (2)
	 *
	 *
	 */
	template<typename F, typename S>
	size_t write_header(F& file,  char magic,  size_t width,  size_t height,  uint16_t max, S& status, char sep = ' '){
		char P = 'P';
		std::string repr;
		size_t offset = 6;

		MPI_File_write(file, &P, 1, MPI_CHAR, &status);
		MPI_File_write(file, &magic, 1, MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		repr = std::to_string(width);
		offset += repr.size();
		MPI_File_write(file, (char*) repr.c_str(), repr.size(), MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		repr = std::to_string(height);
		offset += repr.size();
		MPI_File_write(file, (char*) repr.c_str(), repr.size(), MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		repr = std::to_string(max);
		offset += repr.size();
		MPI_File_write(file, (char*) repr.c_str(), repr.size(), MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		return offset;
	}

	/**
	 * Function template to read a ppm header from a ppm stream. (4)
	 *
	 *
	 */
	template<typename S>
	S& read_header(S& in, int& magic, size_t& width, size_t& height, uint16_t& max){
		in.get();
		return in >> magic >> width >> height >> max;
	}

	/**
	 * Function template to read a ppm header from a ppm binary file over MPI. (4)
	 *
	 *
	 */
	template<typename F, typename S>
	F& read_header(F& file, int& magic, size_t& width, size_t& height, uint16_t& max, S& status){
		const size_t n = 32;
		size_t i;
		char buffer[n] = {};
		MPI_File_read(file, buffer, 1, MPI_CHAR, &status);
		MPI_File_read(file, buffer, 1, MPI_CHAR, &status);
		magic = std::atoi(buffer);

		MPI_File_read(file, buffer, 1, MPI_CHAR, &status);
		std::fill_n(buffer, n, 0);

		i = 0;
		do{
			MPI_File_read(file, buffer + i, 1, MPI_CHAR, &status);
			++i;
		}while(i < n && buffer[i - 1] != ' ' && buffer[i - 1] != '\n' && buffer[i - 1] != '\t' && buffer[i - 1] != '\r');

		width = std::atoi(buffer);

		std::fill_n(buffer, n, 0);

		i = 0;
		do{
			MPI_File_read(file, buffer + i, 1, MPI_CHAR, &status);
			++i;
		}while(i < n && buffer[i - 1] != ' ' && buffer[i - 1] != '\n' && buffer[i - 1] != '\t' && buffer[i - 1] != '\r');

		height = std::atoi(buffer);


		std::fill_n(buffer, n, 0);

		i = 0;
		do{
			MPI_File_read(file, buffer + i, 1, MPI_CHAR, &status);
			++i;
		}while(i < n && buffer[i - 1] != ' ' && buffer[i - 1] != '\n' && buffer[i - 1] != '\t' && buffer[i - 1] != '\r');

		max = std::atoi(buffer);

		return file;
	}


}


#endif // PPM_HPP