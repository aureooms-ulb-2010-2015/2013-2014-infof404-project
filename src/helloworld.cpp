#include <iostream>
#include "os/task_t.h"
#include "lib/io.h"

int main(){
	std::cout << "Hello world!" << std::endl;
	os::task_t task;
	std::cout << task << std::endl;
	return 0;
}