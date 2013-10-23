#include <iostream>

#include "os/io.h"
#include "os/study/plot.h"
#include "lib/pinput.h"
#include "lib/exception.h"

int main(int argc, char const *argv[]){
	try{
		std::cout << "SVG TEST" << std::endl;
		
		size_t u_width, d_width;

		std::vector<std::vector<double>> mean;
		std::vector<double> vector_u;
		std::vector<uint> vector_d;

		os::load_mean(std::cin, mean, vector_u, vector_d, u_width, d_width);

		double boundaries[2] = {0, 1};

		const double res = 50, x_res = 50, y_res = 50, scale_res = 30;
		const svg::Color axis_color(0, 0, 0);
		const svg::Stroke axis_stroke(1, axis_color);
		const std::string file = "svg/3.svg";
		const lib::color color_good(255, 255, 255), color_bad(0, 0, 0);


		svg::Dimensions dimensions(150 + u_width * x_res, 150 + d_width * y_res);
		svg::Layout layout(dimensions, svg::Layout::BottomLeft);

		svg::Document doc(file, layout);
		os::study::plot_mean(doc, mean, boundaries[0], boundaries[1], u_width, d_width, res, color_good, color_bad, 75, 75);
		os::study::plot_scale(doc, 0, 1, 0.2, scale_res, color_good, color_bad, 0, 0);
		os::study::plot_axis(doc, "u", vector_u, u_width, x_res, "d", vector_d, d_width, y_res, axis_stroke, axis_color, 74, 74);
		doc.save();

		std::cout << std::endl;

	}
	catch(const std::exception& e){
		std::cout << "error -> " << e.what() << std::endl;
		return 1;
	}
	return 0;
}