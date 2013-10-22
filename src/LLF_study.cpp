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
#include "lib/exception.h"

template<typename O, typename U, typename D, typename N, typename K, typename P, typename V, typename S>
void fill_parameters(const O& options, U& u, D& d, N& n, K& k, P& p_min, P& p_max, V& s, S& mode, S& file_name, bool& open){

	typename O::const_iterator it;

	if(((it = options.find("-u")) != options.end() || (it = options.find("--utilization")) != options.end()) && it->second.size() > 0){
		for(auto& x : it->second) u.push_back(std::stod(x));
	}
	else throw lib::exception("Cannot find -u option");

	if(((it = options.find("-d")) != options.end() || (it = options.find("--delta")) != options.end()) && it->second.size() > 0){
		for(auto& x : it->second) d.push_back(std::stoul(x));
	}
	else throw lib::exception("Cannot find -d option");

	if((it = options.find("-n")) != options.end() && it->second.size() > 0){
		for(auto& x : it->second) n.push_back(std::stoul(x));
	}
	else throw lib::exception("Cannot find -n option");

	if((it = options.find("-k")) != options.end() && it->second.size() > 0){
		k = std::stoul(it->second[0]);
	}
	else throw lib::exception("Cannot find -k option");


	if((it = options.find("-p")) != options.end() || (it = options.find("--period")) != options.end()){
		if(it->second.size() > 0){
			p_min = p_max = std::stoul(it->second[0]);
			if(it->second.size() > 1){
				p_max = std::stoul(it->second[1]);
			}
			if(p_min > p_max) std::swap(p_min, p_max);
		}
	}

	if(((it = options.find("-s")) != options.end() || (it = options.find("--seed")) != options.end()) && it->second.size() > 0){
		for(auto& x : it->second) s.push_back(std::stoi(x));
	}

	if((it = options.find("-o")) != options.end() || (it = options.find("--output")) != options.end()){
		if(it->second.size() > 0){
			file_name = it->second[0];
			open = true;
		}
	}

	if(((it = options.find("-m")) != options.end() || (it = options.find("--mode")) != options.end()) && it->second.size() > 0){
		mode = it->second[0];
	}

}

template<typename U, typename D, typename N, typename P, typename S>
void check_parameters(const U& u, const D& d, const N& n, const P& p_min, const S& mode){
	for(auto& x : d){
		if(x < 1) throw lib::exception("E d[i] < 1");
	}
	for(auto& x : n){
		if(x > p_min) throw lib::exception("E n[i] > p_min");
		for(auto& y : u){
			if(y * p_min < x) throw lib::exception("E u[j] < n/p_min[i]");
		}
	}
	if(mode != "event" && mode != "time") throw lib::exception("mode not in [event|time]");
}

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help]" << std::endl;
	std::cout << "   " << "[-v | --verbose]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "[-u | --utilization] ... (double[] | v[i] >= 0)" << std::endl;
	std::cout << "   " << "[-d | --delta      ] ... (uint[])" << std::endl;
	std::cout << "   " << "-n                   ... (uint[])" << std::endl;
	std::cout << "   " << "-k                   ... (uint)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-p | --period] #0 [#1] (int[2], #1 >= #0)" << std::endl;
	std::cout << "   " << "[-s | --seed  ] #0 (uint)" << std::endl;
	std::cout << "   " << "[-o | --output] #0 (string)" << std::endl << std::endl;
	std::cout << "   " << "[-m | --mode  ] #0 (string)" << std::endl << std::endl;
}

int main(int argc, char* argv[]){
	try{

		std::vector<std::string> params;
		std::map<std::string, std::vector<std::string>> options;
		std::set<std::string> flags;
		std::set<std::string> option_set = {
			"-u", "--utilization",
			"-d", "--delta",
			"-n",
			"-k",
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



		std::vector<double> vector_u;
		std::vector<uint> vector_d;
		std::vector<uint> vector_n;
		size_t k;

		uint p_min = 50;
		uint p_max = 100;
		std::vector<long long> seed_v;

		std::string mode = "event";
		std::string file_name;
		bool open = false;

		fill_parameters(options, vector_u, vector_d, vector_n, k, p_min, p_max, seed_v, mode, file_name, open);
		check_parameters(vector_u, vector_d, vector_n, p_min, mode);

		if(seed_v.size() == 0) seed_v.push_back(std::chrono::system_clock::now().time_since_epoch().count());

		const size_t u_width = vector_u.size();
		const size_t d_width = vector_d.size();


		os::task_system_t task_system;

		std::seed_seq seed(seed_v.begin(), seed_v.end());
		std::default_random_engine generator;
		std::uniform_real_distribution<double> usage_distribution(0.0,1.0);
		std::uniform_int_distribution<uint> period_distribution(p_min,p_max);

		typedef std::default_random_engine G;
		typedef std::uniform_real_distribution<double> U;
		typedef std::uniform_int_distribution<uint> P;
		os::task_system_generator<G,U,P> task_system_generator(generator, usage_distribution, period_distribution);

		os::benchmark_t benchmark;

		generator.seed(seed);

		if(mode == "event"){
			os::llf_scheduler_event_based<os::task_system_t, os::job_t> scheduler;
			os::study_scheduler(task_system_generator, scheduler, vector_n, vector_u, vector_d, k, benchmark, task_system, os::task_system_period_lcm<uint, os::task_system_t>);
		}
		else{
			os::llf_scheduler_time_based<os::task_system_t, os::job_t> scheduler;
			os::study_scheduler(task_system_generator, scheduler, vector_n, vector_u, vector_d, k, benchmark, task_system, os::task_system_period_lcm<uint, os::task_system_t>);
		}

		double avg = 0, tot = 0;
		for(os::benchmark_node_t& x : benchmark){
			if(x.schedulable) ++avg;
			++tot;
		}

		std::cout << avg << " / " << tot << " : " << (avg/tot*100) << "%"<< std::endl;

		std::cout << std::endl;


		std::cout << "SVG TEST" << std::endl;

		std::vector<std::vector<double>> p_mean(u_width, std::vector<double>(d_width, 0));
		std::vector<std::vector<double>> s_mean(u_width, std::vector<double>(d_width, 0));
		std::vector<std::vector<double>> counter(u_width, std::vector<double>(d_width, 0));

		double boundaries[2][2] = {
			{0, 1},
			{0, 1}
		};

		os::study::compute_mean(benchmark, u_width, d_width, p_mean, s_mean, counter);

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

	}
	catch(const std::exception& e){
		std::cout << "error -> " << e.what() << std::endl;
		return 1;
	}

	return 0;
}