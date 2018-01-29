#include "operations.h"
#include <sstream>
using namespace std;

void Operation::run(Id *aId, Id *bId){
	if( aId->type == Type::constant && bId->type == Type::constant){
		auto constB = (Constant *) bId;
		auto b = constB->value;
		auto constA = (Constant *) aId;
		auto a = constA->value;	
		if ( !CC(a,b) ){		//overflow
			cout <<"overflow\n";
			VC(aId,bId);
		}
	} else if (aId->type == Type::constant){
		CV(aId,bId);
	} else if (bId->type == Type::constant){
		VC(aId,bId);
	} else {
		VV(aId,bId);
	}
	driver->clearAcc();
}

void Operation::load(Id* id,bool hard){
	driver->load(id, hard);
}

void Operation::store(Id* id){
	driver->store(id);
}

Id* Operation::unpack(Id* id,bool withLoad, bool hardLoad){
	return driver->unpack(id,withLoad,hardLoad);
}

void Operation::sub(Id *id){
	driver->sub(id);
}

void Operation::add(Id *id){
	driver->add(id);
}

void Operation::free(Id *id){
	driver->free(id);
}

Operation::Operation(Output *output, Memory *memory, Driver *driver){
	this->output = output;
	this->memory = memory;
	this->driver = driver;
}

Operation::~Operation(){

}

void Operation::CV(Id *aId, Id *bId){
	VV(aId,bId);
}

void Operation::VC(Id *aId, Id *bId){
	VV(aId,bId);
}

bool Multiplication::CC( unsigned long long a, unsigned long long b){
	auto x = a * b;
	if (a != 0 && x / a != b) {
    	// overflow handling
    	return false;
	}	

	output->buff_clear();
	auto id = driver->makeConst( x );
	load(id);
	output->put();
	return true;
}

/*
	similar to Driver::createConst
*/
void Multiplication::VC(Id* aId, Id *bId){

	auto constId = (Constant *) bId;	 
	auto value = constId->value;
	
	output->buff_clear();
	auto a = unpack(aId);
	output->buff("ZERO");

	unsigned long long mask = 0x8000000000000000;
	int i = 64;
	while( i > 0 && ! (value & mask)){
		value = value << 1;
		i--;
	}
	bool ones = true;
	while( i > 0){
		int count = 0;
		while( i > 0 && ((bool)(value & mask) == ones) ){
			value = value << 1;
			i--;
			count++;
		}

		if(ones){		//sequence of ones
			if( count <= 2 ){
				while( count ){
					output->buff("SHL");
					add(a);
					count--;
				}
			} else {
				add(a);
				while( count ){
					output->buff("SHL");
					count--;
				}
				sub(a);
			}
		} else {		//sequence of zeroes
			while( count ){
				output->buff("SHL");
				count--;
			}
		}
		ones = !ones;
	}
	output->put();
	free(a);
}

void Multiplication::CV(Id *aId,Id *bId){
	VC(bId,aId);
}

void Multiplication::VV(Id *aId, Id *bId){
	int result = memory->operational(0);
	int aCopy = memory->operational(1);
	int bCopy = memory->operational(2);

	output->buff_clear();
	output->buff("ZERO");
	output->buff("STORE", result);
	auto a = unpack(aId);
	auto b = unpack(bId,true,true);		//lol
	// output->buff("LOAD",b); done
	sub(a);
	//swap
	output->buff("JZERO",6);
	load(b,true);
	output->buff("STORE",aCopy);
	load(a,true);
	output->buff("STORE",bCopy);
	output->buff("JUMP",5);
	load(a,true);
	output->buff("STORE",aCopy);
	load(b,true);
	output->buff("STORE",bCopy);
	// end swap
	output->buff("JODD",2);
	output->buff("JUMP",4);
	output->buff("LOAD",result);
	output->buff("ADD",aCopy);
	output->buff("STORE",result);
	output->buff("LOAD",aCopy);
	output->buff("SHL");
	output->buff("STORE",aCopy);
	output->buff("LOAD",bCopy);
	output->buff("SHR");
	output->buff("JZERO",3);
	output->buff("STORE",bCopy);
	output->buff("JUMP",-12);
	output->buff("LOAD",result);
	output->put();
	free(a);
	free(b);
}

void Addition::VV(Id *aId, Id *bId){
	output->buff_clear();
	auto b = unpack(bId);
	driver->load(aId);
	add(b);
	output->put();
	free(b);
}

bool Addition::CC( unsigned long long a, unsigned long long b){
	auto x = a + b;
	if ( x < a || x < b ) {
    	// overflow handling
    	return false;
	}	

	output->buff_clear();
	auto id = driver->makeConst( x );
	load(id);
	output->put();
	return true;
}

void Addition::CV(Id *aId, Id *bId){
	VC(bId,aId);
}


void Addition::VC(Id *aId, Id *bId){
	auto constId = (Constant *) bId;
	auto value = constId->value;
	if( !constId->little ){
		VV(aId,bId);
		return;
	}
	output->buff_clear();
	driver->load(aId);
	for(int i = 0; i < value; i ++){
		output->buff("INC");
	}
	output->put();
}

void Substraction::VV(Id *aId, Id *bId){
	output->buff_clear();
	auto b = unpack(bId);
	load(aId);
	sub(b);
	output->put();
	free(b);
}

bool Substraction::CC( unsigned long long a, unsigned long long b){
	output->buff_clear();
	Id * id;
	if( a >= b){
		id = driver->makeConst( a - b);
	} else {
		id = driver->makeConst( (unsigned long long) 0);
	}
	load(id);
	output->put();
	return true;
}

