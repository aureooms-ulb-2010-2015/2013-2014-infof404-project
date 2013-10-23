#ifndef OS_IO_H
#define OS_IO_H

#include <iostream>

#include "os/benchmark_t.h"

namespace os{
	template<typename O, typename V1, typename V2>
	void store_parameters(O& out, const V1& vector_u, const V2& vector_d, const size_t u_width, const size_t d_width){
		out << u_width << ' ' << d_width << std::endl;
		out << vector_u[0];
		for(size_t i = 1; i < u_width; ++i) out << ' ' << vector_u[i];
		out << std::endl;
		out << vector_d[0];
		for(size_t j = 1; j < d_width; ++j) out << ' ' << vector_d[j];
		out << std::endl;
	}

	template<typename I, typename V1, typename V2>
	void load_parameters(I& in, V1& vector_u, V2& vector_d, size_t& u_width, size_t& d_width){
		in >> u_width;
		in >> d_width;
		vector_u.resize(u_width);
		for(size_t i = 0; i < u_width; ++i) in >> vector_u[i];
		vector_d.resize(d_width);
		for(size_t j = 0; j < d_width; ++j) in >> vector_d[j];
	}

	template<typename O, typename M, typename V1, typename V2>
	void store_mean(O& out, M& mean, const V1& vector_u, const V2& vector_d, const size_t u_width, const size_t d_width){
		store_parameters(out, vector_u, vector_d, u_width, d_width);
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
		load_parameters(in, vector_u, vector_d, u_width, d_width);
		mean.resize(u_width, typename M::value_type(d_width));
		for(size_t i = 0; i < u_width; ++i){
			for(size_t j = 0; j < d_width; ++j){
				in >> mean[i][j];
			}
		}
	}



	template<typename O, typename V1, typename V2, typename V3>
	void store_benchmark(O &out, const os::benchmark_t& benchmark, const V1& vector_u, const V2& vector_d, const V3& vector_n, const size_t u_width, const size_t d_width, const size_t n_width, const size_t k) {
		store_parameters(out, vector_u, vector_d, u_width, d_width);
		out << n_width << std::endl;
		out << vector_n[0];
		for(size_t i = 1; i < n_width; ++i) out << ' ' << vector_n[i];
		out << std::endl;
		out << k << std::endl;
		for(const auto& node : benchmark){
			out << node << std::endl;
		}
	}

	template<typename I, typename V1, typename V2, typename V3>
	void load_benchmark(I &in, os::benchmark_t& benchmark, V1& vector_u, V2& vector_d, V3& vector_n, size_t& u_width, size_t& d_width, size_t& n_width, size_t& k) {
		load_parameters(in, vector_u, vector_d, u_width, d_width);
		in >> n_width;
		vector_n.resize(n_width);
		for(size_t i = 0; i < n_width; ++i) in >> vector_n[i];
		in >> k;
		const size_t length = u_width * d_width * n_width * k;
		for(size_t i = 0; i < length; ++i){
			benchmark.emplace_back();
			in >> benchmark.back();
		}
	}
}

#endif // OS_IO_H