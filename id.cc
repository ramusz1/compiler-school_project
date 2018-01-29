#include "id.h"
using namespace std;

Id::Id(Type t, unsigned long long addr){
	this->type = t;
	this->addr = addr;
}
Id::Id(){
	this->type = Type::undefined;
	this->addr = -1;
}

Variable::Variable(): Id(Type::variable){}

ComplexArrayField::ComplexArrayField(Id* array, Id* index){
	this->array = array;
	this->index = index;
	this->type = Type::arrayField;
}

Iterator::Iterator(string name, unsigned long long addr)
: Id(Type::iterator,addr)
{
	this->name = name;
}

Constant::Constant(unsigned long long value, unsigned long long addr ) 
: Id(Type::constant, addr)
{
	this->value = value;
	if( this-> value < 10){
		this->little = true;
	}
}

