%option c++ prefix="og_scanner_" outfile="og_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include <cdk/types/typename_type.h>
#include <cdk/types/basic_type.h>
#include <memory>
#include "og_parser.tab.h"
// don't change this
#define yyerror LexerError

static std::ostringstream strlit;

%}
%x X_STRING X_ESCAPE X_IGNORE X_COMMENT

DIGIT                   [0-9]
EXPONENT                [eE][-+]?{DIGIT}+

%%

    /* ====================================================================== */
    /* ====[                      3.2 - COMMENTS                        ]==== */
    /* ====================================================================== */

"//".*\n                ; /* ignore single line comment content */

"/*"                    yy_push_state(X_COMMENT);
<X_COMMENT>"/*"         yy_push_state(X_COMMENT);
<X_COMMENT>"*/"         yy_pop_state();
<X_COMMENT>.|\n         ; /* ignore multiline comment content */

    /* ====================================================================== */
    /* ====[                      3.4 - TYPES                           ]==== */
    /* ====================================================================== */

"int"                   return tINT_TYPE;
"real"                  return tREAL_TYPE;
"string"                return tSTRING_TYPE;
"ptr"                   return tPOINTER_TYPE;
"auto"                  return tAUTO;

    /* ====================================================================== */
    /* ====[                      3.8.1 - INTEGERS                      ]==== */
    /* ====================================================================== */

{DIGIT}+                  {
                            try {
                                yylval.i = std::stoi(yytext, nullptr, 10);
                                return tINTEGER;
                            } catch (const std::out_of_range &e) {
                                yyerror("Integer out of range");
                            } 
                        }
0[xX][0-9A-Fa-f]+       {
                            try {
                                yylval.i = std::stoi(yytext, nullptr, 16);
                                return tINTEGER;
                            } catch (const std::out_of_range &e) {
                                yyerror("Hexadecimal integer out of range");
                            } 
                        }   
                      
    /* ====================================================================== */
    /* ====[                      3.8.2 - REALS                         ]==== */
    /* ====================================================================== */

{DIGIT}+{EXPONENT}|({DIGIT}+\.{DIGIT}*|{DIGIT}*\.{DIGIT}+){EXPONENT}?   { 
                                                                            yylval.d = strtod(yytext, nullptr);
                                                                            return tREAL;
                                                                        }

    /* ====================================================================== */
    /* ====[                      3.8.3 - STRINGS                       ]==== */
    /* ====================================================================== */

"\""                    yy_push_state(X_STRING);
<X_STRING>"\\"          yy_push_state(X_ESCAPE);
<X_STRING,X_IGNORE>"\""     {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                return tSTRING;
                            }         
<X_STRING>.             strlit << *yytext;

<X_ESCAPE>"0"           { 
                            yy_pop_state();
                            yy_pop_state();
                            yy_push_state(X_IGNORE);
                        }
<X_ESCAPE>"00"          { 
                            yy_pop_state();
                            yy_pop_state();
                            yy_push_state(X_IGNORE);
                        }
<X_ESCAPE>"n"           { 
                            strlit << "\n";
                            yy_pop_state();
                        }
<X_ESCAPE>"r"           { 
                            strlit << "\r";
                            yy_pop_state();
                        }
<X_ESCAPE>"t"           { 
                            strlit << "\t";
                            yy_pop_state();
                        }
<X_ESCAPE>"\""          { 
                            strlit << "\"";
                            yy_pop_state();
                        }
<X_ESCAPE>"\\"          { 
                            strlit << "\\";
                            yy_pop_state();
                        }
<X_ESCAPE>[0-9a-fA-F]{1,2}  {
                                strlit << (char)(unsigned char) strtoul(yytext, nullptr, 16);
                                yy_pop_state();
                            }
<X_ESCAPE>.             {
                            strlit << *yytext;
                            yy_pop_state();
                        }
                        
<X_IGNORE>"\\\""        ;
<X_IGNORE>"\\\\"        ;
<X_IGNORE>.|\n          ;


    /* ====================================================================== */
    /* ====[                  7 - EXPRESSION OPERATORS                  ]==== */
    /* ====================================================================== */

">="                    return tGE;
"<="                    return tLE;
"=="                    return tEQ;
"!="                    return tNE;
"||"                    return tOR;
"&&"                    return tAND;

    /* ====================================================================== */
    /* ====[                        Keywords                            ]==== */
    /* ====================================================================== */

"for"                   return tFOR; 
"if"                    return tIF; 
"then"                  return tTHEN;
"elif"                  return tELIF;
"else"                  return tELSE; 
"public"                return tPUBLIC;
"require"               return tREQUIRE;
"sizeof"                return tSIZEOF;
"input"                 return tINPUT;
"nullptr"               return tNULLPTR;
"procedure"             return tPROCEDURE;
"break"                 return tBREAK;
"continue"              return tCONTINUE;
"return"                return tRETURN;
"do"                    return tDO;
"write"                 return tWRITE;
"writeln"               return tWRITELN;

[A-Za-z][A-Za-z0-9_]*   {
                            yylval.s = new std::string(yytext); 
                            return tIDENTIFIER;
                        }

[-()<>=+*/%;,!~{}\[\]?@]    return *yytext;

[ \r\t\n]+              ; /* ignore whitespace */

.                       yyerror("Unknown character");

%%
