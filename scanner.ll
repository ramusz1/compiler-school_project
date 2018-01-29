%{ /* -*- C++ -*- */
# include <cstdlib>
# include <cerrno>
# include <string>
# include "driver.hh"
# include "parser.hh"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type.  */
#define yyterminate() return token::eof
%}

%option noyywrap

%{
# define YY_USER_ACTION  yylloc->columns (yyleng); 
%}

%x comment

blank [ \t]
pidentifier [_a-z]+
num [0-9]+

%%
%{
  yylloc->step();
%}
{blank}+   yylloc->step ();
[\n]+      yylloc->lines (yyleng); yylloc->step();     

%{
  typedef yy::Parser::token token;
%}

"("             BEGIN(comment);
<comment>")"    BEGIN(0);
<comment>.      ;
<comment>\n     ;

"VAR"           return token::var;
"BEGIN"         return token::begin;
"END"           return token::end;
"READ"          return token::read;
"WRITE"         return token::write;
"IF"            return token::IF;
"ELSE"          return token::ELSE;
"THEN"          return token::THEN;
"ENDIF"         return token::ENDIF;
"WHILE"         return token::WHILE;
"DO"            return token::DO;
"ENDWHILE"      return token::ENDWHILE;
"FOR"           return token::FOR;
"FROM"          return token::FROM;
"TO"            return token::TO;
"DOWNTO"        return token::DOWNTO;
"ENDFOR"        return token::ENDFOR;
":="            return token::set;
[-+*/%;\[\]]    return yy::Parser::token_type(yytext[0]);
">"             return token::gt;
"<"             return token::lt;
">="            return token::geq;
"<="            return token::leq;
"<>"            return token::neq;
"="             return token::eq;
{num}         {
                yylval->sval = new std::string(yytext);
                return token::num;
              }

{pidentifier} { 
                yylval->sval = new std::string(yytext); 
                return token::pidentifier;
              }

.       {
          std::string msg(yytext);
          msg = "unknown symbol: " + msg;
          driver.error(*yylloc,msg);
        }
%%


void Driver::scan_begin()
{
  if (file.empty () || file == "-")
    yyin = stdin;
  else if (!(yyin = fopen (file.c_str (), "r")))
  {
    error ("cannot open " + file + ": " + strerror(errno));
    exit (EXIT_FAILURE);
  }
}

void Driver::scan_end()
{
  fclose (yyin);
}

