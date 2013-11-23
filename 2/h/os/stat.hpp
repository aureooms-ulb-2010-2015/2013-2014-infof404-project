#ifndef OS_STAT_HPP
#define OS_STAT_HPP

#include <iostream>


namespace os{
	namespace stat{

		inline void print(size_t last, size_t prime_n){
			std::cout << "Generated primes : " << prime_n << std::endl;
			std::cout << "Composite numbers : " << (last - prime_n) << std::endl;
			std::cout << "Total : " << last << std::endl;
		}

	}
}



#endif // OS_STAT_HPP