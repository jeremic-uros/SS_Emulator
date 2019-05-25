#ifndef _PARSER_EXCEP_H_
#define _PARSER_EXCEP_H_
#include <string>
#include <iostream>
using namespace std;

namespace util {
	class ParserException {
		string msg;
	public:
		ParserException() = default;
		ParserException(string pp) : msg(pp) {}
		~ParserException() {}
	protected:
		void write(ostream& it) const {
			it << "\n" << (msg != "" ? msg : "Error while parsing");
		}
		friend ostream& operator<< (ostream& it, const ParserException& pe) {
			pe.write(it);
			return it;
		}
	};
}
#endif // !_PARSER_EXCEP_H_
