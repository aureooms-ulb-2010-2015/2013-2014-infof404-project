
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <time.h>


int main(int argc, char const *argv[]){
	
	if (argc<2){
		std::cout<<"N?"<<std::endl;
		return 1;
	}
	clock_t begin, end;
	double time_spent;

	begin = clock();

	size_t n_max = std::stoull(argv[1]);


	std::vector<uint64_t> prime;
	std::vector<bool> sieve(n_max + 1, false);


	prime.push_back(2);
	prime.push_back(3);
	prime.push_back(5);

	sieve[2] = sieve[3] = sieve[5] = true;

	size_t N = sqrt(n_max);
	for(size_t x = 1; x <= N ; ++x){
		for(size_t y = 1; y <= N; ++y){

			size_t n = (4*x*x)+(y*y);
			if ((n <= n_max) && ((n % 12 == 1) || (n % 12 == 5))) sieve[n] = sieve[n] xor true; //Xor with true toggle bool

			n = (3*x*x)+(y*y);
			if ((n <= n_max) && (n % 12 == 7)) sieve[n] = sieve[n] xor true;

			n = (3*x*x)-(y*y);
			if ((x > y) && (n <= n_max) && (n % 12 == 11)) sieve[n] = sieve[n] xor true;
		}
	}
	
	size_t a;
	for(a = 7; a <= N; a += 2){
		if(sieve[a]){
			for (size_t i = a*a; i <= n_max; i += a*a) sieve[i] = false;
			prime.push_back(a);
		}
	}

	for(; a <= n_max; a += 2) if(sieve[a]) prime.push_back(a);

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	std::cout<<"done in "<<time_spent<<std::endl;
	

	return 0;
}