#ifndef PPM_HPP
#define PPM_HPP

namespace ppm{


	typedef struct pixel_t{
		int r;
		int g;
		int b;

		pixel_t(const int r, const int g, const int b) : r(r), g(g), b(b) {}

	} pixel_t;


	/**
	 * Function template to write a pixel to a ppm stream. (4)
	 *
	 *
	 */

	template<typename S>
	S& operator<<(S& out, const pixel_t& c){
		if(out.flags() & std::ios::binary) return out << c.r << c.g << c.b;
		else return out << c.r << ' ' << c.g << ' ' << c.b;
	}

	/**
	 * Function template to read a pixel from a ppm stream. (4)
	 *
	 *
	 */

	template<typename S>
	S& operator>>(S& in, const pixel_t& c){
		return in >> c.r >> c.g >> c.b;
	}

	/**
	 * Function template to flush an array to a ppm stream. (4)
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
	 * Function template to write a ppm header to a ppm stream. (4)
	 *
	 *
	 */
	template<typename S>
	S& write_header(S& out, const int magic, const size_t width, const size_t height, const uint16_t max, const char sep = ' '){
		return out << 'P' << magic << sep << width << sep << height << sep << max << sep;
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


}


#endif // PPM_HPP