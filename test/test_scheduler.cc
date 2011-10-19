#include <cerrno>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <climits>

#include "scheduler.h"
#include "time_util.h"

using namespace net02;
using namespace std;


bool test_construct() {
	thread_pool tp(10);
	scheduler s(UINT_MAX, &tp);

	return true;	 
}

#define EVENT_COUNT 300
int flags[EVENT_COUNT];
int flag_counter = 0;

void set_flag(void *id) {
	flags[flag_counter++] = (long) id;
	//flag_counter++;
}

bool test_schedule_events() {
	thread_pool tp(10);
	scheduler s(UINT_MAX, &tp);
	uint32_t event_id;
	int i, status;

	memset(flags, 0xff, EVENT_COUNT*sizeof(int));

	for(i = 0; i < EVENT_COUNT; i++) {
		status = s.schedule(set_flag, (void *)i, (i+1)*10000, event_id); 
		if(status != 0) {
			return false;
		}
	}
	
	sleep(6);
	for(i = 0; i < EVENT_COUNT-1; i++) {
		printf("i,i+1: %d, %d\n", flags[i], flags[i+1]);
		if( (flags[i] + 1) != flags[i+1] ) {
			return false;
		}
	}

	return true;
}

void set_flag_alternate(void *id) {
	printf("set_flag_alternate: id %d at %d\n", (long) id, flag_counter);
	flags[flag_counter] = (long) id;
	flag_counter += 2;
}

bool test_cancel_events() {
	thread_pool tp(10);
	scheduler s(UINT_MAX, &tp);
	uint32_t event_id;
	int i, status;

	memset(flags, 0xff, EVENT_COUNT*sizeof(int));
	flag_counter = 1;

	for(i = 0; i < EVENT_COUNT; i++) {
		status = s.schedule(set_flag_alternate, (void *)i, (i+1)*10000, event_id); 
		if(status != 0) {
			return false;
		}

		if(event_id % 2 == 0) {
			s.cancel(event_id);
		}
	}
	
	sleep(6);
	for(i = 0; i < EVENT_COUNT-1; i++) {
		if(i % 2 == 0) {
			printf("%d: %d\n", i, flags[i] );
			if(flags[i] != -1) {
				return false;
			}
		}
		else {
			printf("%d, %d: %d, %d\n", i, i+2, flags[i], flags[i+2]);
			if( (flags[i] + 2) != flags[i+2] ) {
				return false;
			}
		}
	}

	return true;
}

struct blah_t {
	int index;
	timeval t;
};
#define DIFF_COUNT 20
double difftimes[DIFF_COUNT];
#define MAX_DELAY (6*1000000)

void get_diff(void *args) {
	timeval now,diff;
	blah_t *bla = (blah_t *)args;
	double a,b;

	gettimeofday(&now, NULL);
	a = now.tv_sec + now.tv_usec/1000000.0;
	b = bla->t.tv_sec + bla->t.tv_usec/1000000.0;

	printf("get_diff %d: %f - %f\n", bla->index, a, b);
	timersub(&now, &bla->t, &diff);
	difftimes[ bla->index ] = diff.tv_sec + diff.tv_usec/1000000.0;

	//timersub(&now, (timeval *)(args + 4), &difftimes[0]);
	//printf("get_diff: %d\n", *((long *) args));
}

bool test_timing() {
	thread_pool tp(10);
	scheduler s(UINT_MAX, &tp);
	int i, status, interval;
	uint32_t event_id;
	blah_t blah;

	srand(time(NULL));
	
	for(i = 0; i < DIFF_COUNT; i++) {
		interval = (rand() % MAX_DELAY) + 1;
		blah.index = i;
		time_util::useconds_from_now(interval, blah.t);
		status = s.schedule(get_diff, (void *) &blah, interval, event_id); 
		if(status != 0) {
			return false;
		}

		usleep(interval + 1000000);
		printf("diff for event %d: %f\n", i, difftimes[i]);
		if(difftimes[i] > 0.001) {
			return false;
		}
	}

	return true;
}	

int main() {
	bool tests[4];
	int i = 0;

	try {
	cout << "BEGIN TEST SCHEDULER" << endl;

	cout << "TEST CONSTRUCT" << endl;
	tests[i++] = test_construct();

	cout << "TEST SCHEDULE EVENTS" << endl;
	tests[i++] = test_schedule_events();

	cout << "TEST CANCEL EVENTS" << endl;
	tests[i++] = test_cancel_events();

	cout << "TEST TIMING" << endl;
	tests[i++] = test_timing();	

	cout << "END TEST SCHEDULER" << endl;
	} /*try*/
	catch(int e) {
		cout << "caught exception: " << strerror(e) << endl;
		return 1;
	}

	cout << endl;
	for(i = 0; i < 4; i++) {
		cout << "test" << i << ": " << (tests[i]? "+" : "-") << endl;
	}

	return 0;
}