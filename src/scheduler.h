#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cstdlib>

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

		uint32_t schedule(void (*event_fn)(void *), void *args, int interval);
		void cancel(uint32_t event_id);

	private:
		class event {
			public:
				/* pass sleep_interval=0 for a non-sleeping event watcher */
				event(timeval start_time, void (*event_callback)(void *), useconds_t sleep_interval) 
					: t(start_time), event_fn(event_callback), sleep_ivl(sleep_interval), cancelled(false) {
					pthread_mutex_init(&event_mtx, NULL);
				}

				~event() {
					pthread_mutex_destroy(&event_mtx);
				}

				const timeval t;
				void (*const event_fn)(void *);
				pthread_mutex_t event_mtx;
				bool cancelled;
				const useconds_t sleep_ivl;
			
			private:
				/* this class shouldn't be copied because of its mutex stuff */
				event(const event&);
				event& operator=(const event&);
		
		}

		/* this class shouldn't be copied because of its mutex stuff */
		scheduler(const scheduler&);
		scheduler& operator=(const scheduler&);

		static void wait_for_event(void *event_ptr);

		thread_pool *const m_pool;
		size_t m_max_events;
		std::map<uint32_t, event *> m_events;
		pthread_mutex_t m_events_mtx;
		
		
}; /* scheduler */

}; /* net02 */
#endif /* SCHEDULER_H */
