#include "time_util.h"

#include <cassert>

using namespace std;
using namespace net02;

/* returns the timeval representing interval 
 * microseconds from now in the output 
 * parameter t
 */
void time_util::useconds_from_now(int interval, timeval &t) {
	timeval delta, now;

	assert(interval >= 0);
	
	gettimeofday(&now, NULL);

	if(interval == 0) {
		t = now;
		return;
	}

	delta.tv_sec = interval/USECS_PER_SEC;
	delta.tv_usec = interval % 1000000;
	timeradd(&now, &delta, &t);

}