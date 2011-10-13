#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <cstdlib>

extern "C" {
#include <pthread.h>
}

#define THREAD_POOL_STACKSIZE (1 << 19) /* 1/2 megabyte */

/*
 * thread_pool
 */
namespace net02 {

class thread_pool {
	public:
		thread_pool(size_t thread_count);
		~thread_pool();
		
		int dispatch_thread(void (*dispatch_fn)(void *), void *arg);
		bool thread_avail();

	private:

		static void *thread_loop(void *thread_data);
		static void thread_at_exit(void *thread_data);
	
		class thread_sync {
			public:
				thread_sync() : busy(false) {
					pthread_mutex_init(&dispatch_mtx, NULL);
					pthread_cond_init(&dispatch, NULL);
				}

				~thread_sync() {
					pthread_mutex_destroy(&dispatch_mtx);
					pthread_cond_destroy(&dispatch);
				}
				
				pthread_mutex_t dispatch_mtx;
				pthread_cond_t dispatch;
				bool busy;
		};

		struct thread_data_t {
			thread_sync sync;
			int index;
			void (*dispatch_fn)(void *);
			void *dispatch_fn_args;			
		};

		struct thread_t {
			thread_data_t data;
			pthread_t id;
			
		};

		thread_t *const m_pool;
		const size_t m_pool_size;

}; /* thread_pool */

}; /* net02 */
#endif /* THREAD_POOL_H */
