#ifndef OS_STUDY_H
#define OS_STUDY_H

#include <algorithm>
#include <functional>

namespace os{
template<typename G, typename S, typename N, typename U, typename D, typename I, typename T, typename LCM>
	void study_scheduler(G& generator, S& scheduler, const N& n, const U& u, const D& d, const I m, T& task_system, LCM acc){
		study_scheduler(generator, scheduler, n, u, d, m, task_system, acc, [](uint, uint, uint, bool, double){});
	}

	template<typename G, typename S, typename N, typename U, typename D, typename I, typename T, typename LCM>
	void study_scheduler(G& generator, S& scheduler, const N& n, const U& u, const D& d, const I m, T&, LCM acc, std::function<void(uint, uint, uint, bool, double)> callback){
		for(size_t i = 0; i < u.size(); ++i){
			const auto& x = u[i];
			for(size_t j = 0; j < n.size(); ++j){
				const auto& y = n[j];
				for(I l = 0; l < m; ++l){
					T task_system;
					generator.next(x, y, task_system);
					uint lcm = acc(task_system);
					for(size_t k = 0; k < d.size(); ++k){
						const auto& z = d[k];
						scheduler.reset();
						scheduler.init(task_system);
						scheduler.run(z, lcm);
						callback(i, j, k, scheduler.schedulable, ((double)scheduler.preempted) / scheduler.i);
					}
				}
			}
		}
	}
}


#endif // OS_STUDY_H