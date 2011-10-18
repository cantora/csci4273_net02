#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;

//#include "threadpool.h"
#include "eventscheduler.h"

void *test_fn1(void *par)
{
	cout << "in test_fn1 " << *(int *)par << endl;
}

void *test_fn2(void *par)
{
	cout << "in test_fn2 " << *(int *)par << endl;
}

void *test_fn3(void *par)
{
	cout << "in test_fn3 " << *(int *)par << endl;
}

main ( )
{
    ThreadPool th(3);
    EventScheduler es(10, &th);

    int m1 = 100, m2 = 200, m3 = 300, m4 = 5000, m5 = 1100, m6 = 1120;
    int a1 = 100, a2 = 200, a3 = 300, a4 = 5000, a5 = 1100, a6 = 1120;
    int i1, i2, i3, i4, i5, i6;

    i1 = es.eventSchedule(test_fn1, (void *)&a5, m5);
    i2 = es.eventSchedule(test_fn2, (void *)&a1, m1);
    i3 = es.eventSchedule(test_fn3, (void *)&a4, m4);
    i4 = es.eventSchedule(test_fn1, (void *)&a3, m3);
    es.eventCancel(i3);
    i5 = es.eventSchedule(test_fn2, (void *)&a2, m2);
    i6 = es.eventSchedule(test_fn3, (void *)&a6, m6);
}


