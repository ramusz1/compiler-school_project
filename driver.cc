#include "driver.hh"
#include <cassert>
#include <sstream>

using namespace std;

unsigned long long Driver::parseNumber(string* value){
	stringstream sstr(*value);
	unsigned long long out;
	sstr >> out;
	return out;
}

// deletes arrayFields !!!
void Driver::load(Id *id, bool hard){

	if( !hard && id->type != Type::arrayField && accumulator.isLoaded(id)){
		return;
	}

	load_flag = true;

	if( id->type == Type::array){
		myErrorPrep();
		cout<<"wrong array usage\n";
	} else if( id->type == Type::arrayField){	
		ComplexArrayField* c = (ComplexArrayField* ) id ;
		output.buff("LOAD",c->array->addr);
		auto indexAddr = unpack(c->index);
		assert(indexAddr->type != Type::pointer);
		add(indexAddr);		//hope it's not a Type::pointer xD
		int tmpAddr = memory.tmpAddr();
		output.buff("STORE",tmpAddr);
		output.buff("LOADI",tmpAddr);
		delete c;
		accumulator.clear();
		return;
	} else if (id->type == Type::pointer) {
		output.buff("LOADI",id->addr);
	} else if(id->type == Type::variable){
		auto tmp = (Variable *) id;
		if ( ! tmp->initialized ){
			myErrorPrep();
			cout<<"Uninitialized variable\n";
		} 
		output.buff("LOAD",id->addr);
	} else if(id->type == Type::constant){
		auto tmp = (Constant *) id;
		if( tmp->little ){
			output.buff("ZERO");
			for(int i=0; i < tmp->value; i ++)
				output.buff("INC");	
		} else {
			tmp->initialize = true;
			output.buff("LOAD", id->addr);
		}

	} else {
		output.buff("LOAD",id->addr);
	}

	accumulator.clear();
	accumulator.addLoaded(id);
}

// withLoad == false by default
// deletes arrayFields !!!
Id* Driver::unpack(Id* id, bool withLoad,bool hardLoad){
	if( id->type == Type::arrayField){	
		ComplexArrayField* c = (ComplexArrayField*) id;
		output.buff("LOAD",c->array->addr);
		auto indexAddr = unpack(c->index);
		assert(indexAddr->type != Type::pointer);
		add(indexAddr);		//hope it's not a Type::pointer xD
		auto tmpAddr = memory.tmpAddr();		
		output.buff("STORE",tmpAddr);
		/*
		output.buff("STORE",tmpAddr);
		*/
		accumulator.clear();
		delete c;
		Id *out = new Id(Type::pointer,tmpAddr);
		if( withLoad ){
			load(out,hardLoad);
		}
		return out;
	} if (id->type == Type::pointer){
		Id *copy = new Id(id->type,id->addr);
		return copy;
	} else if(id->type == Type::variable){
		auto tmp = (Variable *) id;
		if ( ! tmp->initialized ){
			myErrorPrep();
			cout<<"Uninitialized variable\n";
		} 
	} 

	if( withLoad ){
		load(id,hardLoad);
	}
	return id;
}

// deletes arrayFields !!!
void Driver::store(Id* id){
	if( id->type == Type::array){
		myErrorPrep();
		cout<<"wrong array usage\n";
	} else if( id->type == Type::arrayField){	
		ComplexArrayField* c = (ComplexArrayField*) id;
		auto loadAddr = memory.tmpAddr();		
		output.buff("STORE",loadAddr);
		auto arrAddr = memory.tmpAddr();		
		output.buff("LOAD",c->array->addr);
		auto indexAddr = unpack(c->index);
		add(indexAddr);
		output.buff("STORE",arrAddr);
		output.buff("LOAD",loadAddr);
		output.buff("STOREI",arrAddr);
		delete c;
		// accumulator nothing, if something was loaded it still is
		return;
	} else if ( id->type == Type::iterator){
		// erororor
		myErrorPrep();
		cout<<"cannot modify iterator " <<endl;
	} else if ( id->type == Type::variable ) {
		auto tmp = (Variable *) id;
		tmp->initialized = true;
		output.buff("STORE",id->addr);
	} else if (id->type == Type::constant) {
		myErrorPrep();
		cout<<"unable to set value of a constant\n";
	} else if (id->type == Type::pointer){
		assert(false);
	} else {
		output.buff("STORE",id->addr);
	}
	accumulator.addLoaded(id);
}

