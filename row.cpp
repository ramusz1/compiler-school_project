#include "row.h"

Row::Row(string instruction, long long arg){
	this->instruction = instruction;
	this->arg = arg;
}

Row Row::makeRow(string instruction, long long arg){
	Row r(instruction,arg);
	return r;
}

string Row::toString(){
	ostringstream ss;
	ss << instruction;
	if(arg != -1){
	 	ss << " " <<  arg;
	}	
	return ss.str();
}

bool Row::isJump(Row& r){
	return r.instruction == "JUMP" 
		|| r.instruction == "JZERO" 
		|| r.instruction == "JODD";
}
