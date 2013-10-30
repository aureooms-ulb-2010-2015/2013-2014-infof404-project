#ifndef OS_TASK_T_H
#define OS_TASK_T_H

namespace os{
	typedef struct task_t{
		uint offset;
		uint period;
		uint deadline;
		uint wcet;

		task_t(){}
		task_t(uint offset, uint period, uint deadline, uint wcet)
			:offset(offset), period(period), deadline(deadline), wcet(wcet){}
		task_t(uint array[4])
			:offset(array[0]), period(array[1]), deadline(array[2]), wcet(array[3]){}

		template<typename O>
		friend O& operator<< (O &out, const task_t& task) {
			out << task.offset << ' '
				<< task.period << ' '
				<< task.deadline << ' '
				<< task.wcet;
			return out;
		}
	} task_t;
}


#endif // OS_TASK_T_H