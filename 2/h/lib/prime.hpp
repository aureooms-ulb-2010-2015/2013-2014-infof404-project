#ifndef PRIME_HPP
#define PRIME_HPP

namespace prime{


	/**
	 * Recursive binary search template function. (1)
	 *
	 *
	 */

	template<typename A, typename V>
	bool binary_search(const A& array, size_t i, size_t j, const V& value){
		if(i == j) return false;
		const size_t p = i + (j - i) / 2;
		if(value == array[p]) return true;
		else if(value < array[p]) return binary_search(array, i, p, value);
		else return binary_search(array, p + 1, j, value);
	}

	/**
	 * Linear search template function. (1)
	 *
	 *
	 */

	template<typename A, typename V>
	bool linear_search(const A& array, size_t i, size_t j, const V& value){
		for(; i < j; ++i){
			if(array[i] == value) return true;
		}
		return false;
	}



}


#endif // PRIME_HPP