#pragma once
#include <string>
#include <sstream>
using namespace std;
class Row{
	
public:
	string instruction;
	long long arg;
	Row(string, long long arg= -1);
	static Row makeRow(string, long long arg =-1);
	string toString();
	static bool isJump(Row&);
};