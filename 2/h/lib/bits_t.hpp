#ifndef LIB_BITS_T_H
#define LIB_BITS_T_H

#include <algorithm>
template <typename B>

class bits_t{

const B mask[64];

public:

	typedef B block_t;
	static const size_t block_size = sizeof(block_t) * 8;

	block_t* data;
	size_t size;
	size_t blocks;

	class bit_ref{
	public:
		block_t* const block;
		const block_t mask;

		bit_ref(block_t& block, const block_t mask) noexcept : block(&block), mask(mask){}

		inline void operator=(bool v) const noexcept{
			if(v) *block |= mask;
			else  *block &= ~mask;
		}

		inline operator bool() const noexcept{
			return (bool)(*block & mask);
		}
	};



	bits_t() noexcept : mask({
		0b0000000000000000000000000000000000000000000000000000000000000001,
		0b0000000000000000000000000000000000000000000000000000000000000010,
		0b0000000000000000000000000000000000000000000000000000000000000100,
		0b0000000000000000000000000000000000000000000000000000000000001000,
		0b0000000000000000000000000000000000000000000000000000000000010000,
		0b0000000000000000000000000000000000000000000000000000000000100000,
		0b0000000000000000000000000000000000000000000000000000000001000000,
		0b0000000000000000000000000000000000000000000000000000000010000000,
		0b0000000000000000000000000000000000000000000000000000000100000000,
		0b0000000000000000000000000000000000000000000000000000001000000000,
		0b0000000000000000000000000000000000000000000000000000010000000000,
		0b0000000000000000000000000000000000000000000000000000100000000000,
		0b0000000000000000000000000000000000000000000000000001000000000000,
		0b0000000000000000000000000000000000000000000000000010000000000000,
		0b0000000000000000000000000000000000000000000000000100000000000000,
		0b0000000000000000000000000000000000000000000000001000000000000000,
		0b0000000000000000000000000000000000000000000000010000000000000000,
		0b0000000000000000000000000000000000000000000000100000000000000000,
		0b0000000000000000000000000000000000000000000001000000000000000000,
		0b0000000000000000000000000000000000000000000010000000000000000000,
		0b0000000000000000000000000000000000000000000100000000000000000000,
		0b0000000000000000000000000000000000000000001000000000000000000000,
		0b0000000000000000000000000000000000000000010000000000000000000000,
		0b0000000000000000000000000000000000000000100000000000000000000000,
		0b0000000000000000000000000000000000000001000000000000000000000000,
		0b0000000000000000000000000000000000000010000000000000000000000000,
		0b0000000000000000000000000000000000000100000000000000000000000000,
		0b0000000000000000000000000000000000001000000000000000000000000000,
		0b0000000000000000000000000000000000010000000000000000000000000000,
		0b0000000000000000000000000000000000100000000000000000000000000000,
		0b0000000000000000000000000000000001000000000000000000000000000000,
		0b0000000000000000000000000000000010000000000000000000000000000000,
		0b0000000000000000000000000000000100000000000000000000000000000000,
		0b0000000000000000000000000000001000000000000000000000000000000000,
		0b0000000000000000000000000000010000000000000000000000000000000000,
		0b0000000000000000000000000000100000000000000000000000000000000000,
		0b0000000000000000000000000001000000000000000000000000000000000000,
		0b0000000000000000000000000010000000000000000000000000000000000000,
		0b0000000000000000000000000100000000000000000000000000000000000000,
		0b0000000000000000000000001000000000000000000000000000000000000000,
		0b0000000000000000000000010000000000000000000000000000000000000000,
		0b0000000000000000000000100000000000000000000000000000000000000000,
		0b0000000000000000000001000000000000000000000000000000000000000000,
		0b0000000000000000000010000000000000000000000000000000000000000000,
		0b0000000000000000000100000000000000000000000000000000000000000000,
		0b0000000000000000001000000000000000000000000000000000000000000000,
		0b0000000000000000010000000000000000000000000000000000000000000000,
		0b0000000000000000100000000000000000000000000000000000000000000000,
		0b0000000000000001000000000000000000000000000000000000000000000000,
		0b0000000000000010000000000000000000000000000000000000000000000000,
		0b0000000000000100000000000000000000000000000000000000000000000000,
		0b0000000000001000000000000000000000000000000000000000000000000000,
		0b0000000000010000000000000000000000000000000000000000000000000000,
		0b0000000000100000000000000000000000000000000000000000000000000000,
		0b0000000001000000000000000000000000000000000000000000000000000000,
		0b0000000010000000000000000000000000000000000000000000000000000000,
		0b0000000100000000000000000000000000000000000000000000000000000000,
		0b0000001000000000000000000000000000000000000000000000000000000000,
		0b0000010000000000000000000000000000000000000000000000000000000000,
		0b0000100000000000000000000000000000000000000000000000000000000000,
		0b0001000000000000000000000000000000000000000000000000000000000000,
		0b0010000000000000000000000000000000000000000000000000000000000000,
		0b0100000000000000000000000000000000000000000000000000000000000000,
		0b1000000000000000000000000000000000000000000000000000000000000000
	}), data(nullptr){}

	void resize(const size_t n, const bool v) noexcept{
		block_t fill = v ? ~block_t(0) : block_t(0);
		size = n;
		blocks = (n + block_size - 1) / block_size;
		data = new block_t[blocks];
		std::fill(data, data + blocks, fill);
	}
	
	inline block_t& block_at_index(const size_t i) const noexcept{
		return data[i / block_size];
	}

	inline size_t rindex_in_block(const size_t i) const noexcept{
		return block_size - ((i % block_size) + 1);;
	}

	inline size_t index_in_block(const size_t i) const noexcept{
		return i % block_size;
	}

	inline bit_ref operator[](const size_t i) noexcept{
		return bit_ref(block_at_index(i), mask[index_in_block(i)]);
	}

	~bits_t(){
		delete[] data;
	}

};

#endif // LIB_BITS_T_H
