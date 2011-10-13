#include "message.h"

#include <iostream>
#include <cerrno>
#include <cassert>

using namespace net02;
using namespace std;

message::message(char *msg, size_t msg_len) : m_msg(msg), m_msg_len(msg_len) {
	assert(m_msg_len > 0);
	assert(m_msg != NULL);
}

message::~message() {
	char *hdr;
	
	while(!m_headers.empty()) {
		hdr = m_headers.top().first;
		delete[] hdr;
		m_headers.pop();
	}

	delete[] m_msg;
}

void message::add_header(char *header, size_t hdr_len) {
	assert(header != NULL);
	assert(hdr_len > 0);

	//m_headers.push(pair<char *, size_t>(
}

char *message::strip_header(size_t hdr_len) {

}

size_t message::len() const {

}

void message::flatten(char *buffer) const {

}

