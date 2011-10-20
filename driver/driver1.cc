#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <cstdio>

using namespace std;

#include "thread_pool.h"

using namespace net02;

void test_fn(void *par)
{
	printf("in test_fn %d\n", (long)par);
}

main ( )
{
	thread_pool th(3);
	int max = 100;

    
    for (int i = 0; i < 20; i++) {
	max = 100 * i;
        while( th.thread_avail() != true) usleep(1000);

        //cout << "dispatch max " << max << endl;
	if(th.dispatch_thread(test_fn, (void *)max) < 0) {
		cout << "no threads available" << endl;
	}
	pthread_yield();
    }

    sleep(1); /* sleep to give the last couple threads time to finish */
}


