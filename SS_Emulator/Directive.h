#ifndef _DIRECTIVE_H_
#define _DIRECTIVE_H_

#include "ParsedLine.h"
#include <unordered_map>



class Directive : public ParsedLine {
private:
	std::string param;
protected:
	void write(std::ostream& it) const override;
public:
	Directive() : ParsedLine(), param("") {}
	Directive(unsigned char typ, std::string lbl, std::string nam = "", unsigned char sz = 0) : ParsedLine(typ,lbl,nam,sz),param("") {}

	std::string getParam() { return param; }
	void setParam(std::string parm) {	param = parm; }

	void restore(std::string line) override;

	static std::unordered_map<std::string, unsigned char> numOfParams;
	static std::unordered_map<std::string, std::string> directiveParsingGroup;

};
#endif // !_DIRECTIVE_H_

