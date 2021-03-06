#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <string>
#include <utility>

#include "os/task_system_t.h"
#include "os/generator.h"
#include "lib/pinput.h"
#include "lib/io.h"
#include "lib/exception.h"
#include "lib/ansi.h"

template<typename O, typename U, typename N, typename P, typename V, typename S>
void fill_parameters(const O& options, U& u, N& n, P& p_min, P& p_max, V& s, S& file_name, bool& open){

	typename O::const_iterator it;

	if(((it = options.find("-u")) != options.end() || (it = options.find("--utilization")) != options.end()) && it->second.size() > 0){
		u = std::stod(it->second[0]);
	}
	else throw lib::exception("Cannot find -u option");

	if((it = options.find("-n")) != options.end() && it->second.size() > 0){
		n = std::stoul(it->second[0]);
	}
	else throw lib::exception("Cannot find -n option");


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
		for(auto& x : it->second) s.push_back(std::stoll(x));
	}

	if((it = options.find("-o")) != options.end() || (it = options.find("--output")) != options.end()){
		if(it->second.size() > 0){
			file_name = it->second[0];
			open = true;
		}
	}

}

template<typename U, typename N, typename P>
void check_parameters(U& u, N& n, P& p_min, P& /*p_max*/){
	if(n > p_min) throw lib::exception("n > p_min");
	if(u * p_min < n) throw lib::exception("u < n/p_min");
}

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help        ]" << std::endl;
	std::cout << "   " << "[-v | --verbose     ]" << std::endl;
	std::cout << "   " << "[--nocolor          ]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "[-u | --utilization ] #0 (double >= 0)" << std::endl;
	std::cout << "   " << "-n #0 (int >= 0)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-p | --period      ] #0 [#1] (int[2], #1 >= #0)" << std::endl;
	std::cout << "   " << "[-s | --seed        ] #0 (uint)" << std::endl;
	std::cout << "   " << "[-o | --output      ] #0 (string)" << std::endl << std::endl;
}

int main(int argc, char* argv[]){


	std::vector<std::string> params;
	std::map<std::string, std::vector<std::string>> options;
	std::set<std::string> flags;
	std::set<std::string> option_set = {
		"-u", "--utilization",
		"-n",
		"-p", "--period",
		"-s", "--seed",
		"-o", "--output"
	};
	std::set<std::string> flag_set = {
		"-h", "--help",
		"-v", "--verbose",
		"--nocolor"
	};

	pinput::parse(argc, argv, params, options, flags, option_set, flag_set);

	const bool nocolor = flags.count("--nocolor");

	const char* vcolor = (nocolor)? "" : ansi::blue;
	const char* ecolor = (nocolor)? "" : ansi::red;
	const char* rcolor = (nocolor)? "" : ansi::reset;

	const bool show_help = flags.count("-h") || flags.count("--help");
	if(show_help){
		help();
		return 0;
	}

	const bool verbose = flags.count("-v") || flags.count("--verbose");
		
	try{

		double u;
		uint n;
		uint p_min = 50;
		uint p_max = 100;
		std::vector<long long> seed_v;

		std::string file_name;
		bool open = false;

		fill_parameters(options, u, n, p_min, p_max, seed_v, file_name, open);
		check_parameters(u, n, p_min, p_max);

		if(seed_v.size() == 0) seed_v.push_back(std::chrono::system_clock::now().time_since_epoch().count());

		os::task_system_t task_system;

		std::seed_seq seed(seed_v.begin(), seed_v.end());
		std::default_random_engine generator(seed);
		std::uniform_real_distribution<double> usage_distribution(0.0,1.0);
		std::uniform_int_distribution<uint> period_distribution(p_min,p_max);
		os::generate_task_system(generator, usage_distribution, period_distribution, u, n, task_system);

		//WRITE OUTPUT

		std::streambuf* buffer;
		std::ofstream ofstream;

		if(open){
			ofstream.open(file_name);
			buffer = ofstream.rdbuf();
		}
		else{
			buffer = std::cout.rdbuf();
		}

		std::ostream ostream(buffer);

		ostream << task_system;

		if(open) ofstream.close();

		if(verbose){
			double t = 0;
			for(auto task : task_system){
				t += (double)task.wcet/(double)task.period;
			}

			std::cout << vcolor << std::endl;
			std::cout << "u = " << t << std::endl;
			std::cout.precision(2);
			std::cout << std::fixed;
			std::cout << "abs error = " << (u - t)*100 << "%" << std::endl;
			std::cout << "rel error = " << (1 - t/u)*100 << "%" << std::endl;
			std::cout << "worst abs error that could happen = " << (100./p_min*n) << "%" << std::endl;
			std::cout << rcolor;
		}


	}
	catch(const std::exception& e){
		std::cout << ecolor << "error -> " << e.what() << rcolor << std::endl;
		return 1;
	}

	return 0;
}