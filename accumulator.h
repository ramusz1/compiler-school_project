#pragma once
#include <vector>
#include "id.h"
using namespace std;

class Accumulator {

public:
	bool isLoaded(Id *);
	void clear();
	void addLoaded(Id *);
private:
	vector<Id*> loaded;
};