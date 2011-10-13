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
	int n_threads = 10;

	cout << "test thread pool construction" << endl;
	tp = new thread_pool(n_threads);
	
	sleep(1);
	
	BOOST_REQUIRE(tp->thread_avail());
	
	delete tp;

	sleep(1);
}

#define TEST_DISPATCH_COUNT 100
static char test_dispatch_result[TEST_DISPATCH_COUNT];

void test_dispatch(void *args) {
	printf("test_dispatch: %d\n", (int) args);
	test_dispatch_result[(int) args] = 1;
}

BOOST_AUTO_TEST_CASE(dispatch) {
	thread_pool *tp;
	int i;
	int n_threads = 10;
	
	memset(test_dispatch_result, 0, TEST_DISPATCH_COUNT*sizeof(char));

	cout << "test thread pool dispatch" << endl;
	tp = new thread_pool(n_threads);
	
	sleep(1);
	
	BOOST_REQUIRE(tp->thread_avail());
	
	for(i = 0; i < TEST_DISPATCH_COUNT; i++) {
		//cout << messages[i] << endl;
		//cout << i << endl;
		if(tp->dispatch_thread(test_dispatch, (void *)i) < 0) {
			BOOST_REQUIRE(false);
		}
	}	

	sleep(1);

	delete tp;

	for(i = 0; i < TEST_DISPATCH_COUNT; i++) {
		BOOST_REQUIRE_EQUAL(1, test_dispatch_result[i]);
	}

	sleep(1);
}