//returns space taken
void Driver::sub(Id *id){
	if( id->type == Type::pointer){
		output.buff("SUBI",id->addr);
	} else if( id->type == Type::constant){
		auto c = (Constant *) id;
		if( c->little ){
			for(int i=0; i < c->value; i++){
				output.buff("DEC");
			}
		} else {
			output.buff("SUB",id->addr);
			c->initialize = true;
		}
	} else {
		output.buff("SUB",id->addr);
	}
}

void Driver::add(Id *id){
	if( id->type == Type::pointer){
		output.buff("ADDI",id->addr);
	} else if( id->type == Type::constant){
		auto c = (Constant*) id;
		if( c->little ){
			for(int i=0; i < c->value; i++){
				output.buff("INC");
			}
		} else {
			output.buff("ADD",id->addr);
			c->initialize = true;
		}
	} else {
		output.buff("ADD",id->addr);
	}
}

void Driver::free(Id *id){
	if( id->type == Type::pointer){
		delete id;
	}
}

void Driver::declare(string* variable){
	auto v = new Variable();
	if ( ! memory.put((*variable), v ) ){
		//TODO Errororroror
		myErrorPrep();
		cout<<"variable already defined : " << *variable <<endl;
		delete v;
	}
	delete variable;
}

void Driver::declareArray(string* variable, string* sizeStr){
	auto size = this->parseNumber(sizeStr);
	Id* i = new Id(Type::array);
	if ( ! memory.put((*variable),i,size+1) ){	//super important + 1 xd 
		//TODO Errororroror
		myErrorPrep();
		cout<<"array variable already defined : " << *variable <<endl;
		delete i;
	}
	delete sizeStr;
	delete variable;
	// TODO 
	// STORE addr at addr - 1
	createConst(i->addr+1,i->addr);
}

Id* Driver::getVariable(string* variable){
	auto a = memory.get((*variable));
	if( a->type == Type::undefined){
		//TODO Errororroror
		myErrorPrep();
		cout<<"err: no such variable " << *variable << endl;
	}
	return a;
}

Id* Driver::getArrayFieldWithConst(string* variable, string* index){
	auto value = this->parseNumber(index);
	auto a = this->getVariable(variable);
	if( a->type != Type::array ){
		// Errororroror
		myErrorPrep();
		cout<< *variable << " is not a array " << endl;
	}
	delete variable;
	delete index;
	return new Id(Type::arrayFieldConst,a->addr + value + 1);
}

Id* Driver::getArrayField(string* variable, string* index){
	auto a = this->getVariable(variable);
	auto b = this->getVariable(index);
	if( a->type != Type::array ){
		// erororor
		myErrorPrep();
		cout<< *variable << " is not a array " << endl;
	}
	if( b->type == Type::array ){
		// erororor
		myErrorPrep();
		cout<<"eroror: " << *index <<" is not a valid index\n";
	}
	delete variable;
	delete index;
	return new ComplexArrayField(a,b);
}

void Driver::createConst(unsigned long long value, unsigned long long addr){
	output.buff_clear();
	output.buff("ZERO");
	unsigned long long mask = 0x8000000000000000;
	int i = 64;
	while( i > 0 && ! (value & mask)){
		value = value << 1;
		i--;
	}
	bool ones = true;
	bool first = true;
	while( i > 0){
		int count = 0;
		while( i > 0 && ((bool)(value & mask) == ones) ){
			value = value << 1;
			i--;
			count++;
		}

		if(ones){		//sequence of ones
			if( count == 1 && first ){
				output.buff("INC");
				count--;
			} else if( count <= 2 ){
				while( count ){
					output.buff("SHL");
					output.buff("INC");
					count--;
				}
			} else {
				output.buff("INC");
				while( count ){
					output.buff("SHL");
					count--;
				}
				output.buff("DEC");
			}
		} else {		//sequence of zeroes
			while( count ){
				output.buff("SHL");
				count--;
			}
		}
		ones = !ones;
		first = false;
	}
	output.buff("STORE",addr);
	output.putFront();
}

Id* Driver::makeConst(unsigned long long value ){
	ostringstream os;		// meh
	os << value;
	auto constant = memory.get(os.str());
	if( constant->type == Type::constant ){
		return constant;
	}
	constant = new Constant(value);
	memory.put(os.str(),constant);
	return constant;
}

