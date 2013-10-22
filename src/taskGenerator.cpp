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

template<typename O, typename U, typename N, typename P, typename S>
void parse_options(const O& options, U& u, N& n, P& p_min, P& p_max, S& file_name, bool& open){

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
	std::cout << "   " << "[-h | --help]" << std::endl;
	std::cout << "   " << "[-v | --verbose]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "[-u | --utilization] #0 (double >= 0)" << std::endl;
	std::cout << "   " << "-n #0 (int >= 0)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-p | --period] #0 [#1] (int[2], #1 >= #0)" << std::endl;
	std::cout << "   " << "[-o | --output] #0 (string)" << std::endl << std::endl;
}

int main(int argc, char* argv[]){

	try{

		std::vector<std::string> params;
		std::map<std::string, std::vector<std::string>> options;
		std::set<std::string> flags;
		std::set<std::string> option_set = {
			"-u", "--utilization",
			"-n",
			"-p", "--period",
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


		double u;
		uint n;
		uint p_min = 50;
		uint p_max = 100;
		std::string file_name;
		bool open = false;

		parse_options(options, u, n, p_min, p_max, file_name, open);

		check_parameters(u, n, p_min, p_max);

		os::task_system_t task_system;

		uint seed = std::chrono::system_clock::now().time_since_epoch().count();
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


	}
	catch(const std::exception& e){
		std::cout << "error -> " << e.what() << std::endl;
		return 1;
	}

	return 0;
}