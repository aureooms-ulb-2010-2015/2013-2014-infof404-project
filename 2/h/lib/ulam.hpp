#ifndef ULAM_HPP
#define ULAM_HPP

#include <algorithm>
#include <cmath>

namespace ulam{


	/**
	 * Converts list index to a spiral index. (2)
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

	/**
	 * Converts a spiral index to a list index. (4)
	 *
	 *
	 */

	size_t ltos(size_t i, size_t n){
		size_t s = std::sqrt(i), l = s, r = s + 1, L = l * l, R = r * r, x = (n - 1) / 2, y = n / 2;
		if(s % 2 == 1){
			if(i < (L + R) / 2){
				x += s / 2 + 1;
				y += l / 2 + L - i;
			}
			else{
				x -= r / 2 - R + i;
				y -= s / 2 + 1;
			}
		}
		else{
			if(i < (L + R) / 2){
				x -= s / 2;
				y -= l / 2 + L - i;
			}
			else{
				x += r / 2 + 1 - R + i;
				y += s / 2;
			}
		}

		return n * y + x;
	}



}


#endif // ULAM_HPP