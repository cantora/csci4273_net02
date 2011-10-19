#include "scheduler.h"

#include <iostream>
#include <cerrno>
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "time_util.h"

#include "log.h"

using namespace net02;
using namespace std;

scheduler::scheduler(size_t max_events, thread_pool *pool) : m_max_events(max_events), m_pool(pool), m_event_counter(0) {

	assert(m_max_events > 0);
	assert(m_pool != NULL);

	pthread_mutex_init(&m_coord_mtx, NULL);
	pthread_cond_init(&m_terminate, NULL);

	/* init m_coord structure */
	m_coord.terminate = false;
	m_coord.mtx_p = &m_coord_mtx;
	m_coord.term_p = &m_terminate;
	m_coord.tp_p = m_pool;

	/* keep trying to get a thread for our watch thread */
	while(m_pool->dispatch_thread(scheduler::watch_clock, &m_coord) < 0) {
		usleep(10000); /* 0.01 secs */
	}
}

/*
 * note: its possible some events triggered by this
 * object are still running. this should be fine 
 * since those events shouldnt have any references
 * to any stuff in this object.
 */
scheduler::~scheduler() {
	int status;

	/* shut down watch thread thread */
	while( (status = pthread_mutex_trylock(&m_coord_mtx) ) != 0) {
		if(status != EBUSY) {
			throw errno;
		}
		usleep(1000); /* 0.001 secs */
	}	
	
	/* got lock, now tell thread to terminate */
	m_coord.terminate = true;
		
	/* wait for thread to exit */
	NET02_LOG("scheduler: waiting for termination of watch thread...\n");
	if(pthread_cond_wait(&m_terminate, &m_coord_mtx) != 0) {
		throw errno;
	}
	
	NET02_LOG("scheduler: watch thread terminated.\n");
	if(pthread_mutex_unlock(&m_coord_mtx) != 0) {
		throw errno;
	}

	/* destroy mutex and cond */
	pthread_mutex_destroy(&m_coord_mtx);
	pthread_cond_destroy(&m_terminate);
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
	event_t e;
	
	assert(event_fn != NULL);
	assert(interval > 0);

	/* first get the offset from "now" so its as 
	 * accurate as possible.
 	 */
	time_util::useconds_from_now(interval, e.t);

	/* need to get a lock on m_coord to make a new event */
	while( (status = pthread_mutex_trylock(&m_coord_mtx) ) != 0) {
		if(status != EBUSY) {
			throw errno;
		}
		usleep(1000); /* 0.001 secs */
	}

	/* we have a lock, so make a new event */		
	if(m_coord.events.size() >= m_max_events) {
		return -1;
	}
	event_id = m_event_counter++;
		
	/* e.t already initialized */	
	e.event_fn = event_fn;
	e.fn_args = args;
	e.id = event_id;
	e.cancel = false;	

	NET02_LOG("scheduler: create event %d...\n", event_id);	
	m_coord.events.push_back(e);
	m_coord.events.sort(event_sort_time_asc); /* sort by time ascending */
	
	/* unlock m_coord */
	pthread_mutex_unlock(&m_coord_mtx);

	return 0;
}

/* if e1.t is greater than e2.t, return false, else return true.
 * in otherwords, if e1.t <= e2.t, e1 goes first in the order.
 */
bool scheduler::event_sort_time_asc(scheduler::event_t &e1, scheduler::event_t &e2) {
	return (!timercmp(&e1.t, &e2.t, >) );
}

void scheduler::id_to_event(uint32_t event_id, std::list<event_t>::iterator &itr) { 
	for(itr = m_coord.events.begin(); itr != m_coord.events.end(); itr++) {
		if(itr->id == event_id) {
			return;
		}
	}
}

/* returns negative value if the event was not found
 * otherwise returns 0 if the event was successfully
 * cancelled.
 */
int scheduler::cancel(uint32_t event_id) {
	int status, result;
	list<event_t>::iterator itr;
		
	while( (status = pthread_mutex_trylock(&m_coord_mtx) ) != 0) {
		if(status != EBUSY) {
			throw errno;
		}
		usleep(1000); /* 0.001 secs */
	}

	id_to_event(event_id, itr);

	if(itr != m_coord.events.end() ) {
		NET02_LOG("scheduler: set cancel for event %d...\n", event_id);
		itr->cancel = true;
		result = 0;
	}
	else {
		result = -1;
	}
	
	/* unlock m_coord */
	pthread_mutex_unlock(&m_coord_mtx);
	
	return result;
}


/*
 * watch the clock, checking each event to see if
 * we passed the time e->t. once that happens run the
 * event function in a new thread. this event can be 
 * cancelled any time before the event function is run 
 * (even after time t if we oversleep a little)
 */
void scheduler::watch_clock(void *coord) {
	coordination_t *c = (coordination_t *) coord;
	int status;
	list<event_t>::iterator itr;
	timeval now;

	NET02_LOG("watch thread: entering watch loop...\n");

	while(1) {
		usleep(100); /* best case scheduler resolution: 0.0001 seconds */

		/* need to get a lock on coord to make a new event */
		if( (status = pthread_mutex_trylock(c->mtx_p) ) != 0) {
			if(status != EBUSY) {
				throw errno;
			}
			continue; /* coord is busy */
		}

		/* we have a lock on m_coord */
		if(c->terminate) { /* we are destructing */
			NET02_LOG("watch thread: terminate...\n");
			if(pthread_cond_signal(c->term_p) != 0) {
				throw errno;
			}

			if(pthread_mutex_unlock(c->mtx_p) != 0) {
				throw errno;
			}
			return;
		}

		if(c->events.size() < 1) {
			goto unlock_coord_mtx;
		}
				
		for(itr = c->events.begin(); itr != c->events.end(); ) {
			list<event_t>::iterator tmp = itr++; /* use temp iterator so we can delete */
			if(tmp->cancel) {
				NET02_LOG("watch thread: cancel event %d...\n", tmp->id);
				c->events.erase(tmp);
				continue;
			}

			gettimeofday(&now, NULL);
			if( timercmp(&now, &tmp->t, <) ) {
				/* the earliest event is not ready,
				 * certainly nothing after it will be ready
				 */
				break; 
			}

			NET02_LOG("watch thread: trigger event %d...\n", tmp->id);
			while(c->tp_p->dispatch_thread(tmp->event_fn, tmp->fn_args) < 0) {
				usleep(100);
			}
			
			NET02_LOG("watch thread: erase event %d...\n", tmp->id);
			c->events.erase(tmp);
		}

	unlock_coord_mtx:
		pthread_mutex_unlock(c->mtx_p);
	} /* while(1) */
}
