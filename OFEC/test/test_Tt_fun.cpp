#include "../utility/catch.hpp"
#include"../utility/elementary_function.h"
#include"../core/global.h"
#include<iostream>
using namespace std;
using namespace OFEC;


TEST_CASE("utility", "[elementary_function]")
{
	InstanceManager::ms_instance_manager.reset(new InstanceManager);

	std::pair<double, double> from_x_range = { 0,1 };
	std::pair<double, double> to_x_range = { 0,1 };
	std::pair<double, double> to_y_range = { 0,1 };
	int sample_fre = 100;
	auto id=ADD_RND(0.1);
	random_fun test_fun(id,sample_fre,from_x_range,to_x_range,to_y_range);
	test_fun.initialize();

	const int max_n = 10000;
	for (int id(0); id < max_n; ++id) {
		cout << "id\t" << id << "\t" << test_fun.get_value(id *1.0/ max_n) << endl;
	}

}