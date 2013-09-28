#ifndef OS_SCHEDULER_H
#define OS_SCHEDULER_H

#include <algorithm>
#include <multimap>
#include <iostream>

namespace os{
	template<typename S, typename T>
	class llf_scheduler{
	private:
		typedef std::multimap<uint, T*> queue_t; 
		queue_t queue;
		queue_t::const_iterator current;
		S& task_system;
		uint idle = 0, preempted = 0;

	public:
		void reset(){
			queue.clear();
			idle = 0;
			preempted = 0;
		}
		void init(S& task_system){
			this->task_system = task_system;
		}
		void run(uint delta, uint lcm){
			for(uint i = 0; i < lcm; ++i){
				if(i % delta == 0){
					for(T& task : task_system){
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
					else if(i + task->deadline - current->first > 1){
						queue_t::iterator it = queue.emplace(current->first + 1, nullptr);
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