void Division::VV(Id *aId, Id *bId){
	int result = memory->operational(0);
	int aCopy = memory->operational(1);
	int bCopy = memory->operational(2);
	int divider = memory->operational(3);

	output->buff_clear();
	load(aId);			//it's on the top to get some lucky no_load hits
	output->buff("STORE",aCopy);
	output->buff("ZERO");
	output->buff("STORE",result);
	output->buff("INC");
	output->buff("STORE",divider);
	load(bId,true);
	output->buff("STORE",bCopy);
	output->buff("JZERO",30);
	output->buff("SHL");		// while b' <= a', b' << 2, div << 2
	output->buff("SUB",aCopy);
	output->buff("JZERO",2);
	output->buff("JUMP",8);
	output->buff("LOAD",divider);
	output->buff("SHL");
	output->buff("STORE",divider);
	output->buff("LOAD",bCopy);
	output->buff("SHL");
	output->buff("STORE",bCopy);
	output->buff("JUMP",-10); 		// endwhile

	output->buff("LOAD",aCopy);
	output->buff("INC");
	output->buff("SUB",bCopy);
	output->buff("JZERO",6);
	output->buff("DEC");
	output->buff("STORE",aCopy);
	output->buff("LOAD",result);		// result += divider
	output->buff("ADD",divider);
	output->buff("STORE",result);		
	output->buff("LOAD",divider);		// label xd 
	output->buff("SHR");
	output->buff("STORE",divider);
	output->buff("JZERO",5);
	output->buff("LOAD",bCopy);		
	output->buff("SHR");
	output->buff("STORE",bCopy);
	output->buff("JUMP",-16);
	output->buff("LOAD",result);
	output->put();
	
}

bool Division::CC( unsigned long long a, unsigned long long b){
	output->buff_clear();
	Id *id;
	if(b == 0)
		id = driver->makeConst( (unsigned long long)0 );
	else
		id = driver->makeConst( a / b);
	load(id);
	output->put();
	return true;
}

bool Modulo::CC( unsigned long long a, unsigned long long b){
	output->buff_clear();
	Id *id;
	if(b == 0)
		id = driver->makeConst( (unsigned long long)0 );
	else
		id = driver->makeConst( a % b);

	load(id);
	output->put();
	return true;
}

void Modulo::VV(Id *aId, Id *bId){
	int aCopy = memory->operational(0);
	int bCopy = memory->operational(1);

	output->buff_clear();
	load(aId);
	output->buff("STORE",aCopy);
	auto bValue = unpack(bId,true,true);
	output->buff("STORE",bCopy);
	output->buff("JZERO",30);
	output->buff("SHL");		// while b' <= a', b' << 2, div << 2
	output->buff("SUB",aCopy);
	output->buff("JZERO",2);
	output->buff("JUMP",5);
	output->buff("LOAD",bCopy);
	output->buff("SHL");
	output->buff("STORE",bCopy);
	output->buff("JUMP",-7); 		// endwhile

	output->buff("LOAD",aCopy);
	output->buff("INC");
	output->buff("SUB",bCopy);
	output->buff("JZERO",10);
	output->buff("DEC");
	output->buff("STORE",aCopy);
	output->buff("INC");
	sub(bValue);
	output->buff("JZERO",12);
	output->buff("LOAD",bCopy);		//label xd
	output->buff("SHR");
	output->buff("STORE",bCopy);
	output->buff("JUMP",-12);
	output->buff("LOAD",bCopy);		//label xd
	output->buff("SHR");
	output->buff("STORE",bCopy);
	output->buff("INC");
	sub(bValue);
	output->buff("JZERO",2);
	output->buff("JUMP",-19);
	output->buff("LOAD",aCopy);
	output->put();
	free(bValue);
}


void Modulo::VC(Id *aId, Id *bId){
	int aCopy = memory->operational(0);
	int bCopy = memory->operational(1);

	int jumpLength = 1;

	output->buff_clear();
	load(aId);
	output->buff("STORE",aCopy);
	auto bValue = unpack(bId,true,true);		//bValue is a Type::constant

	auto c = (Constant*) bValue;
	if( c->little ){
		jumpLength = c->value;
	}

	output->buff("STORE",bCopy);
	output->buff("JZERO",28 + 2 * jumpLength);
	output->buff("SHL");		// while b' <= a', b' << 2, div << 2
	output->buff("SUB",aCopy);
	output->buff("JZERO",2);
	output->buff("JUMP",5);
	output->buff("LOAD",bCopy);
	output->buff("SHL");
	output->buff("STORE",bCopy);
	output->buff("JUMP",-7); 		// endwhile

	output->buff("LOAD",aCopy);
	output->buff("INC");
	output->buff("SUB",bCopy);
	output->buff("JZERO",9 + jumpLength);
	output->buff("DEC");
	output->buff("STORE",aCopy);
	output->buff("INC");
	sub(bValue);
	output->buff("JZERO",11 + jumpLength);
	output->buff("LOAD",bCopy);		//label xd
	output->buff("SHR");
	output->buff("STORE",bCopy);
	output->buff("JUMP",- 11 - jumpLength);
	output->buff("LOAD",bCopy);		//label xd
	output->buff("SHR");
	output->buff("STORE",bCopy);
	output->buff("INC");
	sub(bValue);
	output->buff("JZERO",2);
	output->buff("JUMP",- 17 - 2 * jumpLength);
	output->buff("LOAD",aCopy);
	output->put();
	free(bValue);
}

