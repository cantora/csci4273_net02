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
message::message(char *msg, size_t msg_len) {
	add_header(msg, msg_len);
}

message::~message() {
	char *hdr;
	
	while(!m_to_delete.empty()) {
		hdr = m_to_delete.front();
		delete[] hdr;
		m_to_delete.pop_front();
	}
}

void message::add_header(char *header, size_t hdr_len) {
	assert(header != NULL);
	assert(hdr_len > 0);

	m_to_delete.push_front(header);
	m_buffers.push_front(make_pair(header, hdr_len));
}

/* 
 * this function assumes that no call to
 * strip header will want to strip only part 
 * of a header, thus it just pops the top
 * header off the stack.
 */
char *message::strip_header(size_t hdr_len) {
	char *hdr;
	//pair<char *, size_t> top;
	list< pair<char *, size_t> >::iterator itr;

	assert(!m_buffers.empty());
	assert(m_buffers.front().second >= hdr_len);

	/*for(itr = m_buffers.front(); itr != m_buffers; ) {
		tmp = itr;
		itr++;	
	}*/

	itr = m_buffers.begin();
	hdr = itr->first;
	if(hdr_len < itr->second) {
		itr->second -= hdr_len;
		itr->first += hdr_len;
	}
	else {
		m_buffers.pop_front();
	}
	
	return hdr;
}


size_t message::len() const {
	list< pair<char *, size_t> >::const_iterator itr;
	int sum = 0;

	for(itr = m_buffers.begin(); itr != m_buffers.end(); itr++) {
		sum += itr->second;			
	}

	return sum;
}

void message::flatten(char *buffer) const {
	list< pair<char *, size_t> >::const_iterator itr;

	for(itr = m_buffers.begin(); itr != m_buffers.end(); itr++) {
		memcpy(buffer, itr->first, itr->second);
		buffer += itr->second;
	}
}

