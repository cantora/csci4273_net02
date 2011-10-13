#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdlib>
#include <cstring>
#include <list>
#include <utility>

#define MESSAGE_STATIC 0
#define MESSAGE_DYNAMIC 1

namespace net02 {

template <int MSG_TYPE>
class message {
	public:
		message(char *msg, size_t msg_len);
		~message();
		
		void add_header(char *header, size_t hdr_len);
		char *strip_header(size_t hdr_len);
		size_t len() const;
		void flatten(char *buffer) const;

	private:
		
		std::list< std::pair<char *, size_t> > m_headers;
		const char *m_msg;
		const size_t m_msg_len;
		
}; /* message */

}; /* net02 */
#endif /* MESSAGE_H */
