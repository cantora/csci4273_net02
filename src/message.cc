#include "message.h"

#include <iostream>
#include <cerrno>
#include <cassert>

using namespace net02;
using namespace std;

/*
 * add list of headers to delete even if theyve been stripped, take full ownership
 * of all char * added to message
 * allow partial strips by shifting the pointer "up" len bytes. this is fine since
 * original pointer is saved in the delete list and caller no longer has 
 * obligation to delete the stripped header.
 */
message::message(char *msg, size_t msg_len) : m_msg(msg), m_msg_len(msg_len) {
	assert(m_msg_len > 0);
	assert(m_msg != NULL);
}

message::~message() {
/*
	if(MSG_TYPE != MESSAGE_STATIC) {
		char *hdr;
	
		while(!m_headers.empty()) {
			hdr = m_headers.front().first;
			delete[] hdr;
			m_headers.pop_front();
		}

		delete[] m_msg;
	}
*/
}

void message::add_header(char *header, size_t hdr_len) {
	assert(header != NULL);
	assert(hdr_len > 0);

	m_headers.push_front(make_pair(header, hdr_len));
}

/* 
 * this function assumes that no call to
 * strip header will want to strip only part 
 * of a header, thus it just pops the top
 * header off the stack.
 * the caller has the responsibility for 
 * deleting the data at the returned char *
 */
char *message::strip_header(size_t hdr_len) {
	char *hdr;

	assert(!m_headers.empty());
	assert(m_headers.front().second == hdr_len);

	hdr = m_headers.front().first;
	m_headers.pop_front();

	return hdr;
}


size_t message::len() const {
	list< pair<char *, size_t> >::const_iterator itr;
	int sum = 0;

	for(itr = m_headers.begin(); itr != m_headers.end(); itr++) {
		sum += itr->second;			
	}

	return m_msg_len + sum;
}

void message::flatten(char *buffer) const {
	list< pair<char *, size_t> >::const_iterator itr;
	
	for(itr = m_headers.begin(); itr != m_headers.end(); itr++) {
		memcpy(buffer, itr->first, itr->second);
		buffer += itr->second;
	}

	memcpy(buffer, m_msg, m_msg_len);
}

