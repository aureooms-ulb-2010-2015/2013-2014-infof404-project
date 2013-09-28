#include <iostream>
#include "os/task_t.h"
#include "os/task_system_t.h"
#include "lib/io.h"

int main(){
	std::cout << "Hello world!" << std::endl;
	
	{
	os::task_t task;
	std::cout << task << std::endl;
	}

	{
	os::task_system_t task_system(5);
	std::cout << task_system << std::endl;
	}
	return 0;
}