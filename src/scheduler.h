#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cstdlib>
#include <list>

extern "C" {
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
}

#include "thread_pool.h"

namespace net02 {

class scheduler {
	public:
		scheduler(size_t max_events, thread_pool *pool);
		~scheduler();

		int schedule(void (*event_fn)(void *), void *args, int interval, uint32_t &event_id);
		int cancel(uint32_t event_id);

	private:
		struct event_t {
			timeval t;
			void (*event_fn)(void *);
			void *fn_args;
			uint32_t id;
			bool cancel;
		};

		static bool event_sort_time_asc(event_t &e1, event_t &e2);

		void id_to_event(uint32_t event_id, std::list<event_t>::iterator &itr);
			
		/* this class shouldn't be copied because of its mutex stuff */
		scheduler(const scheduler&);
		scheduler& operator=(const scheduler&);

		static void watch_clock(void *coord);

		thread_pool *const m_pool;
		const size_t m_max_events;
		uint32_t m_event_counter;

		struct coordination_t {
			/* list of events in time ascending order */
			std::list<event_t> events;
			bool terminate;
			pthread_mutex_t *mtx_p;
			pthread_cond_t *term_p;
			thread_pool *tp_p;
		};
		
		coordination_t m_coord;
		pthread_mutex_t m_coord_mtx;
		pthread_cond_t m_terminate;
				
		
}; /* scheduler */

}; /* net02 */
#endif /* SCHEDULER_H */
