#ifndef ERATOSTHENE_HPP
#define ERATOSTHENE_HPP

namespace eratosthene{


	template<typename A>
	inline void go_through(size_t i, size_t k, const size_t count, A& prime){
		for(; i < count; i += k) prime[i] = false;
	}

	inline size_t number_to_index(size_t n){
		return (n - 1) / 2 - 1;
	}

	inline size_t index_to_number(size_t i){
		return 2 * (i + 1) + 1;
	}
}

#endif // ERATOSTHENE_HPP