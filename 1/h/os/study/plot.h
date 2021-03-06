#ifndef OS_STUDY_PLOT_H
#define OS_STUDY_PLOT_H

#include "svg/namespace.hpp"
#include "lib/color.h"

namespace os{

	namespace study{

		int compute_color(const double value, const int start, const int stop){
			return value * (stop - start) + start;
		}

		svg::Color color_inv(int r, int g, int b){
			return svg::Color((r+128)%256 , (g+128)%256, (b+128)%256);
		}

		// svg::Color color_inv(int r, int g, int b){
		// 	r = (r > 128)? r - 40 : r + 40;
		// 	g = (g > 128)? g - 40 : g + 40;
		// 	b = (b > 128)? b - 40 : b + 40;
		// 	return svg::Color(r, g, b);
		// }

		template<typename D, typename M>
		void plot_mean(D& doc, M& mean, const double v_min, const double v_max, const size_t u_width, const size_t d_width, const double res, const lib::color start, const lib::color stop, const double x_offset, const double y_offset){
			const double v_range = v_max - v_min;
			for(size_t i = 0; i < u_width; ++i){
				for(size_t j = 0; j < d_width; ++j){
					const double value = (mean[i][j] - v_min) / v_range;
					const int r = compute_color(value, start.r, stop.r);
					const int g = compute_color(value, start.g, stop.g);
					const int b = compute_color(value, start.b, stop.b);
					svg::Rectangle rectangle(svg::Point(i*res + x_offset, (j+1)*res + y_offset), res, res, svg::Color(r, g, b));
					std::stringstream value_s;
					value_s.precision(2);
					value_s << std::fixed << value;
					svg::Text text(svg::Point(i*res + res/2 - 14 + x_offset, j*res+ res/2 - 5 + y_offset), value_s.str(), color_inv(r,g,b));
					doc << rectangle << text;
				}
			}
		}

		template<typename D>
		void plot_scale(D& doc, const double v_min, const double v_max, const size_t v_size, const double res, const lib::color start, const lib::color stop, const double x_offset, const double y_offset){
			
			for(size_t j = 0; j <= v_size; ++j){
				const double i = (v_max - v_min) / v_size * j;
				const int r = compute_color(i, start.r, stop.r);
				const int g = compute_color(i, start.g, stop.g);
				const int b = compute_color(i, start.b, stop.b);
				svg::Rectangle rectangle(svg::Point(j*res + x_offset, 1*res + y_offset), res, res, svg::Color(r, g, b));
				std::stringstream value_s;
				value_s.precision(2);
				value_s << std::fixed << i;
				svg::Text text(svg::Point(j*res + res/2 - 14 + x_offset, res/2 - 5 + y_offset), value_s.str(), color_inv(r,g,b));
				doc << rectangle << text;
			}
		}

		template<typename D, typename A, typename B>
		void plot_axis(D& doc, const std::string& x_label, const A& x_array, const size_t x_size, const double x_res, const std::string& y_label, const B& y_array, const size_t y_size, const double y_res, svg::Stroke stroke, svg::Color color, const double x_offset, const double y_offset){
			svg::Line x_axis(svg::Point(x_offset, y_offset), svg::Point(x_res * x_size + x_offset, y_offset),stroke);
			svg::Line y_axis(svg::Point(x_offset, y_offset), svg::Point(x_offset, y_res * y_size + y_offset),stroke);
			svg::Polygon x_arrow(color);
			x_arrow
			 << svg::Point(x_res * x_size + x_offset + 20, y_offset)
			 << svg::Point(x_res * x_size + x_offset, y_offset + 5)
			 << svg::Point(x_res * x_size + x_offset, y_offset - 5);
			svg::Polygon y_arrow(color);
			y_arrow
			 << svg::Point(x_offset, y_res * y_size + y_offset + 20)
			 << svg::Point(x_offset + 5, y_res * y_size + y_offset)
			 << svg::Point(x_offset - 5, y_res * y_size + y_offset);
			doc << x_axis << y_axis << x_arrow << y_arrow;

			{
				for(size_t i = 0; i < x_size; ++i){
					std::stringstream stream;
					stream.precision(2);
					stream << std::fixed << x_array[i];
					svg::Text text(svg::Point(i*x_res + x_res/2 - 14 + x_offset, - 5 + y_offset -y_res/4), stream.str(), color);
					doc << text;
				}
				svg::Text text(svg::Point(x_size*x_res + x_res/2 - 14 + x_offset,  - 5 + y_offset -y_res/4), x_label, color);
				doc << text;
			}

			{
				for(size_t j = 0; j < y_size; ++j){
					std::stringstream stream;
					stream.precision(2);
					stream << std::fixed << y_array[j];
					svg::Text text(svg::Point(- 14 + x_offset - x_res/4, j*y_res+ y_res/2 - 5 + y_offset), stream.str(), color);
					doc << text;
				}
				svg::Text text(svg::Point(- 14 + x_offset - x_res/4, y_size*y_res+ y_res/2 - 5 + y_offset), y_label, color);
				doc << text;
			}
			
		}
	}

}


#endif // OS_STUDY_PLOT_H