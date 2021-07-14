#define CATCH_CONFIG_RUNNER
#include "../utility/catch.hpp"

#include "prime_method.h"
#include "custom_method.h"
#include<iostream>
using namespace std;
int main(int argc, char* argv[]) {
	for (int id(0); id < argc; ++id) {
		cout << argv[id] << endl;
	}
	time_t timer_start, timer_end;
	OFEC::registerProblem();
	OFEC::registerAlgorithm();
	OFEC::setAlgForPro();
	time(&timer_start);
#ifdef OFEC_UNIT_TEST
	int result = Catch::Session().run(argc, argv);
#else
	OFEC::run();
#endif
	time(&timer_end);
	std::cout << "Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;
	return 0;
}