#ifndef OS_SCHEDULER_H
#define OS_SCHEDULER_H

#include <algorithm>
#include <map>
#include <iostream>

namespace os{
	template<typename S>
	class llf_scheduler{
	private:
		typedef typename S::value_type task_t;
		typedef std::multimap<uint, task_t*> queue_t;
		typedef typename queue_t::iterator queue_iterator;
		typedef typename queue_t::const_iterator queue_const_iterator;
		queue_t queue;
		queue_const_iterator current;
		S* task_system;
		uint idle = 0, preempted = 0;

	public:
		void reset(){
			queue.clear();
			idle = 0;
			preempted = 0;
		}
		void init(S& task_system){
			this->task_system = &task_system;
		}
		void run(uint delta, uint lcm){
			for(uint i = 0; i < lcm; ++i){
				if(i % delta == 0){
					for(task_t& task : *task_system){
						if(i > task.offset && (i - task.offset) % task.period == 0){
							queue.emplace(i + task.deadline - task.wcet, &task);
						}
					}

					current = queue.cbegin();
				}

				if(current != queue.cend()){
					if(i > current->first){
						std::cout << "error" << std::endl;
						break;
					}
					else if(i + current->second->deadline - current->first > 1){
						queue_iterator it = queue.emplace(current->first + 1, nullptr);
						std::swap(it->second, current->second);
						queue.erase(current);
						current = it;
					}
					else{
						queue.erase(current);
						current = queue.cbegin();
					}
				}
				else{
					++idle;
					std::cout << "idle" << std::endl;
				}
			}
		}

	};
}


#endif // OS_SCHEDULER_H