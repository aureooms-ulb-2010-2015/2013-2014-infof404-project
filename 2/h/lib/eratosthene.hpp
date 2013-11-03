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