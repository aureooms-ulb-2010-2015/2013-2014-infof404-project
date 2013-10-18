#ifndef OS_LLF_SCHEDULER_EVENT_BASED_H
#define OS_LLF_SCHEDULER_EVENT_BASED_H

#include <algorithm>
#include <map>
#include <iostream>
#include "lib/io.h"

namespace os{
	template<typename S, typename J>
	class llf_scheduler_event_based{
	private:
		typedef typename S::value_type task_t;
		typedef std::pair<uint, J> node_t;
		typedef std::multimap<uint, J> queue_t;
		typedef typename queue_t::iterator queue_iterator;
		typedef typename queue_t::const_iterator queue_const_iterator;
		queue_t queue;
		queue_iterator current;
		S* task_system;

	public:
		uint idle = 0, preempted = 0;
		bool schedulable = true;

		void reset(){
			queue.clear();
			current = queue.begin();
			idle = 0;
			preempted = 0;
			schedulable = true;
		}
		void init(S& task_system){
			this->task_system = &task_system;
		}
		void run(uint delta, uint lcm){
			for(uint i = 0; i < lcm;){

				::operator<<(std::cout << "queue -> ", queue) << std::endl;
				std::cout << i << " -> ";
				if(i % delta == 0){
					for(task_t& task : *task_system){
						if(i >= task.offset && (i - task.offset) % task.period == 0){
							queue.insert(node_t(i + task.deadline - task.wcet, J(i, task.wcet, i + task.deadline)));
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
						schedulable = false;
						std::cout << "error" << std::endl;
						break;
					}
					else if(current->second.d - current->first > delta){
						queue_iterator it = queue.insert(node_t(current->first + delta, current->second));
						queue.erase(current);
						current = it;
						std::cout << "work * " << delta << std::endl;
						i += delta;
					}
					else{
						std::cout << "work * " << (current->second.d - current->first) << ", free" << std::endl;
						i += current->second.d - current->first;
						queue.erase(current);
						current = queue.begin();
					}
				}
				else{
					idle += delta - i % delta;
					i += delta - i % delta;
					std::cout << "idle * " << (delta - i % delta) << std::endl;
				}
			}
		}

	};
}


#endif // OS_LLF_SCHEDULER_EVENT_BASED_H