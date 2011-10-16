#include "thread_pool.h"

#include <iostream>
#include <cerrno>
#include <cassert>
#include <cstdio>


using namespace net02;
using namespace std;

thread_pool::thread_pool(size_t thread_count) : m_pool_size(thread_count), m_pool(new thread_t [thread_count]) {

	long i, status;
	pthread_t thread;
	pthread_attr_t attr;
	size_t stacksize;

	assert(thread_count > 0);
	
	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);

	//pthread_attr_getstacksize (&attr, &stacksize);
	//cout << "Default stack size = " << stacksize << endl;

	stacksize = THREAD_POOL_STACKSIZE;
	pthread_attr_setstacksize (&attr, stacksize);

	//cout << "create " << m_pool_size << " threads" << endl; cout.flush();
	
	for(i = 0; i < m_pool_size; i++) {
		m_pool[i].data.index = i;
		status = pthread_create(&m_pool[i].id, &attr, thread_pool::thread_loop, (void *)&m_pool[i].data);
		if(status != 0) {
			throw errno;
		}
		
	}

	pthread_attr_destroy(&attr);
}

thread_pool::~thread_pool() {
	int i, status;
	
	for(i = 0; i < m_pool_size; i++) {
		//cout << "cancel thread " << i << endl;
		status = pthread_cancel(m_pool[i].id);
		if(status != 0) {
			throw errno;
		}
	}

	for(i = 0; i < m_pool_size; i++) {
		status = pthread_join(m_pool[i].id, NULL);
		if(status != 0) {
			throw errno;
		}
	}

	delete[] m_pool;
}


/*
 * finds a thread to run dispatch_fn(args). 
 * returns the index of the thread used to 
 * run the function. returns zero if dispatch
 * went ok, returns negative one to
 * indicate that all threads were busy
 */
int thread_pool::dispatch_thread(void (*dispatch_fn)(void *), void *args) { // , thread_pool::dispatch_id_t &did
	int i, status;

	assert(dispatch_fn != NULL);

	for(i = 0; i < m_pool_size; i++) {
		status = pthread_mutex_trylock(&m_pool[i].data.sync.dispatch_mtx);
		
		/* we locked the dispatch_mtx, so we can dispatch this thread */
		if(status == 0) {
			if(m_pool[i].data.sync.busy == false) {
				m_pool[i].data.dispatch_fn = dispatch_fn;
				m_pool[i].data.dispatch_fn_args = args;
				
				/* signal the worker (its blocking on dispatch conditions) */
				pthread_cond_signal(&m_pool[i].data.sync.dispatch);
				printf("thread_pool: sent dispatch signal to thread %d\n", m_pool[i].data.index);

				m_pool[i].data.sync.busy = true;

				/* increment job id to keep track of which job is which for cancellation purposes */
				/* nevermind, this isnt actually useful
				 *m_pool[i].data.job_id++; 
				 *did.worker_id = i;
				 *did.job_id = m_pool[i].data.job_id;*/

				pthread_mutex_unlock(&m_pool[i].data.sync.dispatch_mtx);
				/* return out of this for loop because we found a worker */
				return 0;
			}
			else {
				pthread_mutex_unlock(&m_pool[i].data.sync.dispatch_mtx);
			}
		}  
		else if(status == EBUSY) { /* this thread is busy right now */
			//do nothing
		}
		else { /* error */
			throw errno;
		}
	}	
	
	/* return negative value indicating that no threads were available */
	return -1;
}

bool thread_pool::thread_avail() {
	int i, status;
	bool available;

	for(i = 0; i < m_pool_size; i++) {
		status = pthread_mutex_trylock(&m_pool[i].data.sync.dispatch_mtx);
		
		/* we locked the dispatch_mtx, so this thread is available */
		if(status == 0) {
			available = !m_pool[i].data.sync.busy;
			pthread_mutex_unlock(&m_pool[i].data.sync.dispatch_mtx);

			/* its maybe possible theres a timing window where busy and mtx are not in sync
 			 * so we check to be sure busy is false;
			 */
			if(available) {
				return true;
			}
		}  
		else if(status == EBUSY) { /* this thread is busy right now */
			//do nothing
		}
		else { /* error */
			throw errno;
		}
	}	
	
	/* if we got here then all our trylocks gave EBUSY 
	 * or had data.sync.busy == true
	 */
	return false;
}

void *thread_pool::thread_loop(void *thread_data) {
	thread_data_t *mydata = (thread_data_t *) thread_data;

	/* lock dispatch mutex in order to wait */
	pthread_mutex_lock(&mydata->sync.dispatch_mtx);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	
	pthread_cleanup_push(thread_pool::thread_at_exit, thread_data);
	
	while(1) {
		printf("thread %d: waiting for dispatch...\n", mydata->index);

		/* set busy flag to false, indicating that
		 *  this thread is ready for work
		 */
		mydata->sync.busy = false;

		/* wait for a dispatch condition signal.
		 * dispatch_mtx must be locked when this is called.
		 * this function unlocks dispatch_mtx just before waiting
		 * and then relocks it just before waking up from a signal
		 */
		if( pthread_cond_wait(&mydata->sync.dispatch, &mydata->sync.dispatch_mtx) != 0) {
			throw errno;
		}
		printf("thread %d: calling dispatch_fn...\n", mydata->index);
		assert(mydata->sync.busy == true);
		
		/* do work! */
		mydata->dispatch_fn(mydata->dispatch_fn_args);
		
		/* loop and wait on dispatch condition. note that dispatch_mtx is still locked here. */		
	}

	/* shouldnt ever get here */
	pthread_cleanup_pop(1);
}

void thread_pool::thread_at_exit(void *thread_data) {
	thread_data_t *mydata = (thread_data_t *) thread_data;

	printf("thread %d: cancelled...\n", mydata->index);
}