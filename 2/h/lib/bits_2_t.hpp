#ifndef LIB_BITS_2_T_H
#define LIB_BITS_2_T_H

#include <algorithm>



template <typename B>

class bits_2_t{

public:
	typedef B block_t;
	static const size_t block_size = sizeof(block_t) * 8;

private:
	block_t* _data;
	size_t _size;
	size_t _blocks;

public:

	class bit_ref{
	public:
		block_t* const block;
		const block_t mask;

		bit_ref(block_t& block, const block_t mask) noexcept : block(&block), mask(mask){}

		inline bool operator= (bool v) const noexcept{
			return v ? *block |= mask : *block &= ~mask;
		}

		inline operator bool() const noexcept{
			return (bool)(*block & mask);
		}
	};

	bits_2_t () noexcept : _data(nullptr){}

	bits_2_t (const size_t n) noexcept : _data(nullptr){
		const block_t fill(0);
		_size = n;
		_blocks = number_of_blocks_needed(n);
		_data = new block_t[_blocks];
		std::fill(_data, _data + _blocks, fill);
	}

	bits_2_t(const size_t n, const bool v) noexcept : _data(nullptr){
		const block_t fill = v ? ~block_t(0) : block_t(0);
		_size = n;
		_blocks = number_of_blocks_needed(n);
		_data = new block_t[_blocks];
		std::fill(_data, _data + _blocks, fill);
	}

	void resize(const size_t n) noexcept{
		resize(n, false);
	}

	void resize(const size_t n, const bool v) noexcept{
		const block_t fill = v ? ~block_t(0) : block_t(0);
		const size_t tmpblocks = number_of_blocks_needed(n);
		const size_t copysize = std::min(_blocks, tmpblocks);
		block_t* tmpdata = new block_t[tmpblocks];
		std::copy(_data, _data + copysize, tmpdata);
		std::fill(tmpdata + copysize, tmpdata + tmpblocks, fill);
		delete[] _data;
		_data = tmpdata;
		_blocks = tmpblocks;
		_size = n;
	}

	void clear() noexcept{
		delete[] _data;
		_size = 0;
		_blocks = 0;
		_data = nullptr;
	}

	inline size_t number_of_blocks_needed (const size_t n) const noexcept {
		return (n + block_size - 1) / block_size;
	}
	
	inline block_t& block_at_index(const size_t i) const noexcept{
		return _data[i / block_size];
	}
	
	inline size_t index_in_block(const size_t i) const noexcept{
		return i % block_size;
	}

	inline bit_ref operator[](const size_t i) noexcept{
		return bit_ref(block_at_index(i), block_t(1) << index_in_block(i));
	}

	inline bool operator[](const size_t i) const noexcept{
		return (bool)(block_at_index(i) & (block_t(1) << index_in_block(i)));
	}

	block_t* data(){
		return _data;
	}

	const block_t* data() const{
		return _data;
	}

	const size_t size() const{
		return _size;
	}

	~bits_2_t(){
		clear();
	}

};

#endif // LIB_BITS_2_T_H
