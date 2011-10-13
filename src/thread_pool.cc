#include "thread_pool.h"

#include <iostream>
#include <cerrno>

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
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_getstacksize (&attr, &stacksize);

	cout << "Default stack size = " << stacksize << endl;

	stacksize = THREAD_POOL_STACKSIZE;
	pthread_attr_setstacksize (&attr, stacksize);

	for(i = 0; i < m_pool_size; i++) {
		m_pool[i].data.index = i;
		if( (status = pthread_create(&m_pool[i].id, &attr, thread_pool::thread_loop, (void *)&m_pool[i].data) ) != 0) {
			throw errno;
		}
		
	}

	pthread_attr_destroy(&attr);
}

thread_pool::~thread_pool() {
	delete[] m_pool;
}

void *thread_pool::thread_loop(void *thread_data) {
	thread_data_t *mydata = (thread_data_t *) thread_data;

	sleep(mydata->index);	
	cout << "thread number " << mydata->index << endl;
	cout.flush();
}