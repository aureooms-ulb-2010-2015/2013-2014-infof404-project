#ifndef OS_TASK_SYSTEM_T_H
#define OS_TASK_SYSTEM_T_H

#include "os/task_t.h"
#include <vector>

namespace os{
	typedef std::vector<task_t> task_system_t;
}

template<typename O>
O& operator<< (O &out, const os::task_system_t& task_system) {

	for(const auto& task : task_system){
		out << task << std::endl;
	}
	return out;
}


#endif // OS_TASK_SYSTEM_T_H