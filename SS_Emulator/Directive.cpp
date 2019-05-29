#include "Directive.h"
#include "Util.h"
#include <queue>

std::unordered_map<std::string, unsigned char> Directive::numOfParams = {
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

std::unordered_map<std::string, std::string> Directive::directiveParsingGroup = {
	{"section","sectionName"},
	{"global", "symbolArray"},
	{"extern","symbolArray"},
	{"byte","expr"},
	{"word","expr"},
	{"align","val"},
	{"skip","val"},
};

void Directive::write(std::ostream & it) const{
	ParsedLine::write(it);
	it << param << delimiter;
}

void Directive::restore(std::string line){
	std::queue<std::string> tokens = util::tokenize(line, "!");
	std::string lineForBaseClass = "";
	for (int i = 0; i < 4; i++) {
		lineForBaseClass += tokens.front() + "!";
		tokens.pop();
	}
	ParsedLine::restore(lineForBaseClass);
	if (!tokens.empty()) param = tokens.front();
}
