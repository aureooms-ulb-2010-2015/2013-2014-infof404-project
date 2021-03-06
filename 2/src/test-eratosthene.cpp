#include <iostream>
#include <vector>

typedef std::vector<uint8_t> array_t;

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

	// for(size_t i = 2; i < count; ++i){
	// 	if(prime[i]) std::cout << i << std::endl;
	// }

	return 0;
}