#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdlib>
#include <cstring>
#include <list>
#include <utility>

namespace net02 {

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

		/* keep a record of every header given
		 * to add_header and delete all of them
 		 * when this object destructs
 		 */
		std::list<char *> m_dynamic_headers;

		char *m_msg;
		size_t m_msg_len;
		
}; /* message */

}; /* net02 */
#endif /* MESSAGE_H */
