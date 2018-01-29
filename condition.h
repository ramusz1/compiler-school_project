#pragma once
#include "id.h"

class Condition{
public:
	Id* id;
	bool wasLoaded;
	Condition(Id*,bool);
};