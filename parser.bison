/*
Declare token types at the top of the bison file,
causing them to be automatically generated in parser.tab.h
for use by scanner.c.
*/

%token TOKEN_ARRAY
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FALSE
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
%token TOKEN_TRUE
%token TOKEN_VOID
%token TOKEN_WHILE
%token TOKEN_IDENT
%token TOKEN_INTEGER_LITERAL
%token TOKEN_CHAR_LITERAL
%token TOKEN_STRING_LITERAL
%token TOKEN_COMMENT_C
%token TOKEN_COMMENT_CPLUSPLUS
%token TOKEN_OP_LEFTPARENTHESS
%token TOKEN_OP_RIGHTPARENTHESS
%token TOKEN_OP_LEFTBRACKET
%token TOKEN_OP_RIGHTBRACKET
%token TOKEN_OP_INCREMENT
%token TOKEN_OP_DECREMENT
%token TOKEN_OP_NOT
%token TOKEN_OP_POWER
%token TOKEN_OP_MUL
%token TOKEN_OP_DIV
%token TOKEN_OP_MOD
%token TOKEN_OP_ADD
%token TOKEN_OP_SUB
%token TOKEN_OP_LE
%token TOKEN_OP_LT
%token TOKEN_OP_GE
%token TOKEN_OP_GT
%token TOKEN_OP_EQ
%token TOKEN_OP_UNEQ
%token TOKEN_OP_AND
%token TOKEN_OP_OR
%token TOKEN_OP_ASSIGN
%token TOKEN_LEFTCURLY
%token TOKEN_RIGHTCURLY
%token TOKEN_COLON
%token TOKEN_COMMA
%token TOKEN_SEMICOLON
%token TOKEN_ERROR

%union {
	struct decl *decl;
	struct stmt *stmt;
	struct type *type;
	struct param_list *params;
	struct expr *expr;
	char *str;
	int n;
}

%type <decl> decl func_definition external_decl translation_unit program
%type <stmt> print_stmt return_stmt compound_stmt expr_stmt unmatched_stmt matched_stmt stmt stmt_list
%type <type> type func_type
%type <params> param param_list param_list_opt
%type <expr> constant primary_expr postfix_expr increment_expr unary_expr power_expr mul_expr add_expr relational_expr logical_and_expr logical_or_expr assignment_expr expr expr_opt expr_list expr_list_opt initializer initializer_list
%type <n> unary_operator

%token <n> TOKEN_INTEGER_LITERAL 
%token <str> TOKEN_ARRAY TOKEN_BOOLEAN TOKEN_CHAR TOKEN_ELSE TOKEN_FALSE TOKEN_FOR TOKEN_FUNCTION TOKEN_IF TOKEN_INTEGER TOKEN_PRINT TOKEN_RETURN TOKEN_STRING TOKEN_TRUE TOKEN_VOID TOKEN_WHILE TOKEN_IDENT TOKEN_CHAR_LITERAL TOKEN_STRING_LITERAL TOKEN_OP_LEFTPARENTHESS TOKEN_OP_RIGHTPARENTHESS TOKEN_OP_LEFTBRACKET TOKEN_OP_RIGHTBRACKET TOKEN_OP_INCREMENT TOKEN_OP_DECREMENT TOKEN_OP_NOT TOKEN_OP_POWER TOKEN_OP_MUL TOKEN_OP_DIV TOKEN_OP_MOD TOKEN_OP_ADD TOKEN_OP_SUB TOKEN_OP_LE TOKEN_OP_LT TOKEN_OP_GE TOKEN_OP_GT TOKEN_OP_EQ TOKEN_OP_UNEQ TOKEN_OP_AND TOKEN_OP_OR TOKEN_OP_ASSIGN TOKEN_LEFTCURLY TOKEN_RIGHTCURLY TOKEN_COLON TOKEN_COMMA TOKEN_SEMICOLON

