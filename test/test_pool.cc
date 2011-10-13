#include <cerrno>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <cstdio>
#include <cstring>

#include "thread_pool.h"

using namespace net02;
using namespace std;


void test_construct() {
	thread_pool *tp;
	int n_threads = 10;

	cout << "test thread pool construction" << endl;
	tp = new thread_pool(n_threads);
	
	sleep(1);
	
	assert(tp->thread_avail());
	
	delete tp;

	sleep(1);
}

#define TEST_DISPATCH_COUNT 100
static char test_dispatch_result[TEST_DISPATCH_COUNT];

void test_dispatch_fn(void *args) {
	printf("test_dispatch: %d\n", (long) args);
	test_dispatch_result[(long) args] = 1;
}

void test_dispatch() {
	thread_pool *tp;
	int i;
	int n_threads = 10;
	
	memset(test_dispatch_result, 0, TEST_DISPATCH_COUNT*sizeof(char));

	cout << "test thread pool dispatch" << endl;
	tp = new thread_pool(n_threads);
	
	sleep(1);
	
	assert(tp->thread_avail());
	
	for(i = 0; i < TEST_DISPATCH_COUNT; i++) {
		//cout << messages[i] << endl;
		//cout << i << endl;
		if(tp->dispatch_thread(test_dispatch_fn, (void *)i) < 0) {
			assert(false);
		}
	}	

	sleep(1);

	delete tp;

	for(i = 0; i < TEST_DISPATCH_COUNT; i++) {
		assert(1 == test_dispatch_result[i]);
	}

	sleep(1);
}

int main() {
	cout << "BEGIN TEST POOL" << endl;

	cout << "TEST CONSTRUCT" << endl;
	test_construct();

	cout << "TEST DISPATCH" << endl;
	test_dispatch();

	cout << "END TEST POOL" << endl;
}