#ifndef OS_LCM_H
#define OS_LCM_H

#include "lib/num.h"

namespace os{
	template<typename I, typename S>
	I task_system_period_lcm(S& task_system){
		I i = 1;
		for(auto& x : task_system){
			i = lib::lcm(i, x.period);
		}
		return i;
	}
}


#endif // OS_LCM_H