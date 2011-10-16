#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <cstdlib>

extern "C" {
#include <sys/time.h>
}

namespace net02 {

namespace time_util {
	
	const int USECS_PER_SEC = 1000000;

	void useconds_from_now(int interval, timeval &t);
}; /* time_util */

}; /* net02 */
#endif /* TIME_UTIL_H */
