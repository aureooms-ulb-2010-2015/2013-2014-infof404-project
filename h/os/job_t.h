#ifndef OS_JOB_T_H
#define OS_JOB_T_H

namespace os{
	typedef struct job_t{
		uint a;
		uint e;
		uint d;

		job_t(){}
		job_t(uint a, uint e, uint d):a(a), e(e), d(d){}
		job_t(uint array[3]):a(array[0]), e(array[1]), d(array[2]){}

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