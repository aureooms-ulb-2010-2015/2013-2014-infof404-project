#include <iostream>
#include <fstream>

#include "os/task_t.h"
#include "os/task_system_t.h"
#include "os/parse.h"
#include "lib/io.h"

int main(){
	std::cout << "Hello world!" << std::endl;

	{
		os::task_t task;
		std::cout << task << std::endl;
	}

	{
		os::task_system_t task_system(5);
		std::cout << task_system << std::endl;
	}

	{
		std::ifstream ifs("system/0", std::ifstream::in);
		char c = ifs.get();
		while (ifs.good()){
			std::cout << c;
			c = ifs.get();
		}
		ifs.close();
		std::cout << '\n';
	}

	{
		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		std::ifstream ifs("system/0", std::ifstream::in);
		os::parse_task_system_stream(ifs, task_system);
		ifs.close();
		
		std::cout << task_system << std::endl;
	}
	return 0;
}