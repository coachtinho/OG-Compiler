%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
int                             i; /* integer value */
double                          d; /* real value */
std::string                     *s;	/* symbol name or string literal */
cdk::basic_node                 *node; /* node pointer */
cdk::typed_node                 *typed;
cdk::sequence_node              *sequence;
cdk::expression_node            *expression; /* expression nodes */
cdk::lvalue_node                *lvalue;
cdk::basic_type                 *btype;
cdk::primitive_type             *ptype;
cdk::reference_type             *rtype;
og::block_node                  *blck;
std::vector<std::string>        *ids;
};

%token tINT_TYPE tREAL_TYPE tSTRING_TYPE tPOINTER_TYPE tAUTO
%token tPUBLIC tREQUIRE tPROCEDURE tRETURN
%token tSIZEOF tINPUT tNULLPTR tWRITE tWRITELN
%token tFOR tDO tCONTINUE tBREAK;
%token tIF tTHEN tELIF tELSE
%token tGE tLE tEQ tNE tAND tOR
%token <i> tINTEGER
%token <d> tREAL
%token <s> tIDENTIFIER tSTRING

%nonassoc tTHEN
%nonassoc tELSE tELIF
%right '='
%left tOR
%left tAND
%nonassoc '~' 
%left tNE tEQ
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY
%nonassoc '['
%nonassoc '@'

%type <s> string
%type <node> instr ifTypes
%type <typed> idecl decl var func funcDef funcDecl varDecl varDef funcVar cycleVar
%type <sequence> exprs idecls decls file instrs funcParam cycleDecls cycleExprs funcVars cycleVars
%type <expression> expr integer real funcCall
%type <lvalue> lval
%type <btype> dataType
%type <ptype> primitiveType
%type <rtype> referenceType
%type <blck> block
%type <ids> identifier identifiers

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file : decls            { compiler->ast($$ = $1); }
     ;

decls : decl            { $$ = new cdk::sequence_node(LINE, $1); }
      | decls decl      { $$ = new cdk::sequence_node(LINE, $2, $1); }
      ;

idecls : idecl              { $$ = new cdk::sequence_node(LINE, $1); }
       | idecls idecl       { $$ = new cdk::sequence_node(LINE, $2, $1); }
       ;

decl : var ';'          { $$ = $1; }
     | func             { $$ = $1; }
     ;

idecl : var ';'         { $$ = $1; }
      ;

/* DECLARATIONS */

var : varDecl           { $$ = $1; }
    | varDef            { $$ = $1; }
    ;


varDef : tPUBLIC dataType identifier '=' expr       { $$ = new og::var_def_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, $5); }
       | dataType identifier '=' expr               { $$ = new og::var_def_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4); }         
       | tPUBLIC tAUTO identifiers '=' exprs        { $$ = new og::var_def_node(LINE, 1, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$3, new og::tuple_node(LINE, $5)); }
       | tAUTO identifiers '=' exprs                { $$ = new og::var_def_node(LINE, 0, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$2, new og::tuple_node(LINE, $4)); }
       ;

varDecl : tPUBLIC dataType identifier               { $$ = new og::var_decl_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3); } 
        | tREQUIRE dataType identifier              { $$ = new og::var_decl_node(LINE, 2, std::shared_ptr<cdk::basic_type>($2), *$3); }
        | dataType identifier                       { $$ = new og::var_decl_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2); }         
        ;

func : funcDecl     { $$ = $1; }
     | funcDef      { $$ = $1; }
     ;

funcDef : tPUBLIC dataType tIDENTIFIER '(' funcParam ')' block          { $$ = new og::func_def_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, $5, $7); }
        | tPUBLIC tAUTO tIDENTIFIER '(' funcParam ')' block             { $$ = new og::func_def_node(LINE, 1, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$3, $5, $7); }
        | tPUBLIC tPROCEDURE tIDENTIFIER '(' funcParam ')' block        { $$ = new og::func_def_node(LINE, 1, cdk::make_primitive_type(0, cdk::TYPE_VOID), *$3, $5, $7); }
        | dataType tIDENTIFIER '(' funcParam ')' block                  { $$ = new og::func_def_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4, $6); }
        | tAUTO tIDENTIFIER '(' funcParam ')' block                     { $$ = new og::func_def_node(LINE, 0, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$2, $4, $6); }
        | tPROCEDURE tIDENTIFIER '(' funcParam ')' block                { $$ = new og::func_def_node(LINE, 0, cdk::make_primitive_type(0, cdk::TYPE_VOID), *$2, $4, $6); }
        ;

