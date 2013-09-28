#ifndef OS_TASK_T_H
#define OS_TASK_T_H

namespace os{
	typedef struct task_t{
		uint offset;
		uint period;
		uint deadline;
		uint wcet;
	} task_t;
}


#endif // OS_TASK_T_H