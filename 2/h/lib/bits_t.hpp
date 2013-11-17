#ifndef LIB_BITS_T_H
#define LIB_BITS_T_H

#include <algorithm>
template <typename B>

class bits_t{

public:

	typedef B block_t;
	static const size_t block_size = sizeof(block_t) * 8;

	block_t* data = nullptr;
	size_t size;
	size_t blocks;

	class bit_ref{
	public:
		block_t* const block;
		const block_t mask;

		bit_ref(block_t& block, const block_t mask) : block(&block), mask(mask){}

		inline void operator=(bool v) const{
			if(v) *block |= mask;
			else  *block &= ~mask;
		}

		inline operator bool() const{
			return (bool)(*block & mask);
		}
	};



	bits_t(){}

	bits_t(const size_t n, const bool v){
		resize(n, v);
	}

	void resize(const size_t n, const bool v){
		block_t fill = v ? ~block_t(0) : block_t(0);
		size = n;
		blocks = (n + block_size - 1) / block_size;
		data = new block_t[blocks];
		std::fill(data, data + blocks, fill);
	}
	
	inline block_t& block_at_index(const size_t i) const{
		return data[i / block_size];
	}

	inline size_t rindex_in_block(const size_t i) const{
		return block_size - ((i % block_size) + 1);;
	}

	inline size_t index_in_block(const size_t i) const{
		return i % block_size;
	}

	inline bit_ref operator[](const size_t i){
		return bit_ref(block_at_index(i), block_t(1) << index_in_block(i));
	}

	~bits_t(){
		delete[] data;
	}

};

#endif // LIB_BITS_T_H
