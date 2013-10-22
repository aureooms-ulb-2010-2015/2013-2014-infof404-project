#ifndef OS_IO_H
#define OS_IO_H

#include <iostream>

namespace os{
	template<typename O, typename M>
	void store_mean(O& out, M& mean, const size_t x_size, const size_t y_size){
		for(size_t i = 0; i < x_size; ++i){
			out << mean[i][0];
			for(size_t j = 1; j < y_size; ++j){
				out << ' ' << mean[i][j];
			}
			out << std::endl;
		}
	}

	template<typename I, typename M>
	void load_mean(I& in, M& mean, const size_t x_size, const size_t y_size){
		for(size_t i = 0; i < x_size; ++i){
			for(size_t j = 0; j < y_size; ++j){
				in >> mean[i][j];
			}
		}
	}
}

#endif // OS_IO_H