%{

#include <stdio.h>
#include "type.h"
#include "expr.h" 
#include "stmt.h" 
#include "decl.h" 

/*
YYSTYPE cannot be used together with %union
#define YYSTYPE char *
*/


/*
Clunky: Manually declare the interface to the scanner generated by flex. 
*/

extern char *yytext;
extern int yylex();
extern int yyerror( char *str );

/*
Clunky: Keep the final result of the parse in a global variable,
so that it can be retrieved by main().
*/

struct decl *program = 0;

%}

%%

/* Here is the grammar: translation_unit is the start symbol. */

/* an empty files is legal in cminor: translation_unit can be empty */

program: translation_unit
		{ program = $1; return 0; }
	;

translation_unit: external_decl translation_unit 
		{ $1->next = $2; $$ = $1; }
	| /* empty */
		{ $$ = 0; }
	;

external_decl: decl
		{ $$ = $1; }
	| func_definition
		{ $$ = $1; }
	;

func_definition: TOKEN_IDENT TOKEN_COLON func_type TOKEN_OP_ASSIGN compound_stmt
		{ fprintf(stdout, "function definition\n\n"); $$ = decl_create($1, $3, 0, $5, 0); }
	;

decl: TOKEN_IDENT TOKEN_COLON type TOKEN_OP_ASSIGN initializer TOKEN_SEMICOLON  /* declaration with initialization */
		{ fprintf(stdout, "declaration with initialziation\n\n"); $$ = decl_create($1, $3, $5, 0, 0); }
	| TOKEN_IDENT TOKEN_COLON type TOKEN_SEMICOLON /* declaration without initialization */
		{ fprintf(stdout, "declaration without initialziation\n\n"); $$ = decl_create($1, $3, 0, 0, 0); }
	| TOKEN_IDENT TOKEN_COLON func_type TOKEN_SEMICOLON /* function prototype */
		{ fprintf(stdout, "function prototype\n\n"); $$ = decl_create($1, $3, 0, 0, 0); }
	;

func_type: TOKEN_FUNCTION type TOKEN_OP_LEFTPARENTHESS param_list_opt TOKEN_OP_RIGHTPARENTHESS
		{ $$ = type_create(TYPE_FUNCTION, $4, 0, $2); }
	;

initializer: expr
		{ $$ = $1; }
	| TOKEN_LEFTCURLY initializer_list TOKEN_RIGHTCURLY
		{ $$ = expr_create(EXPR_LEFTCURLY, 0, $2); }
	;

initializer_list: initializer
		{ $$ = $1; }
	| initializer_list TOKEN_COMMA initializer
		{ $$ = expr_create(EXPR_COMMA, $1, $3); }
	;

param_list_opt: /* empty */
		{ $$ = 0; }
	| param_list
		{ $$ = $1; }
	;

param_list: param
		{ $$ = $1; }
	| param TOKEN_COMMA param_list /* */
		{ $1->next = $3; $$ = $1; }
	;

param: TOKEN_IDENT TOKEN_COLON type 
		{ $$ = param_list_create($1, $3, 0); }

type: TOKEN_INTEGER
		{ $$ = type_create(TYPE_INTEGER, 0, 0, 0); }
	| TOKEN_CHAR
		{ $$ = type_create(TYPE_CHARACTER, 0, 0, 0); }
	| TOKEN_BOOLEAN
		{ $$ = type_create(TYPE_BOOLEAN, 0, 0, 0); }
	| TOKEN_STRING
		{ $$ = type_create(TYPE_STRING, 0, 0, 0); }
	| TOKEN_VOID
		{ $$ = type_create(TYPE_VOID, 0, 0, 0); }
	| TOKEN_ARRAY TOKEN_OP_LEFTBRACKET TOKEN_OP_RIGHTBRACKET type
		{ $$ = type_create(TYPE_ARRAY, 0, 0, $4); }
	| TOKEN_ARRAY TOKEN_OP_LEFTBRACKET logical_or_expr TOKEN_OP_RIGHTBRACKET type
		{ $$ = type_create(TYPE_ARRAY, 0, $3, $5); }
	;

