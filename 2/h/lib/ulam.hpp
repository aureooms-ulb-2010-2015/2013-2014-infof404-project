#ifndef ULAM_HPP
#define ULAM_HPP

#include <algorithm>

namespace ulam{


	/**
	 * Converts a prime list index to a spiral index (n is odd) (4)
	 *
	 *
	 */

	size_t stol(size_t i, size_t n){
		size_t line = i / n;
		size_t col = i % n;

		size_t ring_index, side_col, side_line;

		if(col < n - line){
			ring_index = std::min(line, col);
			side_col = 1;
			side_line = 1;
		}
		else{
			ring_index = n - col - 1;
			side_col = -1;
			if(line < n - col) side_line = 1;
			else side_line = -1;
		}	

		size_t sub_size = n - ring_index * 2;

		return sub_size * sub_size - (sub_size - 1) * 2 + side_line * line - side_col * col; // -1
	}



}


#endif // ULAM_HPP