funcDecl : tPUBLIC dataType tIDENTIFIER '(' funcParam ')'       { $$ = new og::func_decl_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, $5); }
         | tPUBLIC tAUTO tIDENTIFIER '(' funcParam ')'          { $$ = new og::func_decl_node(LINE, 1, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$3, $5); }
         | tPUBLIC tPROCEDURE tIDENTIFIER '(' funcParam ')'     { $$ = new og::func_decl_node(LINE, 1, cdk::make_primitive_type(0, cdk::TYPE_VOID),*$3, $5); }
         | tREQUIRE dataType tIDENTIFIER '(' funcParam ')'      { $$ = new og::func_decl_node(LINE, 2, std::shared_ptr<cdk::basic_type>($2), *$3, $5); }
         | tREQUIRE tAUTO tIDENTIFIER '(' funcParam ')'         { $$ = new og::func_decl_node(LINE, 2, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$3, $5); }
         | tREQUIRE tPROCEDURE tIDENTIFIER '(' funcParam ')'    { $$ = new og::func_decl_node(LINE, 2, cdk::make_primitive_type(0, cdk::TYPE_VOID), *$3, $5); }
         | dataType tIDENTIFIER '(' funcParam ')'               { $$ = new og::func_decl_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4); }
         | tAUTO tIDENTIFIER '(' funcParam ')'                  { $$ = new og::func_decl_node(LINE, 0, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$2, $4); }
         | tPROCEDURE tIDENTIFIER '(' funcParam ')'             { $$ = new og::func_decl_node(LINE, 0, cdk::make_primitive_type(0, cdk::TYPE_VOID), *$2, $4); }
         ;

funcParam : funcVars                { $$ = $1; }
          |                         { $$ = new cdk::sequence_node(LINE); }
          ;

funcVars : funcVar                  { $$ = new cdk::sequence_node(LINE, $1); }
         | funcVars ',' funcVar     { $$ = new cdk::sequence_node(LINE, $3, $1); }
         ;

funcVar : dataType identifier       { $$ = new og::var_decl_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2); }

/* INSTRUCTIONS */

block : '{' '}'                 { $$ = new og::block_node(LINE, nullptr, nullptr); }
      | '{' idecls '}'          { $$ = new og::block_node(LINE, $2, nullptr); }
      | '{' instrs '}'          { $$ = new og::block_node(LINE, nullptr, $2); }      
      | '{' idecls instrs '}'   { $$ = new og::block_node(LINE, $2, $3); }  
      ;

instrs : instr              { $$ = new cdk::sequence_node(LINE, $1);}
       | instrs instr       { $$ = new cdk::sequence_node(LINE, $2, $1);}
       ;

instr : expr ';'                { $$ = new og::evaluation_node(LINE, $1); }  
      | tWRITE expr ';'         { $$ = new og::write_node(LINE, $2, false); }
      | tWRITELN expr ';'       { $$ = new og::write_node(LINE, $2, true); }
      | tBREAK                  { $$ = new og::break_node(LINE); }
      | tCONTINUE               { $$ = new og::continue_node(LINE); }
      | tRETURN exprs ';'       { $$ = new og::return_node(LINE, new og::tuple_node(LINE, $2)); }
      | block                   { $$ = $1; }
      | tIF ifTypes             { $$ = $2; }
      | tFOR cycleDecls ';' cycleExprs ';' cycleExprs tDO instr     { $$ = new og::for_node(LINE, $2, $4, $6, $8); }
      ;

ifTypes : expr tTHEN instr                  { $$ = new og::if_node(LINE, $1, $3); }
        | expr tTHEN instr tELIF ifTypes    { $$ = new og::if_else_node(LINE, $1, $3, $5); }
        | expr tTHEN instr tELSE instr      { $$ = new og::if_else_node(LINE, $1, $3, $5); }
        ;

cycleDecls : tAUTO identifiers '=' exprs    { $$ = new cdk::sequence_node(LINE, new og::var_def_node(LINE, 0, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC), *$2, new og::tuple_node(LINE, $4))); }
           | cycleVars                      { $$ = $1; }
           | exprs                          { $$ = $1; }
           |                                { $$ = new cdk::sequence_node(LINE); }
           ;

cycleVars : cycleVar                        { $$ = new cdk::sequence_node(LINE, $1); }
          | cycleVars ',' cycleVar          { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

cycleVar : dataType identifier '=' expr     { $$ = new og::var_def_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4); }         
         | dataType identifier              { $$ = new og::var_decl_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2); }
         ;

