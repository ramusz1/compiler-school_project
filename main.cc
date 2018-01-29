#include <iostream>
#include "driver.hh"

int main (int argc, char *argv[])
{
	if( argc < 3){
		std::cout<<"Usage: input output\n";
		return 0;
	}
	Driver driver(argv[2]);
	return driver.parse (argv[1]);
}

