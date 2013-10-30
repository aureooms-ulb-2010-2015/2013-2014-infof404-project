#ifndef OS_STUDY_COMPUTE_MEAN_H
#define OS_STUDY_COMPUTE_MEAN_H

#include <fstream>

namespace os{

	namespace study{

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
	}

}


#endif // OS_STUDY_COMPUTE_MEAN_H