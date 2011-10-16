#include "scheduler.h"

#include <iostream>
#include <cerrno>
#include <cassert>
#include <cstdio>


using namespace net02;
using namespace std;

scheduler::scheduler(size_t max_events, thread_pool *pool) : m_max_events(max_events), m_pool(pool) {

	assert(m_max_events > 0);
	assert(m_pool != NULL);

	pthread_mutex_init(&m_coord_mtx, NULL);

	/* init m_coord structure */
	m_coord.terminate = false;
	m_coord.mtx = &m_coord_mtx;

	/* keep trying to get a thread for our watchdog */
	while(m_pool->dispatch_thread(scheduler::watch_clock, ) < 0) {
		usleep(100000); /* 0.1 secs */
	}
}

/*
 * note: its possible some events triggered by this
 * object are still running. this should be fine 
 * since those events shouldnt have any references
 * to any stuff in this object.
 */
scheduler::~scheduler() {
	/* shut down watchdog thread */
	
	
	/* destroy mutex */
	pthread_mutex_destroy(&m_coord_mtx);
}

/* returns 0 if event was successfully scheduled, and
 * returns non-zero if we hit the max_events ceiling
 * the event_id output parameter will have the event 
 * identifier if the function was successful.
 * interval parameter specifies microsecond 
 * offset from "now"
 */
int scheduler::schedule(void (*event_fn)(void *), void *args, int interval, uint32_t &event_id) {
	int status;
	list<event_t>::iterator itr;
	event e;
	
	assert(event_fn != NULL);
	assert(args != NULL);
	assert(interval > 0);

	/* first get the offset from "now" so its as 
	 * accurate as possible.
 	 */
	time_util::useconds_from_now(interval, e.t);

	/* need to get a lock on m_coord to make a new event */
	while( (status = pthread_mutex_trylock(m_coord_mtx) ) != 0) {
		if(status != EBUSY) {
			throw errno;
		}
		usleep(1000); /* 0.001 secs */
	}

	/* we have a lock, so make a new event */
		
	if(m_coord.events.size() >= m_max_events) {
		return -1;
	}
	event_id = ++event_counter;
		
	/* e.t already initialized */	
	e.event_fn = event_fn;
	e.fn_args = args;
	e.id = event_id;
	e.cancel = false;	
	
	m_coord.events.push_back(e);
	m_coord.events.sort(event_sort); /* sort by time ascending */
	
	/* unlock m_events */
	pthread_mutex_unlock(m_events_mutex);

	return 0;
}

/* if e1.t is greater than e2.t, return false, else return true.
 * in otherwords, if e1.t <= e2.t, e1 goes first in the order.
 */
bool scheduler::event_sort(scheduler::event_t &e1, scheduler::event_t &e2) {
	return (!timercmp(e1.t, e2.t, >) );
}

void scheduler::cancel(uint32_t event_id) {

}


/*
 * watch the clock, checking each event to see if
 * we passed the time e->t. once that happens run the
 * event function in a new thread. this event can be 
 * cancelled any time before the event function is run 
 * (even after time t if we oversleep a little)
 */
void scheduler::watch_clock(void *coord) {
	coordination_t *c = (coord_t *) coord;
	int status;
	list<event_t>::const_iterator itr;

	while(1) {
		usleep(10000); /* best case scheduler resolution: 0.01 seconds */

		/* need to get a lock on coord to make a new event */
		if( (status = pthread_mutex_trylock(*(coord->mtx)) ) != 0) {
			if(status != EBUSY) {
				throw errno;
			}
			continue; /* coord is busy */
		}

		/* we have a lock on m_coord */
		if(coord->terminate) {
			pthread_mutex_unlock(*(coord->mtx) );
			return;
		}

		if(coord->events.size() < 1) {
			goto unlock_coord_mtx;
		}
		
		for(itr = coord->events.begin(); itr != coord->events.end(); itr++) {
			//check if time is passed
		}

	unlock_coord_mtx:
		pthread_mutex_unlock(*(coord->mtx) );
	} /* while(1) */
}