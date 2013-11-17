#ifndef PPM_HPP
#define PPM_HPP

#include <mpi.h>
#include <string>

namespace ppm{


	typedef struct pixel_t{
		uint8_t r;
		uint8_t g;
		uint8_t b;

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
	void write(F& file, const pixel_t& c, S& status){
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
	S& load(S& in, A& array, const size_t n){
		for(size_t i = 0; i < n; ++i) in >> array[i];
		return in;
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
	size_t write_header(F& file, const char magic, const size_t width, const size_t height, const uint16_t max, S& status, const char sep = ' '){
		const char P = 'P';
		std::string repr;
		size_t offset = 6;

		MPI_File_write(file, &P, 1, MPI_CHAR, &status);
		MPI_File_write(file, &magic, 1, MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		repr = std::to_string(width);
		offset += repr.size();
		MPI_File_write(file, &repr.c_srt(), repr.size(), MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		repr = std::to_string(height);
		offset += repr.size();
		MPI_File_write(file, &repr.c_srt(), repr.size(), MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		repr = std::to_string(max);
		offset += repr.size();
		MPI_File_write(file, &repr.c_srt(), repr.size(), MPI_CHAR, &status);
		MPI_File_write(file, &sep, 1, MPI_CHAR, &status);

		return offset;
	}

	/**
	 * Function template to read a ppm header from a ppm stream. (2)
	 *
	 *
	 */
	template<typename S>
	S& read_header(S& in, int& magic, size_t& width, size_t& height, uint16_t& max){
		in.get();
		return in >> magic >> width >> height >> max;
	}


}


#endif // PPM_HPP