#ifndef OS_BENCHMARK_NODE_T_H
#define OS_BENCHMARK_NODE_T_H

namespace os{
	typedef struct benchmark_node_t{
		uint u;
		uint n;
		uint d;
		bool schedulable;
		double preempted;

		
		benchmark_node_t(){}
		benchmark_node_t(uint u, uint n, uint d, bool schedulable, double preempted)
		:u(u), n(n), d(d), schedulable(schedulable), preempted(preempted){}

		template<typename O>
		friend O& operator<< (O &out, const benchmark_node_t& benchmark_node) {
			out << '['
				<< benchmark_node.u << ", "
				<< benchmark_node.n << ", "
				<< benchmark_node.d << ", "
				<< benchmark_node.schedulable << ", "
				<< benchmark_node.preempted
			<< ']';
			return out;
		}
	} benchmark_node_t;
}


#endif // OS_BENCHMARK_NODE_T_H