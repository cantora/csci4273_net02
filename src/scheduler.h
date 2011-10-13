#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cstdlib>

extern "C" {
#include <pthread.h>
}

namespace net02 {

class scheduler {
	public:
		scheduler(size_t max_events);
		~scheduler();
				
	private:

		
}; /* scheduler */

}; /* net02 */
#endif /* SCHEDULER_H */
