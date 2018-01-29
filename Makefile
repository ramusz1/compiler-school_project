all: main clear

main: 
	bison --defines=parser.hh -o parser.cc parser.yy
	flex -o scanner.cc scanner.ll
	g++ -o compiler.out scanner.cc parser.cc main.cc driver.cc memory.cpp output.cc row.cpp id.cc operations.cc accumulator.cc condition.cc -std=c++11

clear: 
	rm scanner.cc
	rm parser.cc
	rm parser.hh
	rm position.hh
	rm location.hh
	rm stack.hh