#pragma once
#include <string>
using namespace std;
enum Type { 
	variable,
	constant,
	array,
	arrayField,
	arrayFieldConst,
	iterator,
	iteratorBound,
	pointer,
	undefined
};
class Id {
public:
	unsigned long long addr;
	Type type;
	Id(Type, unsigned long long addr = -1);
	Id();
};

class Variable : public Id {
public:
	bool initialized = false;
	Variable();
};

class ComplexArrayField : public Id {
public:
	ComplexArrayField(Id* array, Id* index);	
	Id* array;
	Id* index;	
};

class Iterator : public Id {
public:
	Iterator(string name,unsigned long long addr = -1);	
	string name;	
};

class Constant : public Id {
public:
	Constant( unsigned long long value, unsigned long long addr = -1 );
	bool initialize = false;
	bool little = false;
	unsigned long long value;
};

