
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <set>
#include <time.h>
#include <vector>




int main(int argc, char const *argv[])
{
	if (argc<3){
		std::cout<<"N?"<<std::endl;
		return 1;
	}
	clock_t begin, end;
	double time_spent;

	begin = clock();
	std::vector<int> pr;

	size_t n = std::stoull(argv[1]);
	size_t k = std::stoull(argv[2]);
	//
	//initialysation of M, pr[], s[]
	//
	int sqrtN= (int) floor(sqrt(n));
	bool  s [n];
	memset(s,true,sizeof(bool)*n);
	
	s[0]=false;
	s[1]=false;
	pr.push_back(2);
	int p=1;
	uint pcount=1,M=2;
	while( p<sqrtN or pr.size()<=k+1){ 

		if (s[p])//p est premier
		{
			pr.push_back(p);
			if(pcount<k){//M= prod des k 1er pr 
				pcount++;
				M*=p;
			}
			for (int i = p*p; i < sqrtN; i+=p)
			{
				s[i]=false;
			}
		}


		++p;
	}
	

	//init the wheel W[]
	//algo W 
	int W[M],prev=M-1;
	memset(W,1,sizeof(int)*M);//set all to one
	for(std::vector<int>::iterator it=pr.begin() ; it < pr.end(); it++ ) {
		for (int x = 0; x < (int)M; x+=*it)
        {
        	W[x]=0;
        }
    }

    W[(int)M-1]=2;
    for (int x = M-2; x > 1; --x)
    {
    	if (W[x]!=0)
    	{
    		W[x]=prev;
    		prev=x;
    	}
    }

	

    //Algo Pk
    //Pritchard using a wheel
    int q,x;
    for (int g = pr.at(k+1); g < pr.at(k+1)+M-1; g+=W[g%M])
    {
    	for (int f = g; f < (int)floor(n/pr.at(k+1)); f+=M)

    	{
    		q=pr.at(k+1);
    		x=q*f;
    		do{
    			p=q;
    			s[x]=false;
    			//q=nextprime(p);//What should getprime do?!
    			x=q*f;
    		}while(x>n or p%f ==0);

    	}
    }




	

	








	









	


	




	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	std::cout << "done in " << time_spent << std::endl;
	return 0;
}