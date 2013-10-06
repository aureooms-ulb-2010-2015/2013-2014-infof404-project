#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include "os/job_t.h"
#include "os/task_t.h"
#include "os/task_system_t.h"
#include "os/parse.h"
#include "os/generator.h"
#include "os/scheduler.h"
#include "os/study.h"
#include "os/benchmark_t.h"
#include "os/benchmark_node_t.h"
#include "os/lcm.h"
#include "lib/io.h"

int main(){

	{
		std::cout << "TASK PRINT TEST" << std::endl;
		os::task_t task;
		std::cout << task << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "TASK SYSTEM PRINT TEST" << std::endl;
		os::task_system_t task_system(5);
		std::cout << task_system << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "READ FILE TEST" << std::endl;
		std::ifstream ifs("system/0", std::ifstream::in);
		char c = ifs.get();
		while (ifs.good()){
			std::cout << c;
			c = ifs.get();
		}
		ifs.close();
		std::cout << '\n';
		std::cout << std::endl;
	}

	{
		std::cout << "PARSE FILE TEST" << std::endl;
		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		std::ifstream ifs("system/0", std::ifstream::in);
		os::parse_task_system_stream(ifs, task_system);
		ifs.close();
		
		std::cout << task_system << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "GENERATE SYSTEM TEST" << std::endl;
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
		std::cout << std::endl;
	}

	return 0;

	{
		std::cout << "GENERATE SYSTEM TEST 2" << std::endl;
		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		uint seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_int_distribution<uint> distribution(0,100);
		uint usage = 70;
		uint n = 5;

		typedef std::default_random_engine G;
		typedef std::uniform_int_distribution<uint> D;
		os::task_system_generator<G,D> task_system_generator(generator, distribution);
		task_system_generator.next(usage, n, task_system);

		std::cout << task_system << std::endl;
		uint u = 0;
		for(auto task : task_system){
			u += task.wcet;
		}
		
		std::cout << u << std::endl;
		std::cout << std::endl;
	}


	{
		std::cout << "SCHEDULER TEST" << std::endl;

		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		std::ifstream ifs("system/1", std::ifstream::in);
		os::parse_task_system_stream(ifs, task_system);
		ifs.close();
		
		std::cout << task_system << std::endl;

		os::llf_scheduler<os::task_system_t, os::job_t> scheduler;
		scheduler.reset();
		scheduler.init(task_system);
		scheduler.run(10u, os::task_system_period_lcm<uint, os::task_system_t>(task_system));



		std::cout << std::endl;
	}

	{
		std::cout << "STUDY TEST" << std::endl;
		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		uint seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_int_distribution<uint> distribution(0,100);

		typedef std::default_random_engine G;
		typedef std::uniform_int_distribution<uint> D;
		os::task_system_generator<G,D> task_system_generator(generator, distribution);

		os::benchmark_t benchmark;
		os::llf_scheduler<os::task_system_t, os::job_t> scheduler;

		os::study_scheduler(task_system_generator, scheduler, std::vector<uint>({4u}), std::vector<uint>({70u}), std::vector<uint>({10u}), 100, benchmark, task_system, os::task_system_period_lcm<uint, os::task_system_t>);

		double avg = 0, tot = 0;
		for(os::benchmark_node_t& x : benchmark){
			if(x.schedulable) ++avg;
			++tot;
		}

		std::cout << avg << " / " << tot << " : " << (avg/tot*100) << "%"<< std::endl;

		std::cout << std::endl;
	}
	return 0;
}