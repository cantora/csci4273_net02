#include "scheduler.h"

#include <iostream>
#include <cerrno>
#include <cassert>
#include <cstdio>


using namespace net02;
using namespace std;

scheduler::scheduler(size_t max_events, thread_pool *pool) : m_max_events(max_events), m_pool(pool) {

	assert(max_events > 0);
	assert(pool != NULL);

	pthread_mutex_init(&m_events_mtx, NULL);

}

scheduler::~scheduler() {
	pthread_mutex_destroy(&m_events_mtx);
}

/* returns 0 if event was successfully scheduled, and
 * returns non-zero if we hit the max_events ceiling
 * the event_id output parameter will have the event 
 * identifier if the function was successful.
 */
int scheduler::schedule(void (*event_fn)(void *), void *args, int interval, uint32_t &event_id) {
	int status;
	event *e;

	/* need to get a lock on m_events to make a new event */
	while( (status = pthread_mutex_trylock(m_events_mutex) ) != 0) {
		if(status != EBUSY) {
			throw errno;
		}
		usleep(100000); /* 0.1 secs */
	}

	/* we have a lock, so make a new event */
	
	event_id = m_events.size();
	if(event_id >= m_max_events) {
		return -1;
	}
	while(m_events.count(event_id) ) {
		event_id = (event_id + 1) % m_max_events;
	}
	
	e = &m_events[event_id];
	e->event_fn = event_fn;
	//e	
	/* unlock m_events */
	pthread_mutex_unlock(m_events_mutex);

	
}

void scheduler::cancel(uint32_t event_id) {

}


/*
 * watch the clock (sleeping at e->sleep_ivl intervals) 
 * until we pass the time e->t. once that happens run the
 * event function. this event can be cancelled any time
 * before the event function is run (even after time t
 * if we oversleep a little)
 */
void scheduler::wait_for_event(void *event_ptr) {
	event *e = (event *) event_ptr;
	
	
}