#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <limits>

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
#include "os/plot_result_svg.h"

int main(){

	// {
	// 	std::cout << "SVG TEST" << std::endl;
 //    	svg::Dimensions dimensions(100, 100);
 //    	svg::Document doc("svg/my_svg.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));

 //    	for(size_t i = 0; i < 10; ++i){
	// 		svg::Rectangle rectangle(svg::Point(i*10, i*10), i*10, i*20, svg::Color(i*10, i*20, i*30));
	// 		doc << rectangle;
	// 	}

	// 	doc.save();
	// }

	// return 0;

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

		const size_t u_width = 3, d_width = 3;

		std::vector<uint> vector_n({3u});
		std::vector<double> vector_u({0.3, 0.7, 0.9});
		std::vector<uint> vector_d({1u, 5u, 10u});
		size_t n = 10;

		// generator.seed(seed);
		// os::study_scheduler(task_system_generator, scheduler_1, vector_n, vector_u, vector_d, n, benchmark[0], task_system, os::task_system_period_lcm<uint, os::task_system_t>);
		generator.seed(seed);
		os::study_scheduler(task_system_generator, scheduler_2, vector_n, vector_u, vector_d, n, benchmark[1], task_system, os::task_system_period_lcm<uint, os::task_system_t>);

		double avg, tot;
		// avg = 0;
		// tot = 0;
		// for(os::benchmark_node_t& x : benchmark[0]){
		// 	if(x.schedulable) ++avg;
		// 	++tot;
		// }

		// std::cout << avg << " / " << tot << " : " << (avg/tot*100) << "%"<< std::endl;

		avg = 0;
		tot = 0;
		for(os::benchmark_node_t& x : benchmark[1]){
			if(x.schedulable) ++avg;
			++tot;
		}

		std::cout << avg << " / " << tot << " : " << (avg/tot*100) << "%"<< std::endl;

		std::cout << std::endl;

		// GZU7DUZGUGZUD

		std::cout << "SVG TEST" << std::endl;


		double p_mean[u_width][d_width] = {};
		double s_mean[u_width][d_width] = {};
		double counter[u_width][d_width] = {};
		// double boundaries[2][2] = {
		// 	{std::numeric_limits<double>::infinity(), 0},
		// 	{std::numeric_limits<double>::infinity(), 0}
		// };

		double boundaries[2][2] = {
			{0, 1},
			{0, 1}
		};

		os::compute_mean(benchmark[1], u_width, d_width, p_mean, s_mean, counter);

		const double res = 50, x_res = 50, y_res = 50, scale_res = 30;
		const svg::Color axis_color(0, 0, 0);
		const svg::Stroke axis_stroke(1, axis_color);
		const std::string p_file = "svg/1.svg";
		const std::string s_file = "svg/2.svg";
		const os::color color_good(255, 255, 255), color_bad(0, 0, 0);


		svg::Dimensions dimensions(150 + u_width * x_res, 150 + d_width * y_res);
		svg::Layout layout(dimensions, svg::Layout::BottomLeft);

		svg::Document doc1(p_file, layout);
		os::plot_mean(doc1, p_mean, boundaries[0][0], boundaries[0][1], u_width, d_width, res, color_good, color_bad, 75, 75);
		os::plot_scale(doc1, 0, 1, 0.2, scale_res, color_good, color_bad, 0, 0);
		os::plot_axis(doc1, "u", vector_u, u_width, x_res, "d", vector_d, d_width, y_res, axis_stroke, axis_color, 74, 74);
		doc1.save();

		svg::Document doc2(s_file, layout);
		os::plot_mean(doc2, s_mean, boundaries[1][0], boundaries[1][1], u_width, d_width, res, color_bad, color_good, 75, 75);
		os::plot_scale(doc2, 0, 1, 0.2, scale_res, color_bad, color_good, 0, 0);
		os::plot_axis(doc2, "u", vector_u, u_width, x_res, "d", vector_d, d_width, y_res, axis_stroke, axis_color, 74, 74);
		doc2.save();

		std::cout << std::endl;
	}
	return 0;
}