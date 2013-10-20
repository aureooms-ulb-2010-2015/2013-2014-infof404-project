#ifndef OS_STUDY_H
#define OS_STUDY_H

#include <algorithm>

namespace os{
	template<typename G, typename S, typename N, typename U, typename D, typename I, typename B, typename T, typename LCM>
	void study_scheduler(G& generator, S& scheduler, const N& n, const U& u, const D& d, const I m, B& benchmark, T&, LCM acc){
		for(size_t i = 0; i < u.size(); ++i){
			const auto& x = u[i];
			for(size_t j = 0; j < n.size(); ++j){
				const auto& y = n[j];
				for(size_t k = 0; k < d.size(); ++k){
					const auto& z = d[k];
					for(I l = 0; l < m; ++l){
						T task_system;
						generator.next(x, y, task_system);
						uint lcm = acc(task_system);
						scheduler.reset();
						scheduler.init(task_system);
						scheduler.run(z, lcm);
						benchmark.emplace_back(i, j, k, scheduler.schedulable, scheduler.preempted);
					}
				}
			}
		}
	}
}


#endif // OS_STUDY_H