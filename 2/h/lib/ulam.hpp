#ifndef ULAM_HPP
#define ULAM_HPP

#include <algorithm>

namespace ulam{


	/**
	 * Converts a prime list index to a spiral index (n is odd or even) (4)
	 *
	 *
	 */

	size_t stol(size_t i, size_t n){
		size_t line = i / n;
		size_t col = i % n;

		if(n % 2 == 0){
			line = n - line - 1;
			col = n - col - 1;
		}

		size_t ring_index, side_col = 1, side_line = 1;

		if(col < n - line){
			ring_index = std::min(line, col);
		}
		else if(col > line){
			ring_index = n - col - 1;
			side_line = -1;
		}
		else{
			ring_index = n - line - 1;
			side_col = -1;
		}	

		size_t sub_size = n - ring_index * 2;

		return sub_size * sub_size - (sub_size - 1) * 2 + side_line * (line - ring_index) - side_col * (col - ring_index) - 1;
	}



}


#endif // ULAM_HPP