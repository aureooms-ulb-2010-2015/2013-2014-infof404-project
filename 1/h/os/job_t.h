#ifndef OS_JOB_T_H
#define OS_JOB_T_H

namespace os{
	typedef struct job_t{
		size_t id;
		uint a;
		uint e;
		uint d;

		job_t(){}
		job_t(size_t id, uint a, uint e, uint d):id(id), a(a), e(e), d(d){}
		job_t(uint array[4]):id(array[0]), a(array[1]), e(array[2]), d(array[3]){}

		template<typename O>
		friend O& operator<< (O &out, const job_t& job) {
			out << '['
				<< job.a << ", "
				<< job.e << ", "
				<< job.d
			<< ']';
			return out;
		}
	} job_t;
}


#endif // OS_JOB_T_H