Id* Driver::makeConst(string* strVal){
	//cout<<"make Const: " << *strVal << endl;
	auto constant = memory.get((*strVal));
	if( constant->type == Type::constant){
		delete strVal;
		return constant;
	}
	auto value = this->parseNumber(strVal);
	constant = new Constant(value);
	memory.put(*strVal,constant);
	delete strVal;	
	return constant;
}

void Driver::command_set(Id* id){
	output.buff_clear();
	store(id);
	output.put();
	output.join();			//wow, this join is SOOO important XD
}

void Driver::command_load(Id* id){
	output.buff_clear();
	load(id);
	output.put();
}

void Driver::command_add(Id* aId,Id* bId){
	Addition a(&output,&memory,this);
	a.run(aId,bId);
}

void Driver::command_subtr(Id* aId,Id* bId){
	Substraction s(&output,&memory,this);
	s.run(aId,bId);
}

Condition* Driver::greaterThan(Id* aId,Id* bId){
	resetLoadFlag();
	command_subtr(aId,bId);
	output.buff_clear();
	output.buff("JZERO",-2);
	output.put();
	output.join();
	return new Condition(aId,load_flag);
}

Condition* Driver::lessThan(Id* a,Id* b){
	return this->greaterThan(b,a);
}

Condition* Driver::greaterEqual(Id* aId, Id* bId){
	resetLoadFlag();
	command_subtr(bId,aId);
	output.buff_clear();
	output.buff("JZERO",2);
	output.buff("JUMP",-2);
	output.put();
	output.join();
	return new Condition(bId,load_flag);
}

Condition* Driver::lessEqual(Id* a, Id* b){
	return this->greaterEqual(b,a);
}

Condition* Driver::equal(Id* aId, Id* bId){
	// doesn't include optimized Const - Const, 
	// operation - substracion api is not good here
	resetLoadFlag();
	output.buff_clear();
	auto b = unpack(bId);
	auto a = unpack(aId,true);
	bool tmpFlag = load_flag;
	sub(b);
	output.buff("JZERO", 2);
	output.buff("JUMP",-2);
	load(b,true);
	sub(a);
	output.buff("JZERO", 2);
	output.buff("JUMP", -2);
	output.put();
	free(a);
	free(b);
	accumulator.clear(); // to load it all
	return new Condition(aId,tmpFlag);
}

Condition* Driver::notEqual(Id* aId, Id* bId){
	// same problems as ::equal
	// ughghghgg, super weak
	int jumpLength = 1;
	if( aId->type == Type::constant ){
		Constant *c = (Constant*) aId;
		if( c->little ){
			jumpLength = c->value;
		}
	}
	if( bId->type == Type::constant ){
		Constant *c = (Constant*) bId;
		if( c->little ){
			jumpLength += c->value;
		}
	}
	output.buff_clear();
	auto b = unpack(bId);
	resetLoadFlag();
	auto a = unpack(aId,true);
	bool tmpFlag = load_flag;
	sub(b);
	output.buff("JZERO", 2);
	output.buff("JUMP", 3 + jumpLength);		//where should I jump XDDD 
	// jump over next substraction and the last JZERO
	load(b,true);
	sub(a);
	output.buff("JZERO", -2);
	output.put();
	free(a);
	free(b);
	accumulator.clear(); // to load it all
	return new Condition(aId,tmpFlag);
}

void Driver::ifThen(){
	int length = output.length(0);
	output.setJumps(1,length+1);
	output.join();
	accumulator.clear(); // to load it all
}

void Driver::ifElseThen(){
	int l1 = output.length(0);	//after else
	output.append(1,Row::makeRow("JUMP",l1+1) );
	int l2 = output.length(1);		//after if
	output.setJumps(2,l2+1);
	output.join();
	output.join();
	accumulator.clear(); // to loa3d it all
}

void Driver::command_else(){
	accumulator.clear();
}

void Driver::whileLoop(Condition *c){

	int l1 = output.length(0);
	int l2 = output.length(1);	
	output.buff_clear();
	if( !c->wasLoaded ){
		load(c->id);
	}
	output.put();
	int l3 = output.length(0);		//newly added piece
	output.buff_clear();
	output.buff("JUMP", -(l1 + l2 + l3) );
	output.put();
	output.join();
	output.join();
	this->ifThen();
	delete c;
}

