
#ifndef DRIVER_HH
#define DRIVER_HH

#include <string>
#include "parser.hh"
#include "memory.h"
#include "output.h"
#include "id.h"
#include "operations.h"
#include "accumulator.h"
#include "condition.h"

// Tell Flex the lexer's prototype ...
# define YY_DECL                                        \
  yy::Parser::token_type                         \
  yylex (yy::Parser::semantic_type* yylval,      \
         yy::Parser::location_type* yylloc,      \
         Driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

using namespace std;

// Conducting the whole scanning and parsing of Calc++.
class Driver
{
public:
  friend class Operation;
  Driver (const char* outputFile);
  virtual ~Driver ();

  void declare(string* variable);
  void declareArray(string* variable,string* size);
  Id* getVariable(string* variable);
  Id* getArrayField(string* variable, string* index);
  Id* getArrayFieldWithConst(string* variable, string* index);
  Id* makeConst(string * str);
  Id* makeConst(unsigned long long);
  Condition* greaterThan(Id* a, Id* b);
  Condition* lessThan(Id* a, Id* b);
  Condition* greaterEqual(Id* a, Id* b);
  Condition* lessEqual(Id* a, Id* b);
  Condition* equal(Id* a, Id* b);
  Condition* notEqual(Id* a, Id* b);
  void ifThen();
  void ifElseThen();
  void whileLoop(Condition*);
  Iterator* forConditionTo(string* iter,Id* from, Id* to);
  Iterator* forConditionDownTo(string* iter,Id* from, Id* to);
  void forLoop(Iterator* iter, bool downTo);
  void command_add(Id* a,Id* b);
  void command_subtr(Id* a,Id* b);
  void command_multiply(Id* a,Id* b);
  void command_divide(Id* a,Id* b);
  void command_mod(Id* a,Id* b);
  void command_load(Id* id);
  void command_read(Id* id);
  void command_write(Id* id);
  void command_set(Id* a);
  void join();
  void finish();
  void command_else();


  bool err_flag;
  // Handling the scanner.
  void scan_begin ();
  void scan_end ();
  bool trace_scanning; 
  // Run the parser.  Return 0 on success.
  int parse (const string& f);
  std::string file;
  bool trace_parsing;
  // Error handling.
  void error (const yy::location& l, const string& m);
  void error (const string& m);
  void updateLocation(const yy::location l);
  void load(Id*, bool hard = false);
  void store(Id*);
  Id* unpack(Id*, bool withLoad = false, bool hardLoad = false);
  void add(Id*);
  void sub(Id*);
  void free(Id *);
  void clearAcc();
private:
  void resetLoadFlag();
  bool load_flag;
  unsigned long long parseNumber(string* str);
  void createConst(unsigned long long, unsigned long long addr);
  void myErrorPrep();
  yy::location location;
  Memory memory;
  Output output;
  Accumulator accumulator;
};

#endif 
