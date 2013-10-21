#include <iostream>
#include <random>
#include <chrono>

#include "os/task_system_t.h"
#include "os/generator.h"
#include "lib/pinput.h"
#include "lib/io.h"

int main(int argc, char* argv[]){

	std::cout << "GENERATE SYSTEM TEST" << std::endl;

	std::vector<std::string> params;
	std::map<std::string, std::vector<std::string>> options;
	std::set<std::string> flags;
	std::set<std::string> option_set = {
		"-o", "--output",
		"-u", "--utilization",
		"-n",
		"-p", "--period"
	};
	std::set<std::string> flag_set = {};

	pinput::parse(argc, argv, params, options, flags, option_set, flag_set);

	std::cout << params << std::endl;
	std::cout << options << std::endl;
	std::cout << flags << std::endl;

	os::task_system_t task_system;

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

	return 0;
}