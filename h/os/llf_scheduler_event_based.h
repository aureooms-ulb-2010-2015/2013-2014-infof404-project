#ifndef OS_LLF_SCHEDULER_EVENT_BASED_H
#define OS_LLF_SCHEDULER_EVENT_BASED_H

#include <algorithm>
#include <map>
#include <queue>
#include <iostream>
#include <functional>

#include "lib/io.h"

namespace os{
	template<typename S, typename J>
	class llf_scheduler_event_based{

	private:
		typedef typename S::value_type task_t;

	// events
	private:
		enum event_id{
			NEW_JOB,
			CHECK_PRIORITIES,
			END_OF_INTERVAL
		};

		typedef struct event_t{
			event_id id;
			size_t task_id;
			task_t* task;

			event_t(event_id id, size_t task_id, task_t* task):id(id),task_id(task_id),task(task){}
			template<typename O>
			friend O& operator<< (O &out, const event_t& event) {
				out << '(';
				if(event.id == NEW_JOB) out << "NEW_JOB";
				else if(event.id == CHECK_PRIORITIES) out << "CHECK_PRIORITIES";
				else out << "END_OF_INTERVAL";
				out << ", ";
				if(event.task) out << *event.task;
				else out << "None";
				out << ')';
				return out;
			}
		} event_t;



	private:
		typedef std::pair<uint, J> node_t;
		typedef std::multimap<uint, J> queue_t;
		typedef typename queue_t::iterator queue_iterator;
		typedef typename queue_t::const_iterator queue_const_iterator;
		queue_t queue;
		queue_iterator current;
		S* task_system;

		typedef std::multimap<uint, event_t> events_t;
		typedef typename events_t::value_type event_p;
		events_t events;
		typedef std::pair<typename events_t::iterator, typename events_t::iterator> events_r;

	public:
		uint idle = 0, preempted = 0;
		bool schedulable = true;

		void reset(){
			queue.clear();
			events.clear();
			current = queue.begin();
			preempted = 0;
			schedulable = true;
		}
		void init(S& task_system){
			this->task_system = &task_system;

			size_t task_id = 0;
			for(task_t& task : task_system){
				events.insert(event_p(task.offset, event_t(NEW_JOB, task_id, &task)));
				++task_id;
			}
			events.insert(event_p(0, event_t(CHECK_PRIORITIES, -1, nullptr)));

		}

		void run(uint delta, uint lcm){
			run(delta, lcm, [](size_t, size_t, size_t, size_t){});
		}

		void run(uint delta, uint lcm, std::function<void(size_t, size_t, size_t, size_t)> callback){

			events.insert(event_p(lcm - 1, event_t(END_OF_INTERVAL, -1, nullptr)));

			uint next = 0;
			uint i = 0;
			while(i < lcm){

				bool new_job = false;
				bool check_priorities = false;
				events_r range = events.equal_range(i);
				const size_t count = events.count(i);
				size_t j = 0;
				typename events_t::iterator it = events.begin();
				for(;j < count; ++j){
					switch(it->second.id){
						case NEW_JOB:{
							queue.insert(node_t(i + it->second.task->deadline - it->second.task->wcet, J(it->second.task_id, i, it->second.task->wcet, i + it->second.task->deadline)));
							callback(0, it->second.task_id, i, 0);
							callback(1, it->second.task_id, i + it->second.task->deadline, 0);
							new_job = true;
							events.insert(event_p(i + it->second.task->period, event_t(NEW_JOB, it->second.task_id, it->second.task)));
							break;
						}

						case CHECK_PRIORITIES:{
							check_priorities = true;
							events.insert(event_p(i + delta, event_t(CHECK_PRIORITIES, -1, nullptr)));
							break;
						}

						case END_OF_INTERVAL:{
							break;
						}
					}
					typename events_t::iterator prev = it;
					++it;
					events.erase(prev);
				}

				// if there was no current job and a new job arrived
				if(current == queue.end() && new_job) current = queue.begin();

				// else if there was a current job and it's time to check priorities
				else if(check_priorities && current != queue.begin() && current != queue.end()){
					++preempted;
					current = queue.begin();
				}

				next = events.begin()->first;

				// if there is a current job
				if(current != queue.end()){
					// deadline missed
					if(i > current->first){
						schedulable = false;
						callback(3, current->second.id, i, 0);
						break;
					}
					// there is still work to do
					else if(current->second.d - current->first > (next - i)){
						queue_iterator it = queue.insert(node_t(current->first + (next - i), current->second));
						queue.erase(current);
						current = it;
						callback(2, current->second.id, i, next);
						i = next;
					}
					// current job done
					else{
						callback(2, current->second.id, i, i + current->second.d - current->first);
						i += current->second.d - current->first;
						queue.erase(current);
						current = queue.begin();
					}
				}
				else{
					idle += next - i;
					i = next;
				}
			}
		}

	};
}


#endif // OS_LLF_SCHEDULER_EVENT_BASED_H