#include "ParsedLine.h"
#include "Util.h"
#include <queue>
#include <string>
#include <iostream>

void ParsedLine::write(std::ostream & it) const{
	it << (int)type << delimiter << "label=" + label << delimiter << (int)size << delimiter << name << delimiter;
}

void ParsedLine::restore(std::string line){
	std::queue<std::string> tokens = util::tokenize(line,"!");
	// get Type
	type = util::convertStringToDecimal(tokens.front());
	tokens.pop();
	// get Label
	label = tokens.front();
	if (label == "label=") label = "";
	else label = label.substr(6, label.size() - 1);
	tokens.pop();
	// get Size
	size = util::convertStringToDecimal(tokens.front());
	tokens.pop();
	// get name
	if(!tokens.empty()) name = tokens.front();
}

std::ostream & operator<<(std::ostream & it, const ParsedLine & pe){
	pe.write(it);
	return it;
}
