#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;

#include "scheduler.h"

using namespace net02;

void test_fn1(void *par)
{
	cout << "in test_fn1 " << *(int *)par << endl;
}

void test_fn2(void *par)
{
	cout << "in test_fn2 " << *(int *)par << endl;
}

void test_fn3(void *par)
{
	cout << "in test_fn3 " << *(int *)par << endl;
}

main ( )
{
    thread_pool th(3);
    scheduler es(10, &th);

    int m1 = 100, m2 = 200, m3 = 300, m4 = 5000, m5 = 1100, m6 = 1120;
    int a1 = 100, a2 = 200, a3 = 300, a4 = 5000, a5 = 1100, a6 = 1120;
    uint32_t i1, i2, i3, i4, i5, i6;

    es.schedule(test_fn1, (void *)&a5, m5, i1);
    es.schedule(test_fn2, (void *)&a1, m1, i2);
    es.schedule(test_fn3, (void *)&a4, m4, i3);
    es.schedule(test_fn1, (void *)&a3, m3, i4);
    es.cancel(i3);
    es.schedule(test_fn2, (void *)&a2, m2, i5);
    es.schedule(test_fn3, (void *)&a6, m6, i6);

    sleep(5);
}


