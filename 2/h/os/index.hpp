#ifndef OS_INDEX_HPP
#define OS_INDEX_HPP

#include <cmath>
#include "os/global.hpp"
#include "lib/prime.hpp"

namespace os{
	namespace index{


		inline size_t local_count(size_t mpi_rank, size_t r, size_t count, size_t& o){
			if(r > mpi_rank){
				if(count % 2 == 1){
					++count;
					o = mpi_rank * count;
				}
				else if(mpi_rank % 2 == 0){
					count += 2;
					o = mpi_rank * (count - 1);
				}
				else{
					o = mpi_rank * (count + 1) + 1;
				}
				
			}
			else{
				if(count % 2 == 1){
					if((mpi_rank - r) % 2 == 0){
						o = r * (count + 1) + (mpi_rank - r) * count;
						++count;
					}
					else{
						o = r * (count + 1) + (mpi_rank - r) * count + 1;
						--count;
					}
				}
				else if(r % 2 == 0){
					o = r * (count + 1) + (mpi_rank - r) * count;
				}
				else{
					o = r * (count + 1) + (mpi_rank - r) * count + 1;
				}
				
			}

			return count;
		}

		inline size_t total_count(const size_t last){
			size_t tmp = last / 6;
			if(last % 6 <= 4) return 2 * tmp + 1;
			else return 2 * tmp + 2;
		}
		inline size_t size(const size_t nth){
			size_t tmp = prime::upper_bound(nth);
			return (tmp == 0) ? 0 : std::sqrt(tmp - 1) + 1;
		}
		inline size_t last(const size_t size){
			return size * size;
		}

		template<typename A>
		inline size_t who(size_t i, const A& partition, size_t n){
			for(size_t k = n - 1; k > 0; --k){
				if(partition[k] <= i) return k;
			}
			return 0;
		}


	}
}




#endif // OS_INDEX_HPP