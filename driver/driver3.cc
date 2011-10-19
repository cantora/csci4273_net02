#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;

#include "message.h"

using namespace net02;

main ( )
{
    char *b1 = new char[100];
    for (int i = 0; i < 100; i++) b1[i] = 'a';

    char *h1 = new char[5];
    for (int i = 0; i < 5; i++) h1[i] = 'h';

    char *h2 = new char[4];
    for (int i = 0; i < 4; i++) h2[i] = 'k';

    char *h3 = new char[24];
    for (int i = 0; i < 24; i++) h3[i] = 'm';

    message *m = new message(b1, 100);
    m->add_header(h1, 5);
    m->add_header(h2, 4);
    m->add_header(h3, 24);
    cout << "Message length = " << m->len( ) << endl;
    
    char* buf = new char[140];
    m->flatten(buf);
    buf[m->len ( )] = '\n';
    cout << "Message: " << buf << endl;

    message *m1 = new message(buf, 133);
    char *c1 = m1->strip_header(24);
    char *c1b = new char[25];
    memcpy(c1b, c1, 24);
    c1b[24] = '\0';
    cout << "Stripped header: " << c1b << endl;

    char *c2 = m1->strip_header(4);
    char *c2b = new char[5];
    memcpy(c2b, c2, 4);
    c2b[4] = '\0';
    cout << "Stripped header: " << c2b << endl;

    char *c3 = m1->strip_header(5);
    char *c3b = new char[6];
    memcpy(c3b, c3, 5);
    c3b[5] = '\0';
    cout << "Stripped header: " << c3b << endl;

    char* buf1 = new char[140];
    m1->flatten(buf1);
    buf[m1->len ( )] = '\n';
    cout << "Message: " << buf1 << endl;
}


