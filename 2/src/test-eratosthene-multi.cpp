#include <iostream>
#include <vector>

#include "lib/eratosthene.hpp"

int main(int argc, char const *argv[]){
	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}

	const size_t last = std::stoull(argv[1]);

	if(last < 2) return 0;
	std::cout << 2 << std::endl;
	if(last == 2) return 0;

	const size_t count = last / 2 + last % 2 - 1;

	std::vector<bool> prime(count, true);

	size_t k = 3;
	while(k * k < last){
		const size_t i = eratosthene::number_to_index(k*k);
		eratosthene::go_through(i, k, count, prime);
		k += 2;
	}

	// for(size_t i = 0; i < count; ++i){
	// 	if(prime[i]) std::cout << eratosthene::index_to_number(i) << std::endl;
	// }

	return 0;
}