cycleExprs : exprs      { $$ = $1; }
           |            { $$ = new cdk::sequence_node(LINE); }
           ;

/* LEFT VALUES */

identifiers : tIDENTIFIER                   { $$ = new std::vector<std::string>(); $$->push_back(*$1); }  
            | identifiers ',' tIDENTIFIER   { $$ = $1; $$->push_back(*$3); }
            ;

identifier : tIDENTIFIER                    { $$ = new std::vector<std::string>(); $$->push_back(*$1); }  
           ;

lval : tIDENTIFIER            { $$ = new cdk::variable_node(LINE, $1); }
     | expr '@' tINTEGER      { $$ = new og::tuple_index_node(LINE, $1, new cdk::integer_node(LINE, $3)); } 
     | expr '[' expr ']'      { $$ = new og::ptr_index_node(LINE, $1, $3); }
     ;

/* EXPRESSIONS */

expr : integer                      { $$ = $1; }
     | real                         { $$ = $1; }
     | string                       { $$ = new cdk::string_node(LINE, $1); }
     | tNULLPTR                     { $$ = new og::nullptr_node(LINE); }
     /* Unary expressions */
     | '-' expr %prec tUNARY        { $$ = new cdk::neg_node(LINE, $2); }
     | '+' expr %prec tUNARY        { $$ = new og::identity_node(LINE, $2); }
     | '~' expr                     { $$ = new cdk::not_node(LINE, $2); }
     | tSIZEOF '(' exprs ')'        { $$ = new og::sizeof_node(LINE, new og::tuple_node(LINE, $3)); }
     /* Arithmetic expressions */
     | expr '+' expr                { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr                { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr                { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr                { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr                { $$ = new cdk::mod_node(LINE, $1, $3); }
     /* Logical expressions */
     | expr tEQ expr	            { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tNE expr	            { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr '<' expr                { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr                { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr	            { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr                { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tAND expr               { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr                { $$ = new cdk::or_node (LINE, $1, $3); }
     /* Left values */
     | lval                         { $$ = new cdk::rvalue_node(LINE, $1); }
     | lval '=' expr                { $$ = new cdk::assignment_node(LINE, $1, $3);}
     | lval '?'                     { $$ = new og::address_of_node(LINE, $1); }
     /* Other expressions */
     | tINPUT                       { $$ = new og::input_node(LINE); }
     | funcCall                     { $$ = $1; }
     | '[' expr ']'                 { $$ = new og::stack_alloc_node(LINE, $2); }
     | '(' expr ')'                 { $$ = $2; }
     ;

exprs : expr	               { $$ = new cdk::sequence_node(LINE, $1); }
      | exprs ',' expr        { $$ = new cdk::sequence_node(LINE, $3, $1); }
	  ;

funcCall : tIDENTIFIER '(' ')'          { $$ = new og::func_call_node(LINE, *$1, new cdk::sequence_node(LINE)); } 
         | tIDENTIFIER '(' exprs ')'    { $$ = new og::func_call_node(LINE, *$1, $3); }
         ;

/* DATA TYPES */

dataType : primitiveType    { $$ = $1; }
         | referenceType    { $$ = $1; }
         ;

primitiveType : tINT_TYPE           { $$ = new cdk::primitive_type(4, cdk::TYPE_INT); }
              | tREAL_TYPE          { $$ = new cdk::primitive_type(8, cdk::TYPE_DOUBLE); }
              | tSTRING_TYPE        { $$ = new cdk::primitive_type(4, cdk::TYPE_STRING); }
              ;

referenceType : tPOINTER_TYPE '<' primitiveType '>'     { $$ = new cdk::reference_type(4, cdk::make_primitive_type($3->size(), $3->name())); delete $3; }
              | tPOINTER_TYPE '<' tAUTO '>'             { $$ = new cdk::reference_type(4, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC)); }
              | tPOINTER_TYPE '<' referenceType '>'     { $$ = new cdk::reference_type(4, cdk::make_reference_type($3->size(), $3->referenced())); delete $3; }
              ;

/* LITERALS */

integer : tINTEGER          { $$ = new cdk::integer_node(LINE, $1); }

real    : tREAL             { $$ = new cdk::double_node(LINE, $1); }

string  : tSTRING           { $$ = $1; }           
        | string tSTRING    { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
        ;

%%
