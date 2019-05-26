#ifndef _PARSER_EXCEP_H_
#define _PARSER_EXCEP_H_
#include <string>
#include <iostream>


namespace util {
	class AssemblerException : public std::exception {
		std::string msg;
	public:
		AssemblerException() = default;
		AssemblerException(std::string pp) : msg(pp) {}
		~AssemblerException() {}
	protected:
		void write(std::ostream& it) const {
			it << "\n" << (msg != "" ? msg : "Error while parsing");
		}
		friend std::ostream& operator<< (std::ostream& it, const AssemblerException& pe) {
			pe.write(it);
			return it;
		}
	};
}
#endif // !_PARSER_EXCEP_H_
