#pragma once

#include "output.h"
#include "memory.h"
#include "driver.hh"
#include "id.h"

class Driver;

class Operation{
public:
	void run(Id *aId, Id *b);
	Operation(Output *output, Memory *memory, Driver *driver);
	~Operation();
protected:
	virtual void VV(Id *aId, Id *b) = 0;
	virtual void CV(Id *aId, Id *b);
	virtual void VC(Id *aId, Id *b);
	virtual bool CC(unsigned long long a, unsigned long long b) = 0;
	void load(Id* id,bool hard = false);
	void store(Id* id);
	Id* unpack(Id* id,bool withLoad = false, bool hardLoad = false);
	void add(Id*);
	void sub(Id*);
	void free(Id*);
	Output* output;
	Memory* memory;
	Driver* driver;
};

class Multiplication : public Operation{
	virtual void VV(Id *aId, Id *bId);
	virtual void CV(Id *aId, Id *bId);
	virtual void VC(Id *aId, Id *bId);
	virtual bool CC(unsigned long long a, unsigned long long b);
	using Operation::Operation;
};

class Addition : public Operation{
	virtual void VV(Id *aId, Id *bId);
	virtual void CV(Id *aId, Id *b);
	virtual void VC(Id *aId, Id *b);
	virtual bool CC(unsigned long long a, unsigned long long b);
	using Operation::Operation;
};

class Substraction : public Operation{
	virtual bool CC(unsigned long long a, unsigned long long b);
	virtual void VV(Id *aId, Id *bId);
	using Operation::Operation;
};

class Division : public Operation{
	virtual bool CC(unsigned long long a, unsigned long long b);
	virtual void VV(Id *aId, Id *bId);
	using Operation::Operation;
};

class Modulo : public Operation{
	virtual bool CC(unsigned long long a, unsigned long long b);
	virtual void VV(Id *aId, Id *bId);
	virtual void VC(Id *aId, Id *bId);
	using Operation::Operation;
};
