#ifndef LIB_NUM_H
#define LIB_NUM_H

#include <iostream>
#include <numeric>
#include <cmath>

namespace lib{

	template<typename I>
	I gcd(I a, I b){
		while(true){
			if (a == 0) return b;
			b %= a;
			if (b == 0) return a;
			a %= b;
		}
	}

	template<typename I>
	I lcm(I a, I b){
		I temp = gcd(a, b);
		return temp ? (a / temp * b) : 0;
	}

	template<typename F, typename I>
	F ceil(F v, const I p){
		v *= pow(10, p);
		v = ceil(v);
		v /= pow(10, -p);
		return v;
	}

	template<typename F, typename I>
	F floor(F v, const I p){
		v *= pow(10, p);
		v = floor(v);
		v /= pow(10, -p);
		return v;
	}
}

#endif //LIB_NUM_H