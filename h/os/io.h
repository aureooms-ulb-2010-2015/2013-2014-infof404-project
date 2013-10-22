#ifndef OS_IO_H
#define OS_IO_H

#include <iostream>

namespace os{
	template<typename O, typename M, typename V1, typename V2>
	void store_mean(O& out, M& mean, const V1& vector_u, const V2& vector_d, const size_t u_width, const size_t d_width){
		out << u_width << ' ' << d_width << std::endl;
		out << vector_u[0];
		for(size_t i = 1; i < u_width; ++i) out << ' ' << vector_u[i];
		out << std::endl;
		out << vector_d[0];
		for(size_t j = 1; j < d_width; ++j) out << ' ' << vector_d[j];
		out << std::endl;
		for(size_t i = 0; i < u_width; ++i){
			out << mean[i][0];
			for(size_t j = 1; j < d_width; ++j){
				out << ' ' << mean[i][j];
			}
			out << std::endl;
		}
	}

	template<typename I, typename M, typename V1, typename V2>
	void load_mean(I& in, M& mean, V1& vector_u, V2& vector_d, size_t& u_width, size_t& d_width){
		in >> u_width;
		in >> d_width;
		vector_u.resize(u_width);
		for(size_t i = 0; i < u_width; ++i) in >> vector_u[i];
		vector_d.resize(d_width);
		for(size_t j = 0; j < d_width; ++j) in >> vector_d[j];
		mean.resize(u_width, typename M::value_type(d_width));
		for(size_t i = 0; i < u_width; ++i){
			for(size_t j = 0; j < d_width; ++j){
				in >> mean[i][j];
			}
		}
	}
}

#endif // OS_IO_H