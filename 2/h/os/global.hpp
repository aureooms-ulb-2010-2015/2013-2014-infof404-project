#ifndef OS_GLOBAL_HPP
#define OS_GLOBAL_HPP


#include <vector>
#include <map>
#include <set>
#include <chrono>


typedef std::chrono::high_resolution_clock hrclock;

namespace os{
	namespace global{

		double start, stop, start_c, stop_c;
		hrclock::time_point checkpoint, tmp;
		hrclock::duration duration(0);

		bool help, speed, ssd,
		prime_filter, composite_filter,
		prime_color, composite_color, avoid_overflow;


		std::vector<std::string> params;
		std::map<std::string, std::vector<std::string>> options;
		std::set<std::string> flags;
		std::set<std::string> option_set = {
			"--prime-filter",
			"--composite-filter",
			"--prime-color",
			"--composite-color"
		};
		std::set<std::string> flag_set = {
			"-h", "--help",
			"--speed",
			"--ssd",
			"--avoid-overflow"
		};

		
		const int SETUP = 0;
		const int GATHER = 1;

	}
}



#endif // OS_GLOBAL_HPP