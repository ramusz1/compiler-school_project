#include "accumulator.h"
#include <algorithm>

using namespace std;

bool Accumulator::isLoaded(Id *id){
	auto it = find(loaded.begin(),loaded.end(),id);
	return it != loaded.end();
}

void Accumulator::clear(){
	this->loaded.clear();
}

void Accumulator::addLoaded(Id *id){
	if( !isLoaded(id) ){
		loaded.push_back(id);
	}
}
