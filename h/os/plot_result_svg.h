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

	template<typename B, typename M>
	void compute_mean(B& benchmark, const size_t u_width, const size_t d_width, M& p_mean, M& s_mean, double boundaries[2][2]){
		for(auto& x : benchmark){
			p_mean[x.u][x.d] += x.preempted;
			s_mean[x.u][x.d] += x.schedulable;
		}
		for(size_t i = 0; i < u_width; ++i){
			for(size_t j = 0; j < d_width; ++j){
				p_mean[i][j] /= benchmark.size();
				s_mean[i][j] /= benchmark.size();
				if(p_mean[i][j] < boundaries[0][0]) boundaries[0][0] = p_mean[i][j];
				if(p_mean[i][j] > boundaries[0][1]) boundaries[0][1] = p_mean[i][j];
				if(s_mean[i][j] < boundaries[1][0]) boundaries[1][0] = s_mean[i][j];
				if(s_mean[i][j] > boundaries[1][1]) boundaries[1][1] = s_mean[i][j];
			}
		}
	}

	template<typename D, typename R, typename M>
	void plot_mean(D& doc, M& mean, const double v_min, const double v_max, const size_t u_width, const size_t d_width, const double res, const color start, const color stop){
		const double v_range = v_max - v_min;
		for(size_t i = 0; i < u_width; ++i){
			for(size_t j = 0; j < d_width; ++j){
				const double value = (mean[i][j] - v_min) / v_range;
				std::cout << v_min << ',' << value << ',' << v_max << std::endl;
				const int r = compute_color(value, start.r, stop.r);
				const int g = compute_color(value, start.g, stop.g);
				const int b = compute_color(value, start.b, stop.b);
				R rectangle(svg::Point(i*res, (j+1)*res), res, res, svg::Color(r, g, b));
				doc << rectangle;
			}
		}
	}

}


#endif // OS_PLOT_RESULT_SVG_H