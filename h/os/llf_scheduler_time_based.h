#ifndef OS_LLF_SCHEDULER_TIME_BASED_H
#define OS_LLF_SCHEDULER_TIME_BASED_H

#include <algorithm>
#include <map>
#include <iostream>
#include <functional>

#include "lib/io.h"

namespace os{
	template<typename S, typename J>
	class llf_scheduler_time_based{
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
			preempted = 0;
			schedulable = true;
		}
		void init(S& task_system){
			this->task_system = &task_system;
		}

		void run(uint delta, uint lcm){
			run(delta, lcm, [](size_t, size_t, size_t, size_t){});
		}

		void run(uint delta, uint lcm, std::function<void(size_t, size_t, size_t, size_t)> callback){
			for(uint i = 0; i < lcm; ++i){

				::operator<<(std::cout << "queue -> ", queue) << std::endl;
				std::cout << i << " -> ";

				// check for new jobs
				bool new_job = false;
				size_t id = 0;
				for(task_t& task : *task_system){
					if(i >= task.offset && (i - task.offset) % task.period == 0){
						queue.insert(node_t(i + task.deadline - task.wcet, J(id, i, task.wcet, i + task.deadline)));
						std::cout << "new job, ";
						callback(0, id, i, 0);
						callback(1, id, i + task.deadline, 0);
						new_job = true;
					}
					++id;
				}

				// if there was no current job and a new job arrived
				if(current == queue.end() && new_job) current = queue.begin();

				// else if there was a current job and it's time to check priorities
				else if(i % delta == 0 && current != queue.begin() && current != queue.end()){
					++preempted;
					std::cout << "preempted, ";
					current = queue.begin();
				}

				// if there is a current job
				if(current != queue.end()){
					// deadline missed
					if(i > current->first){
						schedulable = false;
						std::cout << "error" << std::endl;
						callback(3, current->second.id, i, 0);
						break;
					}
					// there is still work to do
					else if(current->second.d - current->first > 1){
						queue_iterator it = queue.insert(node_t(current->first + 1, current->second));
						queue.erase(current);
						current = it;
						std::cout << "work" << std::endl;
						callback(2, current->second.id, i, i+1);
					}
					// current job done
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


#endif // OS_LLF_SCHEDULER_TIME_BASED_H