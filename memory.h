#pragma once
#include <unordered_map>
#include <string>
#include "id.h"
using namespace std;

class Memory{

public :

	
	bool put(string str,Id* id,unsigned long long size = 1);

	Id* get(string str);

	void drop(string name);

	int operational(int i);

	int tmpAddr();

	void clear();

	const unordered_map<string,Id*>& getTable();

	Memory();

private:
	unordered_map<string,Id*> table;

	unsigned long long variables;

	int tempAddr;

	int operations;

};