stmt_list: /* empty */ 
		{ $$ = 0; }
	| stmt stmt_list 
		{ $1->next = $2; $$ = $1; }
	;

stmt: matched_stmt
		{ $$ = $1; }
	| unmatched_stmt
		{ $$ = $1; }
	;

matched_stmt: external_decl
		{ $$ = stmt_create(STMT_DECL, $1, 0, 0, 0, 0, 0, 0); }
	| expr_stmt
		{ $$ = $1; }
	| compound_stmt
		{ $$ = $1; }
	| return_stmt
		{ $$ = $1; }
	| print_stmt
		{ $$ = $1; }
	| TOKEN_FOR TOKEN_OP_LEFTPARENTHESS expr_opt TOKEN_SEMICOLON expr_opt TOKEN_SEMICOLON expr_opt TOKEN_OP_RIGHTPARENTHESS matched_stmt
		{ fprintf(stdout, "matched for statement\n\n"); $$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0); }
	| TOKEN_IF TOKEN_OP_LEFTPARENTHESS expr TOKEN_OP_RIGHTPARENTHESS matched_stmt TOKEN_ELSE matched_stmt
		{ fprintf(stdout, "matched if statement\n\n"); $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0); }
	;

unmatched_stmt: TOKEN_IF TOKEN_OP_LEFTPARENTHESS expr TOKEN_OP_RIGHTPARENTHESS stmt
		{ fprintf(stdout, "unmatched if statement\n\n"); $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, 0, 0); }
	| TOKEN_IF TOKEN_OP_LEFTPARENTHESS expr TOKEN_OP_RIGHTPARENTHESS matched_stmt TOKEN_ELSE unmatched_stmt
		{ fprintf(stdout, "unmatched if statement\n\n"); $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0); }
	| TOKEN_FOR TOKEN_OP_LEFTPARENTHESS expr_opt TOKEN_SEMICOLON expr_opt TOKEN_SEMICOLON expr_opt TOKEN_OP_RIGHTPARENTHESS unmatched_stmt
		{ fprintf(stdout, "unmatched for statement\n\n"); $$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0); }
	;

expr_stmt: expr TOKEN_SEMICOLON
		{ fprintf(stdout, "expression statement\n\n"); $$ = stmt_create(STMT_EXPR, 0, 0, $1, 0, 0, 0, 0); }
	;

compound_stmt: TOKEN_LEFTCURLY stmt_list TOKEN_RIGHTCURLY
		{ fprintf(stdout, "compound statement\n\n"); $$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0, 0); }
	;

return_stmt: TOKEN_RETURN expr_opt TOKEN_SEMICOLON
		{ fprintf(stdout, "return statement\n\n"); $$ = stmt_create(STMT_RETURN, 0, 0, $2, 0, 0, 0, 0); }
	;

print_stmt: TOKEN_PRINT expr_list_opt TOKEN_SEMICOLON
		{ fprintf(stdout, "print statement\n\n"); $$ = stmt_create(STMT_PRINT, 0, 0, $2, 0, 0, 0, 0); }
	;

expr_list_opt: /* empty */
		{ $$ = 0; }
	| expr_list
		{ $$ = $1; }
	;

expr_list: expr 
	| expr_list TOKEN_COMMA expr
		{ $$ = expr_create(EXPR_COMMA, $1, $3); }
	;

expr_opt: /* empty */
	| expr
	;

expr: assignment_expr
	;

assignment_expr: logical_or_expr
	| unary_expr TOKEN_OP_ASSIGN assignment_expr
		{ $$ = expr_create(EXPR_ASSIGN, $1, $3); }
	;

logical_or_expr: logical_and_expr
	| logical_or_expr TOKEN_OP_OR logical_and_expr
		{ $$ = expr_create(EXPR_OR, $1, $3); }
	;

logical_and_expr: relational_expr
	| logical_and_expr TOKEN_OP_AND relational_expr
		{ $$ = expr_create(EXPR_AND, $1, $3); }
	;

