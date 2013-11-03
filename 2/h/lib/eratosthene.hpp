#ifndef ERATOSTHENE_HPP
#define ERATOSTHENE_HPP

namespace eratosthene{


	template<typename A>
	inline void go_through(size_t i, size_t k, const size_t count, A& prime){
		for(; i < count; i += k) prime[i] = false;
	}
}

#endif // ERATOSTHENE_HPP