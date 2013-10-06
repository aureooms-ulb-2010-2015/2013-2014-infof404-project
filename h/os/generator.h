#ifndef OS_GENERATOR_H
#define OS_GENERATOR_H

#include <algorithm>

namespace os{
	template<typename G, typename D,typename U, typename N, typename S>
	void generate_task_system(G& generator, D& distribution, const U usage, const N n, S& task_system){
		if(n > 0){
			U left = usage;
			N i = 0;
			while(i < n-1){
				U u = distribution(generator) % (left - n + i + 1);
				left -= u;
				uint offset = 0;
				uint period = 100;
				uint deadline = u + distribution(generator) % (period - u + 1);
				uint wcet = u;
				task_system.emplace_back(offset, period, deadline, wcet);
				++i;
			}
			uint offset = 0;
			uint period = 100;
			uint deadline = left + distribution(generator) % (period - left + 1);
			uint wcet = left;
			task_system.emplace_back(offset, period, deadline, wcet);
		}
	}

	template<typename G, typename D>
	class task_system_generator{
	private:
		G* generator;
		D* distribution;
	public:
		task_system_generator(G& generator, D& distribution):
		generator(&generator), distribution(&distribution){}

		template<typename U, typename N, typename S>
		void next(const U u, const N n, S& task_system){
			generate_task_system(*generator, *distribution, u, n, task_system);
		}
	};

}


#endif // OS_GENERATOR_H