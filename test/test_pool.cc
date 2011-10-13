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

void test_dispatch(void *args) {
	printf("test_dispatch: %s\n", (char *) args);
}

BOOST_AUTO_TEST_CASE(dispatch) {
	thread_pool *tp;
	int i;
	int n_threads = 10;
	char *messages[] = {"Death is rarely scary in video games", "You can always just load a previous save and redo the portion of the game you struggled with", "or the game will simply bring your character back to life and plop you down right where you died.", "We complain about games that suffer from checkpoints that are too far from each other;", "we hate when we have to play the same section of the game again due to death.", "In multiplayer games death usually means you have to take a swig of your beer as you wait for your character to respawn.", "In Aliens: Infestation on the NIntendo DS, death means that you can never use that character again.", NULL};

	cout << "test thread pool dispatch" << endl;
	tp = new thread_pool(n_threads);
	
	sleep(1);
	
	BOOST_REQUIRE(tp->thread_avail());
	
	for(i = 0; messages[i] != NULL; i++) {
		//cout << messages[i] << endl;
		cout << i << endl;
		tp->dispatch_thread(test_dispatch, messages[i]);
	}	

	sleep(1);

	delete tp;

	sleep(1);
}

