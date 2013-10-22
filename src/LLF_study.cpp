#include <iostream>
#include <chrono>
#include <random>

#include "os/generator.h"
#include "os/study/plot.h"
#include "os/benchmark_t.h"
#include "os/task_system_t.h"
#include "os/llf_scheduler_time_based.h"
#include "os/llf_scheduler_event_based.h"
#include "os/job_t.h"
#include "os/study.h"
#include "os/lcm.h"
#include "os/io.h"

#include "lib/pinput.h"

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help]" << std::endl;
	std::cout << "   " << "[-v | --verbose]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "[-u | --utilization] #0 (double >= 0)" << std::endl;
	std::cout << "   " << "-n #0 (uint)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-p | --period] #0 [#1] (int[2], #1 >= #0)" << std::endl;
	std::cout << "   " << "[-s | --seed  ] #0 (uint)" << std::endl;
	std::cout << "   " << "[-o | --output] #0 (string)" << std::endl << std::endl;
}

int main(int argc, char* argv[]){

	std::vector<std::string> params;
	std::map<std::string, std::vector<std::string>> options;
	std::set<std::string> flags;
	std::set<std::string> option_set = {
		"-u", "--utilization",
		"-n",
		"-m",
		"-d", "--delta",
		"-p", "--period",
		"-s", "--seed",
		"-o", "--output"
	};
	std::set<std::string> flag_set = {
		"-h", "--help",
		"-v", "--verbose"
	};

	pinput::parse(argc, argv, params, options, flags, option_set, flag_set);

	if(flags.count("-h") || flags.count("--help")){
		help();
		return 0;
	}



	os::task_system_t task_system;

	std::seed_seq seed = {std::chrono::system_clock::now().time_since_epoch().count()};
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
	size_t m = 10;

	// generator.seed(seed);
	// os::study_scheduler(task_system_generator, scheduler_1, vector_n, vector_u, vector_d, m, benchmark[0], task_system, os::task_system_period_lcm<uint, os::task_system_t>);
	generator.seed(seed);
	os::study_scheduler(task_system_generator, scheduler_2, vector_n, vector_u, vector_d, m, benchmark[1], task_system, os::task_system_period_lcm<uint, os::task_system_t>);

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

	os::study::compute_mean(benchmark[1], u_width, d_width, p_mean, s_mean, counter);

	os::store_mean(std::cout, p_mean, u_width, d_width);
	os::store_mean(std::cout, s_mean, u_width, d_width);

	const double res = 50, x_res = 50, y_res = 50, scale_res = 30;
	const svg::Color axis_color(0, 0, 0);
	const svg::Stroke axis_stroke(1, axis_color);
	const std::string p_file = "svg/1.svg";
	const std::string s_file = "svg/2.svg";
	const lib::color color_good(255, 255, 255), color_bad(0, 0, 0);


	svg::Dimensions dimensions(150 + u_width * x_res, 150 + d_width * y_res);
	svg::Layout layout(dimensions, svg::Layout::BottomLeft);

	svg::Document doc1(p_file, layout);
	os::study::plot_mean(doc1, p_mean, boundaries[0][0], boundaries[0][1], u_width, d_width, res, color_good, color_bad, 75, 75);
	os::study::plot_scale(doc1, 0, 1, 0.2, scale_res, color_good, color_bad, 0, 0);
	os::study::plot_axis(doc1, "u", vector_u, u_width, x_res, "d", vector_d, d_width, y_res, axis_stroke, axis_color, 74, 74);
	doc1.save();

	svg::Document doc2(s_file, layout);
	os::study::plot_mean(doc2, s_mean, boundaries[1][0], boundaries[1][1], u_width, d_width, res, color_bad, color_good, 75, 75);
	os::study::plot_scale(doc2, 0, 1, 0.2, scale_res, color_bad, color_good, 0, 0);
	os::study::plot_axis(doc2, "u", vector_u, u_width, x_res, "d", vector_d, d_width, y_res, axis_stroke, axis_color, 74, 74);
	doc2.save();

	std::cout << std::endl;

	return 0;
}