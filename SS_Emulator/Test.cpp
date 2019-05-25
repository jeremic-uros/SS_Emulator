
#include "Lexer.h"
#include <string>
#include <iostream>
#include <queue>

using namespace std;
int main() {

	Lexer* lexer = Lexer::getLexer();
	string testString1 = ".wor<d> !1!2?3'4";
	string testString2 = "xchgw [sp],		0x1234";
	string testString3 = "a: .skip 8";
	queue<string>* res1 = lexer->tokenize(testString1);
	queue<string>* res2 = lexer->tokenize(testString2);
	queue<string>* res3 = lexer->tokenize(testString3);
	while (!res2->empty()) {
		cout << res2->front() << endl;
		res2->pop();
	}

	free(res1);
	free(res2);
	free(res3);
	int x;
	cin >> x;
	return 0;
}