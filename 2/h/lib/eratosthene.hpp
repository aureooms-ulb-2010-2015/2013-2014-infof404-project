#ifndef ERATOSTHENE_HPP
#define ERATOSTHENE_HPP

namespace eratosthene{


	template<typename A>
	inline void go_through(size_t i, const size_t k, const size_t count, A& prime){
		for(; i < count; i += k){
			std::cout << "cross " << i << std::endl;
			prime[i] = false;
		}
	}

	inline size_t number_to_index(size_t n){
		return (n - 1) / 2 - 1;
	}

	inline size_t index_to_number(size_t i){
		return 2 * (i + 1) + 1;
	}


	inline size_t number_to_index_23_0(size_t n){
		return (n + 1) / 3 - 2;
	}

	inline size_t number_to_index_23_1(size_t n){
		return (n - 1) / 3 - 1;
	}

	inline size_t index_to_number_23_0(size_t i){
		return 6 * (i / 2 + 1) - 1;
	}
	inline size_t index_to_number_23_1(size_t i){
		return 6 * (i / 2 + 1) + 1;
	}

	template<typename A>
	class worker{

	};

	template<typename A, typename S>
	class scheduler{
	public:
		S current; // should be std::map<size_t, size_t>
		size_t next;
		A* prime;

		scheduler(A& prime):prime(&prime){}

		size_t get(){
			//lock
			while(true){
				// wait for signal
				while(!prime[next]) ++next;
				auto& it = current.begin();
				size_t i = it->first;
				if(i * i > next){
					if(prime[next]) break;
					else continue;
				}
				for(; it->first * it->first <= next; ++it){
					if(it->second < next) continue;
				}
				if(prime[next]) break;
			}
			size_t ret = next;
			++next;
			//release
			return ret;
		}

	};
}

#endif // ERATOSTHENE_HPP