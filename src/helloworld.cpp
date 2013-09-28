#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include "os/task_t.h"
#include "os/task_system_t.h"
#include "os/parse.h"
#include "os/generator.h"
#include "os/scheduler.h"
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

	{
		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		uint seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_int_distribution<uint> distribution(0,100);
		uint usage = 70;
		uint n = 5;
		os::generate_task_system(generator, distribution, usage, n, task_system);

		std::cout << task_system << std::endl;
		uint u = 0;
		for(auto task : task_system){
			u += task.wcet;
		}
		
		std::cout << u << std::endl;
	}
	return 0;
}