#ifndef OS_CONFIG_HPP
#define OS_CONFIG_HPP

#include "lib/pinput.hpp"
#include "lib/exception.hpp"
#include "lib/pixel_generator.hpp"
#include "os/global.hpp"

namespace os{
	namespace config{

		inline void fill(int argc, char *argv[]){
			pinput::parse(argc, argv, os::global::params, os::global::options, os::global::flags, os::global::option_set, os::global::flag_set);	
			os::global::help = os::global::flags.count("-h") || os::global::flags.count("--help");
			os::global::speed = os::global::flags.count("--speed");
			os::global::ssd = os::global::flags.count("--ssd");
			os::global::prime_filter = os::global::options.count("--prime-filter") && os::global::options["--prime-filter"].size() > 0;
			os::global::composite_filter = os::global::options.count("--composite-filter") && os::global::options["--composite-filter"].size() > 0;
			os::global::prime_color = os::global::options.count("--prime-color") && os::global::options["--prime-color"].size() > 2;
			os::global::composite_color = os::global::options.count("--composite-color") && os::global::options["--composite-color"].size() > 2;
			os::global::avoid_overflow = os::global::flags.count("--avoid-overflow");
		}

		inline void check(){
			if(os::global::params.size() < 1) throw lib::exception("<nth> missing");
			if(os::global::params.size() < 2 && !os::global::speed) throw lib::exception("<prefix> missing");
		}


		inline void help(){
			std::cout << "> OPTION" << std::endl;
			std::cout << std::endl;
			std::cout << "  --prime-filter" << "      " << "#0 (string)" << std::endl;
			std::cout << "  --composite-filter" << "  " << "#0 (string)" << std::endl;
			std::cout << "  --prime-color" << "       " << "#0 #1 #2 (int)" << std::endl;
			std::cout << "  --composite-color" << "   " << "#0 #1 #2 (int)" << std::endl;
			std::cout << std::endl;
			std::cout << "> FLAG" << std::endl;
			std::cout << std::endl;
			std::cout << "  -h | --help" << std::endl;
			std::cout << "  --speed" << std::endl;
			std::cout << "  --ssd" << std::endl;
			std::cout << "  --avoid-overflow" << std::endl;
			std::cout << std::endl;
		}


	}
}




#endif // OS_CONFIG_HPP