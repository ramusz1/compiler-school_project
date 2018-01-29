#pragma once
#include <vector>
#include "row.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

class Output{
public:

	void putFront();

	void put();

	void buff(string instr,int arg=-1);

	void buff(Row);

	void buff_clear();

	void join();

	int length(int i);

	void setJumps(int countFromEnd, int length, long long rowArgs = -2);

	void append(int countFromEnd,Row r);

	/*
		call before dump, 
		consts initialization need to be joined,
		jumps need to be landed XD 
	*/
	void finalize();

	void dump();

	Output(const char* outputFile);

private:
	vector<vector<Row>> parts;
	vector<Row> buffor;
	string file;
};