Iterator* Driver::forConditionTo(string* iter, Id* from, Id* to){

	auto iterator = new Iterator(*iter);
	if ( ! memory.put((*iter), iterator, 2) ){
		//erororoor
		myErrorPrep();
		cout<<"undefined variable : " << *iter <<endl;
	}
	output.buff_clear();
	//	auto addr = unpack(iterator); ??
	auto addr = iterator->addr;
	load(to);
	output.buff("STORE",addr + 1);
	load(from);
	output.buff("STORE",addr);		//jeden load mniej można zrobić hmm
	output.put();
	output.buff_clear();
	output.buff("SUB",addr+1);
	output.buff("JZERO",2);
	output.buff("JUMP",-2);
	output.put();
	accumulator.clear();
	delete iter;
	return iterator;
}

Iterator* Driver::forConditionDownTo(string* iter, Id* from, Id* to){

	auto iterator = new Iterator(*iter);
	if ( ! memory.put((*iter), iterator, 2) ){
		//erororoor
		myErrorPrep();
		cout<<"undefined variable : " << *iter <<endl;
	}
	output.buff_clear();
	//	auto addr = unpack(iterator); ??
	auto addr = iterator->addr;
	load(to);
	output.buff("STORE",addr + 1);
	load(from);
	output.buff("STORE",addr);		//jeden load mniej można zrobić hmm
	output.put();
	output.buff_clear();
	output.buff("INC");
	output.buff("SUB",addr+1);
	output.buff("JZERO",-2);
	output.put();
	accumulator.clear();

	delete iter;
	return iterator;
}

void Driver::forLoop(Iterator* iter,bool downTo){
	int l1 = output.length(0);
	int l2 = output.length(1);	
	output.buff_clear();
	load(iter);//that next jump is more obvious
	int jumpDist = 3;
	if( downTo ){
		output.buff("JZERO", 4);
		output.buff("DEC");
		jumpDist = 4;
	} else {
		output.buff("INC");
	}
	output.buff("STORE", iter->addr);
	output.buff("JUMP", -(l1 + l2 + jumpDist) );
	output.put();
	output.join();
	this->ifThen();	
	output.join();
	memory.drop(iter->name);
	delete iter;
}

void Driver::command_multiply(Id* aId,Id* bId){
	Multiplication m(&output,&memory,this);
	m.run(aId,bId);
}

void Driver::command_divide(Id* aId,Id* bId){
	Division d(&output,&memory,this);
	d.run(aId,bId);
}

void Driver::command_mod(Id* aId,Id* bId){
	Modulo m(&output,&memory,this);
	m.run(aId,bId);
}


void Driver::command_read(Id* id){
	output.buff_clear();
	output.buff("GET");
	store(id);
	output.put();
	accumulator.clear();
	accumulator.addLoaded(id);
}

void Driver::command_write(Id* id){
	output.buff_clear();
	load(id);
	output.buff("PUT");
	output.put();
	
}

void Driver::join(){
	output.join();		
}

void Driver::finish(){
	output.buff_clear();
	output.buff("HALT");
	output.put();
	output.join();
	for(auto& id : memory.getTable()){
		if( id.second->type == Type::constant){
			auto constant = (Constant *) id.second;
			if( constant->initialize ){
				createConst(constant->value,constant->addr);
			} 
		}
	}

	output.finalize();
	memory.clear();
}

void Driver::clearAcc(){
	accumulator.clear();
}

void Driver::resetLoadFlag(){
	load_flag = false;
}

/*
 *	BASE FUNCTIONS FOR PARSER TO WORK
 */

Driver::Driver (const char *outputFile ): output(outputFile)
{
	err_flag = false;
}

Driver::~Driver ()
{

}

int Driver::parse (const std::string &f)
{
	file = f;
	scan_begin();
	yy::Parser parser(*this);
	int res = parser.parse();
	scan_end ();

	if( !err_flag){
		output.dump();
	} else {
		res = 1;
	}

	return res;
}

void Driver::myErrorPrep(){
	err_flag = true;
	cout<<"Error at: " << this->location << endl;
}

void Driver::error (const yy::location& l, const std::string& m)
{
	err_flag = true;
  	cerr << l << ": " << m << std::endl;
}

void Driver::error (const std::string& m)
{
	err_flag = true;
	cerr << m << std::endl;
}

void Driver::updateLocation(const yy::location l){
	this->location = l;
}
