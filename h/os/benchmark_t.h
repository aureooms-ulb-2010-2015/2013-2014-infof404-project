#ifndef OS_BENCHMARK_T_H
#define OS_BENCHMARK_T_H

#include "os/benchmark_node_t.h"
#include <vector>

namespace os{
	typedef std::vector<benchmark_node_t> benchmark_t;
}

template<typename O>
O& operator<< (O &out, const os::benchmark_t& benchmark) {

	for(const auto& node : benchmark){
		out << node << std::endl;
	}
	return out;
}

template<typename I>
I& operator>> (I &in, os::benchmark_t& benchmark) {

	while(in.good()){
		benchmark.emplace_back();
		in >> benchmark.back();
	}
	return in;
}


#endif // OS_BENCHMARK_T_H