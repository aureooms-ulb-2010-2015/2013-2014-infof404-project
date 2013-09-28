#include <iostream>
#include <numeric>

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
}