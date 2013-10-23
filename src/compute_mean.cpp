#include <iostream>
#include <vector>

#include "os/benchmark_t.h"
#include "os/io.h"
#include "os/study/plot.h"


int main(/*int argc, char const *argv[]*/){
	
	std::vector<double> vector_u;
	std::vector<uint> vector_d;
	std::vector<uint> vector_n;

	size_t u_width;
	size_t d_width;
	size_t n_width;
	size_t k;

	os::benchmark_t benchmark;

	os::load_benchmark(std::cin, benchmark, vector_u, vector_d, vector_n, u_width, d_width, n_width, k);

	std::vector<std::vector<double>> p_mean(u_width, std::vector<double>(d_width, 0));
	std::vector<std::vector<double>> s_mean(u_width, std::vector<double>(d_width, 0));
	std::vector<std::vector<double>> counter(u_width, std::vector<double>(d_width, 0));

	os::study::compute_mean(benchmark, u_width, d_width, p_mean, s_mean, counter);

	os::store_mean(std::cout, p_mean, vector_u, vector_d, u_width, d_width);
	os::store_mean(std::cout, s_mean, vector_u, vector_d, u_width, d_width);

	return 0;
}