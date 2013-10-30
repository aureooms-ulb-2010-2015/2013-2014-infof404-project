#include <iostream>
#include <vector>
#include <numeric>

#include "lib/prime.hpp"

int main(/*int argc, char const *argv[]*/){
	std::cout << "hello, world" << std::endl;

	uint k;
	const size_t size = 10000;
	std::vector<uint> array(size, 0);
	std::iota(array.begin(), array.end(), 0);

	while(std::cin.good()){
		std::cin >> k;
		std::cout << std::boolalpha << prime::binary_search(array, 0, size, k) << std::endl;
	}

	return 0;
}