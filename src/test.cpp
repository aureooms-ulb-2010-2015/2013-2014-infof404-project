#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include "os/job_t.h"
#include "os/task_t.h"
#include "os/task_system_t.h"
#include "os/parse.h"
#include "os/generator.h"
#include "os/llf_scheduler_time_based.h"
#include "os/llf_scheduler_event_based.h"
#include "os/study.h"
#include "os/benchmark_t.h"
#include "os/benchmark_node_t.h"
#include "os/lcm.h"
#include "lib/io.h"

#include "svg/namespace.hpp"

int main(){

	{
		std::cout << "SVG TEST" << std::endl;
    	svg::Dimensions dimensions(100, 100);
    	svg::Document doc("svg/my_svg.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));

    	for(size_t i = 0; i < 10; ++i){
			svg::Rectangle rectangle(svg::Point(i*10, i*10), i*10, i*20, svg::Color(i*10, i*20, i*30));
			doc << rectangle;
		}

		doc.save();
	}

	return 0;

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
		std::uniform_real_distribution<double> usage_distribution(0.0,1.0);
		std::uniform_int_distribution<uint> period_distribution(50,100);
		double u = 0.7;
		uint n = 5;
		os::generate_task_system(generator, usage_distribution, period_distribution, u, n, task_system);

		std::cout << task_system << std::endl;
		double t = 0;
		for(auto task : task_system){
			t += (double)task.wcet/(double)task.period;
		}
		
		std::cout << t << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "GENERATE SYSTEM TEST (CLASS)" << std::endl;
		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		uint seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_real_distribution<double> usage_distribution(0.0,1.0);
		std::uniform_int_distribution<uint> period_distribution(100,1000);
		double u = 0.7;
		uint n = 5;

		typedef std::default_random_engine G;
		typedef std::uniform_real_distribution<double> U;
		typedef std::uniform_int_distribution<uint> P;
		os::task_system_generator<G,U,P> task_system_generator(generator, usage_distribution, period_distribution);
		task_system_generator.next(u, n, task_system);

		std::cout << task_system << std::endl;
		double t = 0;
		for(auto task : task_system){
			t += (double)task.wcet/(double)task.period;
		}
		
		std::cout << t << std::endl;
		std::cout << std::endl;
	}


	{
		std::cout << "SCHEDULER TIME BASED TEST" << std::endl;

		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		std::ifstream ifs("system/1", std::ifstream::in);
		os::parse_task_system_stream(ifs, task_system);
		ifs.close();
		
		std::cout << task_system << std::endl;

		os::llf_scheduler_time_based<os::task_system_t, os::job_t> scheduler;
		scheduler.reset();
		scheduler.init(task_system);
		scheduler.run(10u, os::task_system_period_lcm<uint, os::task_system_t>(task_system));



		std::cout << std::endl;
	}

	{
		std::cout << "SCHEDULER EVENT BASED TEST" << std::endl;

		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		std::ifstream ifs("system/1", std::ifstream::in);
		os::parse_task_system_stream(ifs, task_system);
		ifs.close();
		
		std::cout << task_system << std::endl;

		os::llf_scheduler_event_based<os::task_system_t, os::job_t> scheduler;
		scheduler.reset();
		scheduler.init(task_system);
		scheduler.run(10u, os::task_system_period_lcm<uint, os::task_system_t>(task_system));



		std::cout << std::endl;
	}

	{
		std::cout << "STUDY TEST" << std::endl;
		os::task_system_t task_system;
		std::cout << task_system << std::endl;

		//uint seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::seed_seq seed = {1878};
		std::default_random_engine generator;
		std::uniform_real_distribution<double> usage_distribution(0.0,1.0);
		std::uniform_int_distribution<uint> period_distribution(50,100);

		typedef std::default_random_engine G;
		typedef std::uniform_real_distribution<double> U;
		typedef std::uniform_int_distribution<uint> P;
		os::task_system_generator<G,U,P> task_system_generator(generator, usage_distribution, period_distribution);

		os::benchmark_t benchmark[2];
		os::llf_scheduler_time_based<os::task_system_t, os::job_t> scheduler_1;
		os::llf_scheduler_event_based<os::task_system_t, os::job_t> scheduler_2;

		std::vector<uint> vector_n({2u});
		std::vector<double> vector_u({0.7});
		std::vector<uint> vector_d({10u});
		size_t n = 100;

		generator.seed(seed);
		os::study_scheduler(task_system_generator, scheduler_1, vector_n, vector_u, vector_d, n, benchmark[0], task_system, os::task_system_period_lcm<uint, os::task_system_t>);
		generator.seed(seed);
		os::study_scheduler(task_system_generator, scheduler_2, vector_n, vector_u, vector_d, n, benchmark[1], task_system, os::task_system_period_lcm<uint, os::task_system_t>);

		double avg, tot;
		avg = 0;
		tot = 0;
		for(os::benchmark_node_t& x : benchmark[0]){
			if(x.schedulable) ++avg;
			++tot;
		}

		std::cout << avg << " / " << tot << " : " << (avg/tot*100) << "%"<< std::endl;

		avg = 0;
		tot = 0;
		for(os::benchmark_node_t& x : benchmark[1]){
			if(x.schedulable) ++avg;
			++tot;
		}

		std::cout << avg << " / " << tot << " : " << (avg/tot*100) << "%"<< std::endl;

		std::cout << std::endl;
	}
	return 0;
}