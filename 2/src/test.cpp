#include <iostream>
#include <vector>
#include <numeric>
#include <utility>

#include "lib/prime.hpp"
#include "lib/io.hpp"

int main(/*int argc, char const *argv[]*/){
	std::cout << "hello, world" << std::endl;

	size_t i = 0, count = 10000;
	std::vector<uint64_t> prime, mul;

	size_t k = 1;
	while(i < count){
		++k;
		if(!prime::linear_search(mul, 0, mul.size(), k)){
			std::cout << k << std::endl;
			++i;
			prime.push_back(k);
			mul.push_back(k);
		}
		for(size_t j = 0; j < i; ++j) if(mul[j] <= k) mul[j] += prime[j];
		// size_t last = i - 1;
		// for(size_t j = 0; j <= last;){
		// 	if(mul[j] <= k){
		// 		mul[j] += prime[j];
		// 		std::swap(prime[j], prime[last]);
		// 		std::swap(mul[j], mul[last]);
		// 		if(last > 0) --last;
		// 		else break;
		// 	}
		// 	else ++j;
		// }
		// break;
	}

	std::cout << prime << std::endl;
	std::cout << mul << std::endl;

	return 0;
}