/* this is different from C. In C, relational ops have higher precedence than equality ops */
relational_expr: add_expr
	| relational_expr TOKEN_OP_LT add_expr
		{ $$ = expr_create(EXPR_LT, $1, $3); }
	| relational_expr TOKEN_OP_LE add_expr
		{ $$ = expr_create(EXPR_LE, $1, $3); }
	| relational_expr TOKEN_OP_GT add_expr
		{ $$ = expr_create(EXPR_GT, $1, $3); }
	| relational_expr TOKEN_OP_GE add_expr
		{ $$ = expr_create(EXPR_GE, $1, $3); }
	| relational_expr TOKEN_OP_EQ add_expr
		{ $$ = expr_create(EXPR_EQ, $1, $3); }
	| relational_expr TOKEN_OP_UNEQ add_expr
		{ $$ = expr_create(EXPR_UNEQ, $1, $3); }
	;

add_expr: mul_expr
	| add_expr TOKEN_OP_ADD mul_expr
		{ $$ = expr_create(EXPR_ADD, $1, $3); }
	| add_expr TOKEN_OP_SUB mul_expr
		{ $$ = expr_create(EXPR_SUB, $1, $3); }
	;

mul_expr: power_expr
	| mul_expr TOKEN_OP_MUL power_expr
		{ $$ = expr_create(EXPR_MUL, $1, $3); }
	| mul_expr TOKEN_OP_DIV power_expr
		{ $$ = expr_create(EXPR_DIV, $1, $3); }
	| mul_expr TOKEN_OP_MOD power_expr
		{ $$ = expr_create(EXPR_MOD, $1, $3); }
	;

power_expr: unary_expr
	| power_expr TOKEN_OP_POWER unary_expr
		{ $$ = expr_create(EXPR_POWER, $1, $3); }
	;

unary_expr: increment_expr
	| unary_operator unary_expr
		{ $$ = expr_create($1, 0, $2); }
	;

unary_operator: TOKEN_OP_SUB
		{ $$ = EXPR_UNARY_NEG; }
	| TOKEN_OP_NOT
		{ $$ = EXPR_NOT; }
	;

increment_expr: postfix_expr
	| increment_expr TOKEN_OP_INCREMENT
		{ $$ = expr_create(EXPR_INCREMENT, $1, 0); }
	| increment_expr TOKEN_OP_DECREMENT
		{ $$ = expr_create(EXPR_DECREMENT, $1, 0); }
	;

postfix_expr: primary_expr
	| postfix_expr TOKEN_OP_LEFTBRACKET expr TOKEN_OP_RIGHTBRACKET /* array subscript */
		{ $$ = expr_create(EXPR_LEFTBRACKET, $1, $3); }
	| postfix_expr TOKEN_OP_LEFTPARENTHESS expr_list_opt TOKEN_OP_RIGHTPARENTHESS /* function call */
		{ $$ = expr_create(EXPR_LEFTPARENTHESS, $1, $3); }
	;

primary_expr: constant
	| TOKEN_IDENT
		{ $$ = expr_create_name($1); }
	| TOKEN_OP_LEFTPARENTHESS expr TOKEN_OP_RIGHTPARENTHESS /* grouping */
		{ $$ = expr_create(EXPR_LEFTPARENTHESS, 0, $2); }
	;

constant: TOKEN_INTEGER_LITERAL
		{ $$ = expr_create_integer_literal($1); }
	| TOKEN_CHAR_LITERAL
		{ $$ = expr_create_character_literal($1); }
	| TOKEN_STRING_LITERAL
		{ $$ = expr_create_string_literal($1); }
	| TOKEN_TRUE
		{ $$ = expr_create_boolean_literal(1); }
	| TOKEN_FALSE
		{ $$ = expr_create_boolean_literal(0); }
	;
%%

/*
This function will be called by bison if the parse should
encounter an error.  In principle, "str" will contain something
useful.  In practice, it often does not.
*/

int yyerror( char *str )
{
	printf("parse error: %s\n",str);
}
