#ifndef OS_GENERATOR_H
#define OS_GENERATOR_H

#include <algorithm>
#include <set>

namespace os{


	template<typename F, typename I, typename J>
	F floor_min_uniform(F v, const I p, const J min, const F u){
		J i = v * p * u;
		i %= (J)((p * u) - min);
		i += min;
		v = i;
		return v / p;
	}

	template<typename G, typename U, typename P, typename D, typename N, typename S>
	void generate_task_system(G& generator, U& usage_distribution, P& period_distribution, const D u, const N n, S& task_system){
		if(n > 0){
			std::set<D> sep;
			sep.insert(u);
			while(sep.size() < n){
				sep.insert(floor_min_uniform(usage_distribution(generator), period_distribution.min(), 1u, u));
			}

			D left = 0;
			for(D right : sep){
				uint offset = 0;
				uint period = period_distribution(generator);
				uint wcet = (right - left) * period;
				uint deadline = wcet + period_distribution(generator) % (period - wcet + 1);
				task_system.emplace_back(offset, period, deadline, wcet);

				left = right;
			}
		}
	}

	template<typename G, typename U, typename P>
	class task_system_generator{
	private:
		G* generator;
		U* usage_distribution;
		P* period_distribution;

	public:
		task_system_generator(G& generator, U& usage_distribution, P& period_distribution):
		generator(&generator),
		usage_distribution(&usage_distribution),
		period_distribution(&period_distribution){}

		template<typename D, typename N, typename S>
		void next(const D u, const N n, S& task_system){
			generate_task_system(*generator, *usage_distribution, *period_distribution, u, n, task_system);
		}
	};

}


#endif // OS_GENERATOR_H