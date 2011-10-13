#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <cstdlib>
#include <vector>


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
		
		static void *thread_loop(void *thread_data);
	protected:
		
		
	private:
		class thread_sync {
			public:
				thread_sync() {
					pthread_mutex_init(&mtx, NULL);
					pthread_cond_init(&cond, NULL);
				}

				~thread_sync() {
					pthread_mutex_destroy(&mtx);
					pthread_cond_destroy(&cond);
				}
		
				pthread_mutex_t mtx;
				pthread_cond_t cond;
		};

		struct thread_data_t {
			int index;
			thread_sync sync;
		};

		struct thread_t {
			pthread_t id;
			thread_data_t data;
		};

		thread_t *const m_pool;
		const size_t m_pool_size;

}; /* thread_pool */

}; /* net02 */
#endif /* THREAD_POOL_H */
