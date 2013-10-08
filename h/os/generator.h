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
				U u = distribution(generator) % (left - n + i) + 1;
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

	template<typename G, typename D,typename U, typename N, typename S>
	void generate_task_system2(G& generator, D& distribution, const U usage, const N n, S& task_system){
		if(n > 0){
			U left = usage;
			N i = 0;
			D period_distribution(std::max(1 + (n * 100 - 1) / usage, distribution.min()),distribution.max());
			while(i < n-1){
				U p = period_distribution(generator);
				U u = distribution(generator) % (left - n + i) + 1;
				left -= u;
				uint offset = 0;
				uint period = p;
				uint deadline = u + distribution(generator) % (distribution.max() - u + 1);
				uint wcet = 1 + (u * p - 1) / distribution.max();
				task_system.emplace_back(offset, period, deadline, wcet);
				++i;
			}
			U p = period_distribution(generator);
			uint offset = 0;
			uint period = p;
			uint deadline = left + distribution(generator) % (distribution.max() - left + 1);
			uint wcet = 1 + (left * p - 1) / distribution.max();
			task_system.emplace_back(offset, period, deadline, wcet);
		}
	}

	template<typename G, typename D, typename N, typename S>
	void generate_task_system2(G& generator, D& distribution, const double usage, const N n, S& task_system){
		if(n > 0){
			double left = usage;
			N i = 0;
			while(i < n-1){
				uint r = 1.0 / left;
				double p = distribution(generator) % (distribution.max() - r) + r;
				double u = distribution(generator) % (p - n + i) + 1;
				uint offset = 0;
				uint period = p;
				uint wcet = u * p / distribution.max();
				uint deadline = wcet + distribution(generator) % (distribution.max() - u + 1);
				task_system.emplace_back(offset, period, deadline, wcet);
				++i;
				left -= (double)wcet/(double)period;
			}
			double p = period_distribution(generator);
			uint offset = 0;
			uint period = p;
			uint deadline = left + distribution(generator) % (distribution.max() - left + 1);
			uint wcet = 1 + (left * p - 1) / distribution.max();
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