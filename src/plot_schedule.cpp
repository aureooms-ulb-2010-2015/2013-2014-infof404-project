#include <iostream>
#include <map>
#include <set>

#include "os/plot_schedule_svg.h"
#include "lib/pinput.h"
#include "lib/exception.h"
#include "lib/ansi.h"

template<typename P, typename O, typename S>
void fill_parameters(const P& parameters, const O& options, S& file_name_out, uint& grad){

	if(parameters.size() < 1) throw lib::exception("Missing file name");

	file_name_out = parameters[0];

	typename O::const_iterator it;

	if(((it = options.find("-g")) != options.end()) && it->second.size() > 0){
		grad = std::stoul(it->second[0]);
	}

}

void check_parameters(const uint grad){
	if(grad < 1) throw lib::exception("grad < 1");
}

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h | --help    ]" << std::endl;
	std::cout << "   " << "[-v | --verbose ]" << std::endl;
	std::cout << "   " << "[--nocolor      ]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "#0 (string)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-g] #0 (int > 0)" << std::endl << std::endl;
}

int main(int argc, char *argv[]){

	std::vector<std::string> params;
	std::map<std::string, std::vector<std::string>> options;
	std::set<std::string> flags;
	std::set<std::string> option_set = {
		"-g"
	};
	std::set<std::string> flag_set = {
		"-h", "--help",
		"-v", "--verbose",
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

		std::string file_name_out;
		uint grad = 2;

		fill_parameters(params, options, file_name_out, grad);
		check_parameters(grad);
		
		int time_unit = 15;
		uint event_id, task_id, beg, end;
		int task_nbr, lcm;
		bool over = false;

		std::cin >> task_nbr;
		std::cin >> lcm;

		os::schedule::svg_scale scale = os::schedule::svg_scale(lcm,time_unit,task_nbr);
		svg::Document doc = os::schedule::make_SVG_doc(scale, file_name_out);

		draw_axis(doc, scale, grad); 
		draw_task_lines(doc, scale);
		draw_time_unit_lines(doc, scale);

		while(std::cin.good() and not over){

			std::cin >> event_id;
			std::cin >> task_id;
			std::cin >> beg;
			std::cin >> end;

			over = plot(doc, task_id, event_id, beg, end, scale);
		}

		doc.save();

	}
	catch(const std::exception& e){
		std::cout << ecolor << "error -> " << e.what() << rcolor << std::endl;
		return 1;
	}

	return 0;
}
