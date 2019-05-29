#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#include <string>

class Symbol {
public:
	enum Type {
		GLOBAL = 0,
		LOCAL = 1,
		ABSOLUT = 2
	};
private:
	std::string name;
	std::string section;
	unsigned short value;
	enum Type type;
	unsigned short rb;
public:
	Symbol(std::string nm,std::string sec,unsigned short val,Symbol::Type typ,unsigned short rbb) : name(nm),section(sec),value(val),type(typ),rb(rbb) {}

};
#endif // !_SYMBOL_H_

