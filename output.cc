#include "output.h"
#include <cassert>
using namespace std;

int Output::length(int i){
	return ( parts.rbegin() + i )->size();
}

void Output::setJumps(int countFromEnd, int length, long long rowArg){
	auto it = parts.rbegin() + countFromEnd;
	int i = 0;
	for(auto row = it->rbegin(); row != it->rend(); row++){
		if( row->arg == rowArg){
			row->arg = i + length;
		}
		i++;
	}
}

void Output::append(int countFromEnd,Row r){
	auto it = parts.rbegin() + countFromEnd;
	it->push_back(r);
}


void Output::buff_clear(){
	buffor.clear();
}

void Output::buff(string instr, int arg){
	buffor.push_back(Row::makeRow(instr,arg));
}

void Output::buff(Row r){
	buffor.push_back(r);
}

void Output::putFront(){
	parts.insert(parts.begin(),buffor);
}

void Output::put(){
	parts.push_back(buffor);
}

void Output::join(){
	auto b = parts.rbegin();
	auto a = b+1;
	a->reserve(a->size() + b->size() );
	a->insert(a->end(), b->begin(), b->end());
	parts.pop_back();
}

void Output::dump(){
	ofstream out(file);
	for(int i = 0; i < parts[0].size() ; i++){
		//cout<<i<< "\t" <<parts[0][i].toString()<<endl;
		out<<parts[0][i].toString()<<endl;
	}
	out.close();
}

void Output::finalize(){

	while( parts.size() > 1){
		join();
	} 
	
	// set final labels : 
	
	for(int i = 0; i < parts[0].size(); i ++){
		if( Row::isJump(parts[0][i]) ){
			parts[0][i].arg += i;
		}
	}
}

Output::Output(const char* file):file(file)
{
	//empty	
}

