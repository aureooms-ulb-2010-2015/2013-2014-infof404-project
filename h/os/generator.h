#ifndef OS_GENERATOR_H
#define OS_GENERATOR_H

#include <algorithm>
#include <set>

namespace os{
	template<typename G, typename D,typename U, typename N, typename S>
	void generate_task_system(G& generator, D& distribution, const U usage, const N n, S& task_system){
		if(n > 0){
			double sep[n+1] = {};
			for(N i = 1; i < n; ++i){
				sep[i] = rate_distribution(generator);
			}
			for(N i = 0; i < n; ++i){
				uint offset = 0;
				uint period = period_distribution(generator);
				uint wcet = (sep[i+1] - sep[i])*period;
				uint deadline = wcet + period_distribution(generator) % (period - wcet + 1);
			}
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