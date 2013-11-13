#include <iostream>

#include "lib/ulam.hpp"

int main(int argc, char const *argv[]){

	if(argc < 2){
		std::cout << "#0 missing" << std::endl;
		return 1;
	}

	size_t n = std::stoul(argv[1]);

	while(std::cin.good()){
		size_t i;
		std::cout << "> ";
		std::cin >> i;
		std::cout << i << ", " << ulam::stol(i, n) << std::endl;
	}


	return 0;
}