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

svg::Document make_SVG_doc(int lcm, int task_nbr, int size_of_time_unit ){
		//lcm total number of time unit needed

		svg::Dimensions dimensions(25 + lcm*size_of_time_unit , 100 + 50*task_nbr);//width,height
		svg::Document doc("svg/plot_res.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));

		return doc;

	}

void draw_axis (svg::Document& doc, int width, int height, int time_unit, int lcm ){
	svg::Line vert_axis(svg::Point(12, 12), svg::Point(12, height-12), svg::Stroke(2,svg::Color(20, 30, 40)));
	svg::Line hor_axis(svg::Point(12, 12), svg::Point(width-12, 12), svg::Stroke(2,svg::Color(20, 30, 40)));

	svg::Polygon hor_arrow(svg::Color(20, 30, 40));
	hor_arrow << svg::Point(width-12, 12) << svg::Point(width-18, 6) << svg::Point(width-18, 18);



	doc << vert_axis << hor_axis << hor_arrow ;


	//draw graduation

	int max_grad = lcm * time_unit;

	std::cout<<max_grad<<std::endl;
	for (int i = 0; i <= lcm ; i+=5)
	{
		std::cout<<i<<" " <<lcm<<std::endl;
		std::stringstream value_s;
		value_s.precision(0);
		value_s  << i;
		int offset = 9;
		if (i>9){offset=0;}
		else if (i>99){offset=-9;}
		svg::Text text(svg::Point( i*time_unit+offset , 2), value_s.str(), svg::Color(20, 30, 40));
		doc <<  text;
	}

	}


void draw_vert_arrow (svg::Document& doc, svg::Point head){

	svg::Point tail (head.get_x(), head.get_y()+25);

	svg::Line arrow_body(head, tail, svg::Stroke(2,svg::Color(20, 30, 40)));
	svg::Polygon arrow_head(svg::Color(20, 30, 40));
	arrow_head << head << svg::Point(head.get_x()+3, head.get_y()+7) << svg::Point(head.get_x()-3, head.get_y()+7);
	doc << arrow_body << arrow_head;

	}

void draw_rectangle(svg::Document& doc, svg::Point top_left_corner, int time_unit){
	
	svg::Rectangle rect = svg::Rectangle(top_left_corner,time_unit,25);
    doc<<rect;
}
void draw_circle(svg::Document& doc, svg::Point center){
	int circle_diameter = 12;
	svg::Circle circle(center, circle_diameter, svg::Fill(svg::Color(20, 30, 40)));
	doc << circle;
}


int main(){

	 



	 {
	 	std::cout << "SVG TEST" << std::endl;
	 	//PARAM
	 	int lcm = 120;
	 	int task_nbr = 3;
	 	int time_unit = 10;
	 	//ATTRIBUTS
    	int width =25 + lcm*time_unit;
    	int height =100 + 50*task_nbr;//25 for arrows 25 for blocks
    	

    	svg::Document doc = make_SVG_doc(lcm,task_nbr,time_unit);
    	draw_axis(doc,width,height,time_unit,lcm ); 
    	




   		

	 	doc.save();
	 }

	/*
{
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
}
	*/

	
	return 0;
}


