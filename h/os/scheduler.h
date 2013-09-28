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
		typedef std::pair<uint, task_t*> node_t;
		typedef std::multimap<uint, task_t*> queue_t;
		typedef typename queue_t::iterator queue_iterator;
		typedef typename queue_t::const_iterator queue_const_iterator;
		queue_t queue;
		queue_iterator current;
		S* task_system;
		uint idle = 0, preempted = 0;

	public:
		void reset(){
			queue.clear();
			current = queue.begin();
			idle = 0;
			preempted = 0;
		}
		void init(S& task_system){
			this->task_system = &task_system;
		}
		void run(uint delta, uint lcm){
			for(uint i = 0; i < lcm; ++i){
				std::cout << i << " -> ";
				if(i % delta == 0){
					for(task_t& task : *task_system){
						if(i >= task.offset && (i - task.offset) % task.period == 0){
							queue.insert(node_t(i + task.deadline - task.wcet, &task));
							std::cout << "new job, ";
						}
					}

					if(current != queue.begin() && current != queue.end()){
						++preempted;
						std::cout << "preempted, ";
					}
					current = queue.begin();

				}

				if(current != queue.end()){
					if(i > current->first){
						std::cout << "error" << std::endl;
						break;
					}
					else if(i + current->second->deadline - current->first > 1){
						queue_iterator it = queue.insert(node_t(current->first + 1, nullptr));
						std::swap(it->second, current->second);
						queue.erase(current);
						current = it;
						std::cout << "work" << std::endl;
					}
					else{
						queue.erase(current);
						current = queue.begin();
						std::cout << "free" << std::endl;
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