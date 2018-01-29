#include "memory.h"
#include <iostream>
using namespace std;

bool Memory::put(string str, Id* id,unsigned long long size){
	if(table.find(str) != table.end())
		return false;	
	// this element does not exist yet
	table[str] = id;
	id->addr = variables;
	variables+=size;
	return true;
}

Id* Memory::get(string str){
	if( table.find(str) == table.end())
		return new Id(Type::undefined);		
	
	return table[str];
}

void Memory::drop(string name){
	table.erase(name);
}

int Memory::operational(int i){
	return operations + i;
}

int Memory::tmpAddr(){
	int out = tempAddr;
	tempAddr = ( tempAddr + 1 ) % 3;
	return out;
}

const unordered_map<string,Id*>& Memory::getTable(){
	return table;
}

void Memory::clear(){
	for(auto& p : table){
		delete p.second;
	}
}

Memory::Memory(){
	tempAddr = 0;
	operations = 3;
	variables = 8;
}

