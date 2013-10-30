#include <iostream>
#include <vector>
#include <numeric>
#include <utility>

#include "lib/prime.hpp"
#include "lib/io.hpp"

int main(int argc, char const *argv[]){
	if(argc != 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}

	size_t i = 0, count = std::stoull(argv[1]);
	std::vector<uint64_t> prime, mul;

	size_t k = 1;
	while(i < count){
		++k;
		if(!prime::linear_search(mul, 0, mul.size(), k)){
			std::cout << k << std::endl;
			++i;
			prime.push_back(k);
			mul.push_back(k*k);
			continue;
		}
		for(size_t j = 0; j < i; ++j) if(mul[j] <= k) mul[j] += prime[j];
	}

	std::cout << prime << std::endl;
	std::cout << mul << std::endl;

	return 0;
}