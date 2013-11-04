
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <time.h>


int main(int argc, char const *argv[]){
	
	if (argc<2)
	{
		std::cout<<"N?"<<std::endl;
		return 1;
	}
	clock_t begin, end;
	double time_spent;

	begin = clock();

	int n_max=std::stoull(argv[1]);


	std::vector<uint64_t> prime;
	bool sieve[n_max];	
	memset(sieve, 0, sizeof(sieve));

	prime.push_back(2);
	prime.push_back(3);
	prime.push_back(5);

	sieve[2]=sieve[3]=sieve[5]=true;

	int N = sqrt(n_max);
	for (int x = 1; x <= N ; ++x)
	{
		for (int y = 1; y <= N; ++y)
		{
			int n = (4*x*x)+(y*y);
			if ((n <= n_max) && ((n % 12 == 1) or (n % 12 == 5)))
			{
				sieve[n] ^= true;//Xor with true toggle bool
			}
			n = (3*x*x)+(y*y);
			if ( (n <=n_max) && (n % 12 == 7 ))
			{
				sieve[n] ^= true;
			}
			n = (3*x*x)-(y*y);
			if ((x>y) && (n<=n_max) && (n %12==11))
			{
				sieve[n] ^= true;
			}
		}
	}
	
	int a;
   for (a = 7; a <= N; a+=2){
      if (sieve[a]){
         for (int i = a*a; i < n_max; i += a*a)
            sieve[i] = false;
         prime.push_back(a);
      }
   }
   for (; a < n_max; a+=2) if (sieve[a])
      prime.push_back(a);

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	std::cout<<"done in "<<time_spent<<std::endl;
	

	return 0;
}