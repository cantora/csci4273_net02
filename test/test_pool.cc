#define BOOST_TEST_MODULE "net02:test_pool"
#include <boost/test/unit_test.hpp>

#include <cerrno>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "thread_pool.h"

extern "C" {
}

using namespace net02;
using namespace std;


BOOST_AUTO_TEST_CASE(construct) {
	thread_pool *tp;

	cout << "test thread pool construction" << endl;
	tp = new thread_pool(10);
	
	pthread_exit(NULL);
	delete tp;
}

