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


template<typename I>
I& operator>> (I& stream, os::task_system_t& task_system){
	char pt;
	size_t i = 0;
	bool skip = true;
	uint current[4] = {};
	while(true){
		stream.get(pt);
		if(!stream.good()) break;
		switch(pt){
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':{
				skip = false;
				current[i] *= 10;
				current[i] += (pt - '0');
				break;
			}

			default:{
				if(!skip){
					++i;
					if(i == 4){
						task_system.emplace_back(current);
						i = 0;
						skip = true;
						std::fill_n(current, 4, 0);
					}
				}
			}
		}
	}
	if(!skip) task_system.emplace_back(current);

	return stream;
}


#endif // OS_TASK_SYSTEM_T_H