#ifndef OS_PARSE_TASK_SYSTEM_STREAM_H
#define OS_PARSE_TASK_SYSTEM_STREAM_H

namespace os{
	template<typename I, typename S>
	void parse_task_system_stream(I& stream, S& system){
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
							system.emplace_back(current);
							i = 0;
							skip = true;
							current = {};
						}
					}
				}
			}
		}
		if(!skip) system.emplace_back(current);
	}
}


#endif // OS_PARSE_TASK_SYSTEM_STREAM_H