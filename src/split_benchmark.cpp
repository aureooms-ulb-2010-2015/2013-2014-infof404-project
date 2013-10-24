#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "os/benchmark_t.h"
#include "lib/pinput.h"
#include "lib/ansi.h"
#include "os/study/compute_mean.h"
#include "os/io.h"
#include "lib/exception.h"


template<typename O, typename S, typename V>
void fill_parameters(const O& options, S& file_name_in, bool& open_in, V& file_name_out){

	typename O::const_iterator it;

	if((it = options.find("-i")) != options.end() || (it = options.find("--input")) != options.end()){
		if(it->second.size() > 0){
			file_name_in = it->second[0];
			open_in = true;
		}
	}

	if(((it = options.find("-o")) != options.end() || (it = options.find("--output")) != options.end()) && it->second.size() > 0){
		file_name_out = it->second;
	}
	else throw lib::exception("-o missing");
}

void help(){
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "[-o | --output      ] ... (string[])" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-i | --input       ] #0 (string)" << std::endl << std::endl;
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help        ]" << std::endl;
	std::cout << "   " << "[--nocolor          ]" << std::endl << std::endl;
}

int main(int argc, char *argv[]){
	std::vector<std::string> params;
	std::map<std::string, std::vector<std::string>> options;
	std::set<std::string> flags;
	std::set<std::string> option_set = {
		"-i", "--input",
		"-o", "--output"
	};
	std::set<std::string> flag_set = {
		"-h", "--help",
		"--nocolor"
	};

	pinput::parse(argc, argv, params, options, flags, option_set, flag_set);


	const bool nocolor = flags.count("--nocolor");

	const char* ecolor = (nocolor)? "" : ansi::red;
	const char* rcolor = (nocolor)? "" : ansi::reset;

	const bool show_help = flags.count("-h") || flags.count("--help");
	if(show_help){
		help();
		return 0;
	}

	try{

		bool open_in = false;
		std::string file_name_in;
		std::vector<std::string> file_name_out;

		fill_parameters(options, file_name_in, open_in, file_name_out);
		
		std::vector<double> vector_u;
		std::vector<uint> vector_d;
		std::vector<uint> vector_n;

		size_t u_width;
		size_t d_width;
		size_t n_width;
		size_t k;

		os::benchmark_t benchmark;

		// INPUT
		std::streambuf* buffer_in;
		std::ifstream ifstream;
		if(open_in){
			ifstream.open(file_name_in);
			buffer_in = ifstream.rdbuf();
		}
		else buffer_in = std::cin.rdbuf();
		std::istream istream(buffer_in);

		os::load_benchmark(istream, benchmark, vector_u, vector_d, vector_n, u_width, d_width, n_width, k);
		if(open_in) ifstream.close();

		if(n_width > file_name_out.size()) throw lib::exception("too few output files");

		std::vector<os::benchmark_t> new_benchmark(n_width);

		for(auto& x : benchmark){
			new_benchmark[x.n].push_back(x);
		}

		for(size_t i = 0; i < n_width; ++i){
			std::ofstream ofstream(file_name_out[i]);
			os::store_benchmark(ofstream, new_benchmark[i], vector_u, vector_d, std::vector<uint>(1,vector_n[i]), u_width, d_width, 1, k);
			ofstream.close();
		}

	}
	catch(const std::exception& e){
		std::cout << ecolor << "error -> " << e.what() << rcolor << std::endl;
		return 1;
	}

	return 0;
}