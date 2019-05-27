#ifndef _DIRECTIVE_H_
#define _DIRECTIVE_H_

#include "ParsedLine.h"
#include <unordered_map>

namespace codes {

	std::unordered_map<std::string, unsigned char> numOfParams = {
		{"text",0},
		{"data",0},
		{"bss",0},
		{"section",1},
		{"equ",2},
		{"global",-1},
		{"extern",-1},
		{"byte",1},
		{"word",1},
		{"align",1},
		{"skip",1},
		{"end",0},
	};

	std::unordered_map<std::string, std::string> directiveParsingGroup = {
		{"section","sectionName"},
		{"global", "symbol"},
		{"extern","symbol"},
		{"byte","expr"},
		{"word","expr"},
		{"align","val"},
		{"skip","val"},
	};
}

class Directive : public ParsedLine {
private:
	std::string param = "";
public:
	Directive() : ParsedLine() {}
	Directive(unsigned char typ, std::string lbl, std::string nam = "", unsigned char sz = 0) : ParsedLine(typ,lbl,nam,sz) {}

	std::string getParam() { return param; }
	void setParam(std::string parm) {	param = parm; }


};
#endif // !_DIRECTIVE_H_

