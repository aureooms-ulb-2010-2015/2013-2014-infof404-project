#ifndef OS_PLOT_RESULT_SVG_H
#define OS_PLOT_RESULT_SVG_H

#include "svg/namespace.hpp"

namespace os{

	typedef struct color{
		int r = 0;
		int g = 0;
		int b = 0;

		color(){}
		color(int r, int g, int b):r(r), g(g), b(b){}

	} color;

	int compute_color(const double value, const int start, const int stop){
		return value * (stop - start) + start;
	}

	color color_inv(int r, int g, int b){
		return color((r+128)%256, (g+128)%256, (b+128)%256);
	}

	template<typename B, typename M>
	void compute_mean(B& benchmark, const size_t u_width, const size_t d_width, M& p_mean, M& s_mean, M& counter){
		for(auto& x : benchmark){
			p_mean[x.u][x.d] += x.preempted;
			s_mean[x.u][x.d] += x.schedulable;
			++counter[x.u][x.d];
		}
		for(size_t i = 0; i < u_width; ++i){
			for(size_t j = 0; j < d_width; ++j){
				p_mean[i][j] /= counter[i][j];
				s_mean[i][j] /= counter[i][j];
			}
		}
	}

	template<typename B, typename M>
	void compute_mean(B& benchmark, const size_t u_width, const size_t d_width, M& p_mean, M& s_mean, M& counter, double boundaries[2][2]){
		for(auto& x : benchmark){
			p_mean[x.u][x.d] += x.preempted;
			s_mean[x.u][x.d] += x.schedulable;
			++counter[x.u][x.d];
		}
		for(size_t i = 0; i < u_width; ++i){
			for(size_t j = 0; j < d_width; ++j){
				p_mean[i][j] /= counter[i][j];
				s_mean[i][j] /= counter[i][j];
				if(p_mean[i][j] < boundaries[0][0]) boundaries[0][0] = p_mean[i][j];
				if(p_mean[i][j] > boundaries[0][1]) boundaries[0][1] = p_mean[i][j];
				if(s_mean[i][j] < boundaries[1][0]) boundaries[1][0] = s_mean[i][j];
				if(s_mean[i][j] > boundaries[1][1]) boundaries[1][1] = s_mean[i][j];
			}
		}
	}

	template<typename D, typename M>
	void plot_mean(D& doc, M& mean, const double v_min, const double v_max, const size_t u_width, const size_t d_width, const double res, const color start, const color stop, const double x_offset, const double y_offset){
		const double v_range = v_max - v_min;
		for(size_t i = 0; i < u_width; ++i){
			for(size_t j = 0; j < d_width; ++j){
				const double value = (mean[i][j] - v_min) / v_range;
				std::cout << v_min << ',' << value << ',' << v_max << std::endl;
				const int r = compute_color(value, start.r, stop.r);
				const int g = compute_color(value, start.g, stop.g);
				const int b = compute_color(value, start.b, stop.b);
				svg::Rectangle rectangle(svg::Point(i*res + x_offset, (j+1)*res + y_offset), res, res, svg::Color(r, g, b));
				std::stringstream value_s;
				value_s.precision(1);
				value_s << std::fixed << value;
				svg::Text text(svg::Point(i*res + res/2 - 10 + x_offset, j*res+ res/2 - 5 + y_offset), value_s.str(), svg::Color((r+128)%256, (g+128)%256, (b+128)%256));
				doc << rectangle << text;
			}
		}
	}

	template<typename D>
	void plot_scale(D& doc, const double v_min, const double v_max, const double v_step, const double res, const color start, const color stop, const double x_offset, const double y_offset){
		size_t j = 0;
		for(double i = v_min; i <= v_max; i += v_step){
			const int r = compute_color(i, start.r, stop.r);
			const int g = compute_color(i, start.g, stop.g);
			const int b = compute_color(i, start.b, stop.b);
			svg::Rectangle rectangle(svg::Point(j*res + x_offset, 1*res + y_offset), res, res, svg::Color(r, g, b));
			std::stringstream value_s;
			value_s.precision(1);
			value_s << std::fixed << i;
			svg::Text text(svg::Point(j*res + res/2 - 10 + x_offset, res/2 - 5 + y_offset), value_s.str(), svg::Color((r+128)%256, (g+128)%256, (b+128)%256));
			doc << rectangle << text;
			++j;
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
				stream.precision(1);
				stream << std::fixed << x_array[i];
				svg::Text text(svg::Point(i*x_res + x_res/2 - 10 + x_offset, - 5 + y_offset -y_res/4), stream.str(), color);
				doc << text;
			}
			svg::Text text(svg::Point(x_size*x_res + x_res/2 - 10 + x_offset,  - 5 + y_offset -y_res/4), x_label, color);
			doc << text;
		}

		{
			for(size_t j = 0; j < y_size; ++j){
				std::stringstream stream;
				stream.precision(1);
				stream << std::fixed << y_array[j];
				svg::Text text(svg::Point(- 10 + x_offset - x_res/4, j*y_res+ y_res/2 - 5 + y_offset), stream.str(), color);
				doc << text;
			}
			svg::Text text(svg::Point(- 10 + x_offset - x_res/4, y_size*y_res+ y_res/2 - 5 + y_offset), y_label, color);
			doc << text;
		}
		
	}

}


#endif // OS_PLOT_RESULT_SVG_H