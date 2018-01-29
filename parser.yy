%skeleton "lalr1.cc" 
%require "3.0.4"
%defines
%define parser_class_name {Parser}

%code requires {
# include <string>
class Driver;
class Id;
class Iterator;
class Condition;
}

// The parsing context.
%parse-param { Driver& driver }
%lex-param   { Driver& driver }
%locations
%initial-action
{
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &driver.file;
};
%debug
%error-verbose
%union
{
  int          ival;
  std::string *sval;
  Id          *idval;
  Iterator    *itval;
  Condition   *condval;
};

%code {
  #include "driver.hh"
  #include "id.h"
}

%token eof 0 
%token var
%token set
%token begin
%token end
%token read
%token write
%token IF
%token ELSE
%token THEN
%token ENDIF
%token WHILE
%token DO
%token ENDWHILE
%token FOR
%token FROM
%token TO
%token DOWNTO
%token ENDFOR
%token gt
%token lt
%token geq
%token leq
%token eq
%token neq
%type <idval> value
%type <idval> identifier
%token <sval> pidentifier
%token <sval> num
%type <itval> forConditionTo
%type <itval> forConditionDownTo
%type <condval> condition
%token '+'
%token '-'
%token '*'
%token '/'
%token '%'
%token ';'
%token '['
%token ']'



%start program
%%

program : 				
		| var vdeclarations begin commands end { driver.finish();}

vdeclarations: 
		| vdeclarations pidentifier {
      driver.updateLocation(@$);
      driver.declare($2); 
    }
    | vdeclarations pidentifier '['num']' {
      driver.updateLocation(@$); 
      driver.declareArray($2,$4);
    }

commands: commands command 	{ driver.join();}
		| command 			

command: identifier set expression ';' {driver.command_set($1);}
    | IF condition THEN commands ENDIF {driver.updateLocation(@$);driver.ifThen();delete $2;}
    | IF condition THEN commands else commands ENDIF {driver.updateLocation(@$);driver.ifElseThen();delete $2;}
    | WHILE condition DO commands ENDWHILE {driver.updateLocation(@$);driver.whileLoop($2);}
    | FOR forConditionDownTo DO commands ENDFOR {driver.updateLocation(@$);driver.forLoop($2,true);}
    | FOR forConditionTo DO commands ENDFOR {driver.updateLocation(@$);driver.forLoop($2,false);}
    | read identifier	';' { 
      driver.updateLocation(@$);
      driver.command_read($2); 
    }
		| write value	';' { 
      driver.updateLocation(@$);
      driver.command_write($2); 
    }

else : ELSE {
      driver.updateLocation(@$);
      driver.command_else();
    }

forConditionTo : pidentifier FROM value TO value {
        driver.updateLocation(@$);
        $$ = driver.forConditionTo($1,$3,$5);
      }
forConditionDownTo : pidentifier FROM value DOWNTO value {
        driver.updateLocation(@$);
        $$ = driver.forConditionDownTo($1,$3,$5);
      }

expression : value {
      driver.updateLocation(@$);
      driver.command_load($1);
    }
    | value '+' value {
      driver.updateLocation(@$);
      driver.command_add($1,$3);
    }
    | value '-' value {
      driver.updateLocation(@$);
      driver.command_subtr($1,$3);
    }
    | value '*' value {
      driver.updateLocation(@$);
      driver.command_multiply($1,$3);
    }
    | value '/' value {
      driver.updateLocation(@$);
      driver.command_divide($1,$3);
    }
    | value '%' value {
      driver.updateLocation(@$);
      driver.command_mod($1,$3);
    }

condition : value gt value {
      driver.updateLocation(@$);
      $$ = driver.greaterThan($1,$3);
    }
    | value lt value {
      driver.updateLocation(@$);
      $$ = driver.lessThan($1,$3);
    }
    | value geq value {
      driver.updateLocation(@$);
      $$ = driver.greaterEqual($1,$3);
    }
    | value leq value {
      driver.updateLocation(@$);
      $$ = driver.lessEqual($1,$3);
    }
    | value eq value {
      driver.updateLocation(@$);
      $$ = driver.equal($1,$3);
    }
    | value neq value {
      driver.updateLocation(@$);
      $$ = driver.notEqual($1,$3);
    }


value : num { 
      driver.updateLocation(@$);
      $$ = driver.makeConst($1);
    }
    | identifier {
      driver.updateLocation(@$);
      $$ = $1;
    }

identifier : pidentifier { 
      driver.updateLocation(@$);
      $$ = driver.getVariable($1);
    }
    | pidentifier '[' pidentifier ']' { 
      driver.updateLocation(@$);
      $$ = driver.getArrayField($1,$3);
    }
    | pidentifier '[' num ']' { 
      driver.updateLocation(@$);
      $$ = driver.getArrayFieldWithConst($1,$3);
    }

%%



void yy::Parser::error (const yy::Parser::location_type& l,
                          const std::string& m)
{
  driver.error(l, m);
}
