#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include "os/job_t.h"
#include "os/task_system_t.h"
#include "os/parse.h"
#include "os/llf_scheduler_event_based.h"
#include "os/lcm.h"
#include "lib/pinput.h"
#include "lib/io.h"
#include "lib/exception.h"

template<typename P, typename D, typename S>
void parse_parameters(const P& parameters, D& d, S& file_name, bool& open){

	if(parameters.size() < 1) throw lib::exception("Missing d");

	d = std::stoi(parameters[0]);
	if(parameters.size() >= 2){
		file_name = parameters[1];
		open = true;
	}

}

template<typename D>
void check_parameters(D& d){
	if(d < 1) throw lib::exception("d < 1");
}

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help]" << std::endl;
	std::cout << "   " << "[-v | --verbose]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "#0 (int >= 1)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "#1 (string)" << std::endl << std::endl;
}

int main(int argc, char* argv[]){

	try{

		std::vector<std::string> params;
		std::map<std::string, std::vector<std::string>> options;
		std::set<std::string> flags;
		std::set<std::string> option_set = {};
		std::set<std::string> flag_set = {
			"-h", "--help",
			"-v", "--verbose"
		};

		pinput::parse(argc, argv, params, options, flags, option_set, flag_set);

		if(flags.count("-h") || flags.count("--help")){
			help();
			return 0;
		}


		uint d;
		std::string file_name;
		bool open = false;

		parse_parameters(params, d, file_name, open);

		check_parameters(d);


		//READ INPUT

		std::streambuf* buffer;
		std::ifstream ifstream;

		if(open){
			ifstream.open(file_name);
			buffer = ifstream.rdbuf();
		}
		else{
			buffer = std::cin.rdbuf();
		}
		std::istream istream(buffer);

		os::task_system_t task_system;
		os::parse_task_system_stream(istream, task_system);
		if(open) ifstream.close();
		
		std::cout << task_system << std::endl;
		std::cout << std::endl;


		os::llf_scheduler_event_based<os::task_system_t, os::job_t> scheduler;
		scheduler.reset();
		scheduler.init(task_system);
		scheduler.run(d, os::task_system_period_lcm<uint, os::task_system_t>(task_system));

	}
	catch(const std::exception& e){
		std::cout << "error -> " << e.what() << std::endl;
	}

	return 0;
}