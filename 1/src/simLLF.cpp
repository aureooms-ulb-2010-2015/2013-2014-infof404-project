#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include "os/job_t.h"
#include "os/task_system_t.h"
#include "os/llf_scheduler_event_based.h"
#include "os/llf_scheduler_time_based.h"
#include "os/lcm.h"
#include "lib/pinput.h"
#include "lib/io.h"
#include "lib/exception.h"
#include "lib/ansi.h"

template<typename P, typename O, typename D, typename S>
void parse_parameters(const P& parameters, const O& options, D& d, S& mode, S& file_name_in, bool& open_in, S& file_name_out, bool& open_out){

	if(parameters.size() < 1) throw lib::exception("Missing d");

	d = std::stoul(parameters[0]);
	if(parameters.size() >= 2){
		file_name_in = parameters[1];
		open_in = true;
	}

	typename O::const_iterator it;

	if(((it = options.find("-m")) != options.end() || (it = options.find("--mode")) != options.end()) && it->second.size() > 0){
		mode = it->second[0];
	}

	if((it = options.find("-o")) != options.end() || (it = options.find("--output")) != options.end()){
		if(it->second.size() > 0){
			file_name_out = it->second[0];
			open_out = true;
		}
	};

}

template<typename D, typename S>
void check_parameters(const D d, const S& mode){
	if(d < 1) throw lib::exception("d < 1");
	if(mode != "event" && mode != "time") throw lib::exception("mode not in [event|time]");
}

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help    ]" << std::endl;
	std::cout << "   " << "[-v | --verbose ]" << std::endl;
	std::cout << "   " << "[-p | --pipe    ]" << std::endl;
	std::cout << "   " << "[--nocolor      ]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "#0 (int >= 1)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "#1 (string)" << std::endl << std::endl;
	std::cout << "   " << "[-m | --mode    ] #0 (string)" << std::endl << std::endl;
	std::cout << "   " << "[-o | --output  ] #0 (string)" << std::endl << std::endl;
}

int main(int argc, char* argv[]){


	std::vector<std::string> params;
	std::map<std::string, std::vector<std::string>> options;
	std::set<std::string> flags;
	std::set<std::string> option_set = {
		"-m", "--mode"
	};
	std::set<std::string> flag_set = {
		"-h", "--help",
		"-v", "--verbose",
		"-p", "--pipe",
		"-o", "--output",
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

	const bool pipe = flags.count("-p") || flags.count("--pipe");

	try{

		uint d;
		std::string mode = "event";
		std::string file_name_in, file_name_out;
		bool open_in = false, open_out = false;

		parse_parameters(params, options, d, mode, file_name_in, open_in, file_name_out, open_out);

		check_parameters(d, mode);


		// INPUT

		std::streambuf* buffer_in;
		std::ifstream ifstream;

		if(open_in){
			ifstream.open(file_name_in);
			buffer_in = ifstream.rdbuf();
		}
		else{
			buffer_in = std::cin.rdbuf();
		}
		std::istream istream(buffer_in);

		os::task_system_t task_system;
		istream >> task_system;
		if(open_in) ifstream.close();

		// OUTPUT
		std::streambuf* buffer_out;
		std::ofstream ofstream;

		if(open_out){
			ofstream.open(file_name_out);
			buffer_out = ofstream.rdbuf();
		}
		else{
			buffer_out = std::cout.rdbuf();
		}

		std::ostream ostream(buffer_out);


		uint lcm = os::task_system_period_lcm<uint, os::task_system_t>(task_system);
		uint preempted, idle;
		bool schedulable;

		std::function<void(size_t, size_t, size_t, size_t)> callback = [](size_t, size_t, size_t, size_t){};
		if(pipe){
			ostream << task_system.size() << ' ' << lcm << std::endl;
			callback = [&](size_t event, size_t task, size_t i, size_t j){
				ostream << event << ' ' << task << ' ' << i << ' ' << j << std::endl;
			};
		}

		if(mode == "event"){
			os::llf_scheduler_event_based<os::task_system_t, os::job_t> scheduler;
			scheduler.reset();
			scheduler.init(task_system);
			scheduler.run(d, lcm, callback);

			preempted = scheduler.preempted;
			idle = scheduler.idle;
			schedulable = scheduler.schedulable;
		}
		else{
			os::llf_scheduler_time_based<os::task_system_t, os::job_t> scheduler;
			scheduler.reset();
			scheduler.init(task_system);
			scheduler.run(d, lcm, callback);

			preempted = scheduler.preempted;
			idle = scheduler.idle;
			schedulable = scheduler.schedulable;
		}

		if(!pipe){
			ostream << vcolor;
			ostream << "study interval [" << 0 << ", " << lcm << '[' << std::endl;
			ostream << "# preemptions : " << preempted << std::endl;
			ostream << "# idle : " << idle << std::endl;
			ostream << "b schedulable : " << std::boolalpha << schedulable << std::endl;
			ostream << rcolor;
		}

		if(open_out) ofstream.close();

	}
	catch(const std::exception& e){
		std::cout << ecolor << "error -> " << e.what() << rcolor << std::endl;
		return 1;
	}

	return 0;
}