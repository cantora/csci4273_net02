#include <cerrno>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <cstdio>
#include <cstring>

#include "message.h"

using namespace net02;
using namespace std;


void test_construct() {
	char buf[] = "i am a doughy monster of cookies!!";
	char output[sizeof(buf)];
	message<MESSAGE_STATIC> m(buf, sizeof(buf) );
	
	//assert(m.len() == sizeof(buf) );
	
	//m.flatten(output);
	
	//assert(strcmp(output, buf) == 0);
}

int main() {
	try {
	cout << "BEGIN TEST MESSAGE" << endl;

	cout << "TEST CONSTRUCT" << endl;
	test_construct();

	cout << "END TEST MESSAGE" << endl;
	} /*try*/
	catch(int e) {
		cout << "caught exception: " << strerror(e) << endl;
		return 1;
	}

	return 0;
}