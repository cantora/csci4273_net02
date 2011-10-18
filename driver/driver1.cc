#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;

#include "thread_pool.h"

void *test_fn(void *par)
{
	cout << "in test_fn " << *(int *)par << endl;
}

main ( )
{
	thread_pool th(3);
	int max = 100;

    for (int i = 0; i < 20; i++) {
	max = 100 * i;
	th.dispatch_thread(test_fn, (void *)&max);
	pthread_yield( );
% You may replace the above command with the following
%       sleep(1);        
    }
}


