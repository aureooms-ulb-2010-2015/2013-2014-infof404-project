#ifndef OS_STUDY_H
#define OS_STUDY_H

#include <algorithm>

namespace os{
	template<typename G, typename S, typename N, typename U, typename D, typename I, typename B, typename T>
	void study_scheduler(G& generator, S& scheduler, const N& n, const U& u, const D& d, const I m, B& benchmark){
		for(auto& x : u){
			for(auto& y : n){
				for(auto& z : d){
					for(I i = 0; i < m; ++i){
						T task_system;
						generator.next(x, y, task_system);
						scheduler.reset();
						scheduler.init(task_system);
						scheduler.run(z, task_system.lcm);
						benchmark.add(x, y, z, scheduler.schedulable, scheduler.preempted);
					}
				}
			}
		}
	}
}


#endif // OS_STUDY_H