#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>

#include "os/io.h"
#include "os/study/plot.h"
#include "lib/pinput.h"
#include "lib/exception.h"

template<typename O, typename S, typename C>
void fill_parameters(const O& options, S& file_name_in, bool& open_in, S& file_name_out, C& color_start, C& color_stop, C& color_axis, double& stroke_width, double& res, double& x_res, double& y_res, double& scale_res){

	typename O::const_iterator it;

	if((it = options.find("-o")) != options.end() || (it = options.find("--output")) != options.end()){
		if(it->second.size() > 0){
			file_name_out = it->second[0];
		}
	} else throw lib::exception("Cannot find -o option");

	if((it = options.find("-c")) != options.end() || (it = options.find("--color")) != options.end()){
		if(it->second.size() > 5){
			color_start.r = std::stoi(it->second[0]);
			color_start.g = std::stoi(it->second[1]);
			color_start.b = std::stoi(it->second[2]);
			color_stop.r = std::stoi(it->second[3]);
			color_stop.g = std::stoi(it->second[4]);
			color_stop.b = std::stoi(it->second[5]);
		}
	}

	if((it = options.find("-i")) != options.end() || (it = options.find("--input")) != options.end()){
		if(it->second.size() > 0){
			file_name_in = it->second[0];
			open_in = true;
		}
	}


	if((it = options.find("-r")) != options.end() || (it = options.find("--res")) != options.end()){
		if(it->second.size() > 0){
			res = x_res = y_res = std::stod(it->second[0]);
			if(it->second.size() > 1){
				y_res = std::stod(it->second[1]);
			}
		}
	}

	if((it = options.find("--sr")) != options.end() || (it = options.find("--scaleres")) != options.end()){
		if(it->second.size() > 0){
			scale_res = std::stod(it->second[0]);
		}
	}

	if((it = options.find("-a")) != options.end() || (it = options.find("--axiscolor")) != options.end()){
		if(it->second.size() > 0){
			color_axis.r = color_axis.g = color_axis.b = std::stoi(it->second[0]);
			if(it->second.size() > 2){
				color_axis.g = std::stoi(it->second[1]);
				color_axis.b = std::stoi(it->second[2]);
			}
		}
	}

	if((it = options.find("-s")) != options.end() || (it = options.find("--strokewidth")) != options.end()){
		if(it->second.size() > 0){
			stroke_width = std::stod(it->second[0]);
		}
	}

}

void help(){
	std::cout << " - flags" << std::endl << std::endl;
	std::cout << "   " << "[-h  | --help        ]" << std::endl;
	std::cout << "   " << "[-v  | --verbose     ]" << std::endl << std::endl;
	std::cout << " - mandatory parameters" << std::endl << std::endl;
	std::cout << "   " << "[-o  | --output      ] #0 (string)" << std::endl << std::endl;
	std::cout << " - optional parameters" << std::endl << std::endl;
	std::cout << "   " << "[-c  | --color       ] #0 #1 #2 #3 #4 #5 (int[6])" << std::endl;
	std::cout << "   " << "[-i  | --input       ] #0 (string)" << std::endl;
	std::cout << "   " << "[-r  | --res         ] #0 (double)" << std::endl;
	std::cout << "   " << "[-sr | --scaleres    ] #0 (double)" << std::endl;
	std::cout << "   " << "[-a  | --axiscolor   ] #0 #1 #2 (int[3])" << std::endl;
	std::cout << "   " << "[-s  | --strokewidth ] #0 (double)" << std::endl;
}

int main(int argc, char *argv[]){
	try{
		std::vector<std::string> params;
		std::map<std::string, std::vector<std::string>> options;
		std::set<std::string> flags;
		std::set<std::string> option_set = {
			"-o", "--output",
			"-c", "--color",
			"-i", "--input",
			"-r", "--res",
			"-sr", "--scaleres",
			"-a", "--axiscolor",
			"-s", "--strokewidth",
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

		std::string file_name_in;
		bool open_in = false;
		std::string file_name_out;


		double res = 50, x_res = 50, y_res = 50, scale_res = 30, stroke_width = 1;
		lib::color color_axis(0, 0, 0), color_start(255, 255, 255), color_stop(0, 0, 0);

		fill_parameters(options, file_name_in, open_in, file_name_out, color_start, color_stop, color_axis, stroke_width, res, x_res, y_res, scale_res);

		// CHOOSE INPUT

		std::streambuf* buffer;
		std::ifstream ifstream;

		if(open_in){
			ifstream.open(file_name_in);
			buffer = ifstream.rdbuf();
		}
		else{
			buffer = std::cin.rdbuf();
		}
		std::istream istream(buffer);


		// LOAD DATA
		size_t u_width, d_width;

		std::vector<std::vector<double>> mean;
		std::vector<double> vector_u;
		std::vector<uint> vector_d;
		
		os::load_mean(istream, mean, vector_u, vector_d, u_width, d_width);
		if(open_in) ifstream.close();


		//PLOT DATA
		double boundaries[2] = {0, 1};

		const svg::Color axis_color(color_axis.r, color_axis.g, color_axis.b);
		const svg::Stroke axis_stroke(stroke_width, axis_color);

		svg::Dimensions dimensions(150 + u_width * x_res, 150 + d_width * y_res);
		svg::Layout layout(dimensions, svg::Layout::BottomLeft);

		svg::Document doc(file_name_out, layout);
		os::study::plot_mean(doc, mean, boundaries[0], boundaries[1], u_width, d_width, res, color_start, color_stop, 75, 75);
		os::study::plot_scale(doc, 0, 1, 0.2, scale_res, color_start, color_stop, 0, 0);
		os::study::plot_axis(doc, "u", vector_u, u_width, x_res, "d", vector_d, d_width, y_res, axis_stroke, axis_color, 75, 75);
		doc.save();

		std::cout << std::endl;

	}
	catch(const std::exception& e){
		std::cout << "error -> " << e.what() << std::endl;
		return 1;
	}
	return 0;
}