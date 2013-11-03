#include <iostream>
#include <vector>

#include "lib/eratosthene.hpp"

int main(int argc, char const *argv[]){
	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}

	const size_t count = std::stoull(argv[1]);
	std::vector<bool> prime(count, true);
	prime[0] = prime[1] = false;

	size_t k = 2;
	while(k * k < count){
		eratosthene::go_through(k * k, k, count, prime);
		++k;
	}

	// for(size_t i = 2; i < count; ++i){
	// 	if(prime[i]) std::cout << i << std::endl;
	// }

	return 0;
}