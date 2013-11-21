#ifndef LIB_PINPUT_HPP
#define LIB_PINPUT_HPP

#include <string>
#include <utility>
#include <tuple>

namespace pinput{
	template<typename P, typename O, typename F, typename T, typename U>
	void parse(int argc, char* argv[], P& params, O& options, F& flags, const T& option_set, const U& flag_set){

		std::string optionalParameterKey;
		bool isOptionalParameter = false;

		for(int n = 1; n < argc; ++n){
			std::string p = argv[n];
			if (p[0] == '-'){
				isOptionalParameter = false;
				if(p.size() > 1){
					if(p[1] == '-' || p.size() == 2){

						if(option_set.count(p) > 0){
							options.insert(typename O::value_type(p, {}));
							optionalParameterKey = p;
							isOptionalParameter = true;
						}
						else if(flag_set.count(p) > 0){
							flags.insert(p);
						}
					}
					else{
						for(size_t i = 1; i < p.size(); ++i){
							flags.insert(std::string({'-', p[i]}));
						}
					}
				}
			}
			else if(isOptionalParameter){
				options.at(optionalParameterKey).push_back(p);
			}
			else{
				params.push_back(p);
			}
		}
	}
}

#endif // LIB_PINPUT_HPP
