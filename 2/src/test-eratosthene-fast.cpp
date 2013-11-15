#include <iostream>
#include <vector>

#include "lib/eratosthene.hpp"
#include "lib/prime.hpp"
#include "lib/ppm.hpp"

int main(int argc, char const *argv[]){
	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}


	const size_t nth = std::stoull(argv[1]);
	const size_t last = prime::upper_bound(nth);
	std::cout << last << std::endl;

	if(nth < 1) return 0;
	if(nth == 1){
		std::cout << 2 << std::endl;
		return 0;
	}

	const size_t count = last / 2 + last % 2 - 1;

	std::vector<bool> prime(count, true);

	size_t k = 3;
	while(k * k < last){
		const size_t i = eratosthene::number_to_index(k*k);
		if(prime[i]) eratosthene::go_through(i, k, count, prime);
		k += 2;
	}

	// size_t ith = 1;
	// for(size_t i = 0; i < count; ++i){
	// 	if(prime[i]){
	// 		++ith;
	// 		if(ith == nth){
	// 			std::cout << eratosthene::index_to_number(i) << std::endl;
	// 			break;
	// 		}
	// 	}
	// }

	return 0;
}