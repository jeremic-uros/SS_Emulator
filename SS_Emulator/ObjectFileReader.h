#ifndef _OBJECT_FILE_READER_H_
#define _OBJECT_FILE_READER_H_

#include "Symbol.h"
#include "Section.h"
#include <unordered_map>
#include <string>

class ObjectFileReader {
private:
	ObjectFileReader() {}
public:

	static void readSymbolandSectionTable(std::unordered_map<unsigned short, Symbol>& symTable, std::unordered_map<std::string, Section>&sectTable, std::ifstream & in);

};

#endif // !_OBJECT_FILE_READER_H_

