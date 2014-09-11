#include <iostream>
#include <vector>

#include "lib/bits_2_t.hpp"

typedef bits_2_t<uint16_t> array_t;

int main(int argc, char const *argv[]){
	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}

	const size_t count = std::stoull(argv[1]);
	array_t prime(count, true);
	prime[0] = prime[1] = false;

	size_t k = 2;
	while(k * k < count){
		if(prime[k]) for(size_t i = k * k; i < count; i += k) prime[i] = false;
		++k;
	}

	return 0;
}