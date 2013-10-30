#ifndef OS_STUDY_GNUPLOT_H
#define OS_STUDY_GNUPLOT_H

#include <stddef.h>

namespace os{
	namespace study{


		template<typename B, typename M>
		void gnuplot(B& benchmark, const size_t u_width, const size_t d_width, const size_t n_width, const size_t k, M& mean){
			for(auto& x : benchmark){
				mean[0][x.u][0] += x.preempted;
				mean[0][x.u][1] += x.schedulable;
				mean[1][x.d][0] += x.preempted;
				mean[1][x.d][1] += x.schedulable;
				mean[2][x.n][0] += x.preempted;
				mean[2][x.n][1] += x.schedulable;
			}
			for(size_t i = 0; i < u_width; ++i){
				mean[0][i][0] /= k * d_width * n_width;
				mean[0][i][1] /= k * d_width * n_width;
			}
			for(size_t i = 0; i < d_width; ++i){
				mean[1][i][0] /= k * u_width * n_width;
				mean[1][i][1] /= k * u_width * n_width;
			}
			for(size_t i = 0; i < n_width; ++i){
				mean[2][i][0] /= k * d_width * u_width;
				mean[2][i][1] /= k * d_width * u_width;
			}
		}


	}
}

#endif // OS_STUDY_GNUPLOT_H