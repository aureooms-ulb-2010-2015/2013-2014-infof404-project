#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "os/benchmark_t.h"
#include "lib/pinput.h"
#include "lib/ansi.h"
#include "os/study/gnuplot.h"
#include "os/io.h"

template<typename O, typename S>
void fill_parameters(const O& options, S& file_name_in, bool& open_in, S file_name_out[3], bool& open_out){

	typename O::const_iterator it;

	if((it = options.find("-i")) != options.end() || (it = options.find("--input")) != options.end()){
		if(it->second.size() > 0){
			file_name_in = it->second[0];
			open_in = true;
		}
	}

	if((it = options.find("-o")) != options.end() || (it = options.find("--output")) != options.end()){
		if(it->second.size() > 2){
			file_name_out[0] = it->second[0];
			file_name_out[1] = it->second[1];
			file_name_out[2] = it->second[2];
			open_out = true;
		}
	}
}

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help        ]" << std::endl;
	std::cout << "   " << "[--nocolor          ]" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-i | --input       ] #0 (string)" << std::endl;
	std::cout << "   " << "[-o | --output      ] #0 #1 #2 (string[3])" << std::endl << std::endl;
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
		bool open_in = false, open_out = false;
		std::string file_name_in, file_name_out[3];

		fill_parameters(options, file_name_in, open_in, file_name_out, open_out);
		
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

		std::vector<std::vector<double>> mean[3] = {
			std::vector<std::vector<double>>(u_width, std::vector<double>(2,0)),
			std::vector<std::vector<double>>(d_width, std::vector<double>(2,0)),
			std::vector<std::vector<double>>(n_width, std::vector<double>(2,0))
		};

		os::study::gnuplot(benchmark, u_width, d_width, n_width, k, mean);

		// OUTPUT
		std::streambuf* buffer_out[3];
		std::ofstream ofstream[3];
		if(open_out){
			for(size_t i = 0; i < 3; ++i){
				ofstream[i].open(file_name_out[i]);
				buffer_out[i] = ofstream[i].rdbuf();
			}
		}
		else buffer_out[0] = buffer_out[1] = buffer_out[2] = std::cout.rdbuf();
		std::ostream ostream0(buffer_out[0]);
		std::ostream ostream1(buffer_out[1]);
		std::ostream ostream2(buffer_out[2]);

		os::store_gnuplot(ostream0, mean[0], vector_u, u_width);
		os::store_gnuplot(ostream1, mean[1], vector_d, d_width);
		os::store_gnuplot(ostream2, mean[2], vector_n, n_width);

		if(open_out) for(size_t i = 0; i < 3; ++i) ofstream[i].close();
	}
	catch(const std::exception& e){
		std::cout << ecolor << "error -> " << e.what() << rcolor << std::endl;
		return 1;
	}
	return 0;
}