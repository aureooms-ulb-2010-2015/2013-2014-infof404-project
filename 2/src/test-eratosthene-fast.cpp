#include <iostream>
#include <vector>
#include <cmath>

#include "lib/eratosthene.hpp"
#include "lib/prime.hpp"
#include "lib/ppm.hpp"
#include "lib/ulam.hpp"

int main(int argc, char const *argv[]){
	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}


	const size_t nth = std::stoull(argv[1]);
	const size_t last = prime::upper_bound(nth);
	size_t nth_val = 0;
	
	std::vector<bool> prime;

	if(nth < 1) return 0;
	else if(nth == 1){
		nth_val = 2;
		prime.resize(1, true);
	}
	else{
		const size_t count = last / 2 + last % 2 - 1;

		prime.resize(count, true);

		size_t k = 3;
		while(k * k < last){
			const size_t i = eratosthene::number_to_index(k * k);
			if(prime[i]) eratosthene::go_through(i, k, count, prime);
			k += 2;
		}

		size_t ith = 1;

		for(size_t i = 0; i < count; ++i){
			if(prime[i]){
				++ith;
				if(ith == nth){
					nth_val =  eratosthene::index_to_number(i);
					break;
				}
			}
		}
	}

	const size_t size = std::sqrt(nth_val) + 1;
	const size_t pixels = size * size;

	ppm::write_header(std::cout, 3, size, size, 1);
	std::cout << std::endl;
	for(size_t i = 0; i < pixels;){
		size_t k = ulam::stol(i, size) + 1;
		if(k == 1) std::cout << ppm::pixel_t(0, 0, 0) << ' ';
		else if(k == 2) std::cout << ppm::pixel_t(1, 1, 1) << ' ';
		else if(k % 2 == 0) std::cout << ppm::pixel_t(0, 0, 0) << ' ';
		else if(prime[eratosthene::number_to_index(k)]) std::cout << ppm::pixel_t(1, 1, 1) << ' ';
		else std::cout << ppm::pixel_t(0, 0, 0) << ' ';
		++i;
		if(i % size == 0) std::cout << std::endl;
	}



	return 0;
}