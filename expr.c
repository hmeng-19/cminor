#include "expr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "scope.h"
#include "register.h"
#include "symbol.h"

extern int str_no;
extern struct symbol *cur_func;
extern int ctl_no;

struct expr *expr_create(expr_t kind, struct expr *left, struct expr *right, int line) {
	struct expr *e = (struct expr *)malloc(sizeof(struct expr));

	if(!e) {
		fprintf(stdout, "malloc fails!\n");
		exit(EXIT_FAILURE);
	}

	e->kind = kind;
	e->symbol = 0;
	e->left = left;
	e->right = right;
	e->line = line;
	e->is_global = 0;
	e->reg = -1;
	return e;
}

struct expr *expr_create_name(const char *n, int line) {
	struct expr *e = expr_create(EXPR_IDENT_NAME, 0, 0, line);
	e->name = n;
	return e;
}

struct expr *expr_create_boolean_literal(int c, int line) {
	struct expr *e = expr_create(EXPR_BOOLEAN_LITERAL, 0, 0, line);
	e->literal_value = c;
	return e;
}

struct expr *expr_create_integer_literal(int c, int line) {
	struct expr *e = expr_create(EXPR_INTEGER_LITERAL, 0, 0, line);
	e->literal_value = c;
	return e;
}

struct expr *expr_create_character_literal(const char *str, int line) {
	struct expr *e = expr_create(EXPR_CHARACTER_LITERAL, 0, 0, line);
	e->string_literal = str;
	return e;
}

struct expr *expr_create_string_literal(const char *str, int line) {
	struct expr *e = expr_create(EXPR_STRING_LITERAL, 0, 0, line);
	e->string_literal = str;
	return e;
}

void expr_print(struct expr *e) {
	if(!e) return;
	
	switch(e->kind) {
		case EXPR_LEFTCURLY:
			printf("{");
			expr_print(e->right);
			printf("}");
			break;
		case EXPR_LEFTPARENTHESS:
			expr_print(e->left);
			printf("(");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_LEFTBRACKET:
			expr_print(e->left);
			printf("[");
			expr_print(e->right);
			printf("]");
			break;
		case EXPR_INCREMENT:
			printf("(");
			expr_print(e->left);
			printf("++");
			printf(")");
			break;
		case EXPR_DECREMENT:
			printf("(");
			expr_print(e->left);
			printf("--");
			printf(")");
			break;
		case EXPR_UNARY_NEG:
			printf("(");
			printf("-");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_NOT:
			printf("(");
			printf("!");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_POWER:
			printf("(");
			expr_print(e->left);
			printf("^");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_MUL:
			printf("(");
			expr_print(e->left);
			printf("*");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_DIV:
			printf("(");
			expr_print(e->left);
			printf("/");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_MOD:
			printf("(");
			expr_print(e->left);
			printf("%%");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_ADD:
			printf("(");
			expr_print(e->left);
			printf("+");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_SUB:
			printf("(");
			expr_print(e->left);
			printf("-");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_LE:
			printf("(");
			expr_print(e->left);
			printf("<=");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_LT:
			printf("(");
			expr_print(e->left);
			printf("<");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_GE:
			printf("(");
			expr_print(e->left);
			printf(">=");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_GT:
			printf("(");
			expr_print(e->left);
			printf(">");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_EQ:
			printf("(");
			expr_print(e->left);
			printf("==");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_UNEQ:
			printf("(");
			expr_print(e->left);
			printf("!=");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_AND:
			printf("(");
			expr_print(e->left);
			printf("&&");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_OR:
			printf("(");
			expr_print(e->left);
			printf("||");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_ASSIGN:
			printf("(");
			expr_print(e->left);
			printf("=");
			expr_print(e->right);
			printf(")");
			break;
		case EXPR_COMMA:
			expr_print(e->left);
			printf(",");
			expr_print(e->right);
			break;
		case EXPR_IDENT_NAME:
			printf("%s", e->name);
			break;
		case EXPR_BOOLEAN_LITERAL:
			if(e->literal_value)
				printf("true");
			else
				printf("false");
			break;
		case EXPR_INTEGER_LITERAL:
			printf("%d", e->literal_value);
			break;
		case EXPR_CHARACTER_LITERAL: /*FIXME: need more work, print using the function in token.c */
			printf("%s", e->string_literal);
			break;
		case EXPR_STRING_LITERAL: /*FIXME: need more work, print using the function in token.c */
			printf("%s", e->string_literal);
			break;
	}
	return;
}

void expr_resolve(struct expr *e) {
	if(!e) return;
	
	expr_resolve(e->left);
	expr_resolve(e->right);

	if(e->kind == EXPR_IDENT_NAME) {
		e->symbol = scope_lookup(e->name, e->line, 1);
	}
}

int expr_is_constant(struct expr *e) {
	if(!e) return 1;

	if(e->kind == EXPR_IDENT_NAME) {
		return 0;
	} else {
		return expr_is_constant(e->left) && expr_is_constant(e->right);
	}
}

struct type *expr_typecheck(struct expr *e, int is_array_initializer, int silent_mode) {
	if(!e) return 0;
	
	struct type *left = 0;
	struct type *right = 0;
	switch(e->kind) {
		case EXPR_LEFTCURLY:
			type_free(left);	
			type_free(right);	
			return type_create(TYPE_ARRAY, 0, expr_create_integer_literal(expr_count_item(e->right), e->line), expr_typecheck(e->right, is_array_initializer, silent_mode), e->line, 0);
			break;
		case EXPR_LEFTPARENTHESS:
			if(e->left) {
				//function call
				//check function call arguments and function definition paramters
				expr_func_typecheck(e, silent_mode);
				type_free(left);	
				type_free(right);	
				return scope_lookup(e->left->name, e->left->line, 0)->type->subtype;
			} else {
				//grouping
				type_free(left);	
				type_free(right);	
				return expr_typecheck(e->right, is_array_initializer, silent_mode);
			}
			break;
		case EXPR_LEFTBRACKET:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);

			//the left child should be type array
			if(left->kind != TYPE_ARRAY) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): %s is not an array, and can not be indexed.\n", e->left->line, e->left->name);
					type_error_count += 1;
				}
				type_free(right);
				return left;
			}

			//the right child should be type integer
			if(right->kind != TYPE_INTEGER) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the index (", e->left->line);
					expr_print(e->right);
					printf(") of an array (%s) has wrong type (", e->left->name);
					type_print(right);
					printf("), and should be integer!\n");
					type_error_count += 1;
				}
			}
			type_free(right);
			return left->subtype;
			break;
		case EXPR_INCREMENT:
		case EXPR_DECREMENT:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			if(left->kind != TYPE_INTEGER) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): ++/-- expr only applys to integer types: the type of expr (", e->line);
					expr_print(e->left);
					printf(") is ");
					type_print(left);
					printf("!\n");
					
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_INTEGER, 0, 0, 0, e->line, 0);
			} 

			if(expr_is_constant(e->left)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): lvalue of ++/-- expr (", e->line);
					expr_print(e->left);
					fprintf(stdout, ") should not be constant!\n");
					type_error_count += 1;
				}
			}

			type_free(right);
			return left;
			break;
		case EXPR_UNARY_NEG:
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(right->kind != TYPE_INTEGER) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): unary neg operator expr only applys to integer types: the type of expr (", e->line);
					expr_print(e->right);
					printf(") is ");
					type_print(right);
					printf("!\n");
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_INTEGER, 0, 0, 0, e->line, 0);
			}
			type_free(left);
			return right;
			break;
		case EXPR_NOT:
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(right->kind != TYPE_BOOLEAN) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): not operator expr only applys to boolean types: the type of expr (", e->line);
					expr_print(e->right);
					printf(") is ");
					type_print(right);
					printf("!\n");
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			}
			type_free(left);
			return right;
			break;
		case EXPR_POWER:
		case EXPR_MUL:
		case EXPR_DIV:
		case EXPR_MOD:
		case EXPR_ADD:
		case EXPR_SUB:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(!type_equals(left, right)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the operands of binary arithmetic operator expr (", e->line);
					expr_print(e);
					printf(")mismatch: left operand (");
					expr_print(e->left);
					printf(") type is ");
					type_print(left);
					printf(", right operand (");
					expr_print(e->right);
					printf(") type is ");
					type_print(right);
					printf("!\n");
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_INTEGER, 0, 0, 0, e->line, 0);
			} else {
				if(left->kind != TYPE_INTEGER) {
					if(!silent_mode) {
						fprintf(stdout, "type error (line %d): the operands of binary arithmetic operator expr (", e->line);
						expr_print(e);
						printf(")must be integers: left operand (");
						expr_print(e->left);
						printf(") type is ");
						type_print(left);
						printf(", right operand (");
						expr_print(e->right);
						printf(") type is ");
						type_print(right);
						printf("!\n");
						type_error_count += 1;
					}
					type_free(left);
					type_free(right);
					return type_create(TYPE_INTEGER, 0, 0, 0, e->line, 0);
				}
				type_free(right);
				return left;
			}
			break;
		case EXPR_LE:
		case EXPR_LT:
		case EXPR_GE:
		case EXPR_GT:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(!type_equals(left, right)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the operands of a comparison operator expr (", e->line);
					expr_print(e);
					printf(") mismatch: left operand (");
					expr_print(e->left);
					printf(") type is ");
					type_print(left);
					printf(", right operand (");
					expr_print(e->right);
					printf(") type is ");
					type_print(right);
					printf("!\n");
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			} else {
				if(left->kind != TYPE_INTEGER) {
					if(!silent_mode) {
						fprintf(stdout, "type error (line %d): the operands of a comparison operator expr (", e->line);
						expr_print(e);
						printf(") must be integers: left operand (");
						expr_print(e->left);
						printf(") type is ");
						type_print(left);
						printf(", right operand (");
						expr_print(e->right);
						printf(") type is ");
						type_print(right);
						printf("!\n");
						type_error_count += 1;
					}
					type_free(left);
					type_free(right);
					return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			}
			break;
		case EXPR_EQ:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(!type_equals(left, right)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the operands of the == operator expr (", e->line);
					expr_print(e);
					printf(") mismatch: left operand (");
					expr_print(e->left);
					printf(") type is ");
					type_print(left);
					printf(", right operand (");
					expr_print(e->right);
					printf(") type is ");
					type_print(right);
					printf("!\n");
	
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			} else {
				if(left->kind == TYPE_ARRAY || left->kind == TYPE_FUNCTION) {
					if(!silent_mode) {
						fprintf(stdout, "type error (line %d): the == operator does not apply to array and function types:", e->line);
						printf(" left operand (");
						expr_print(e->left);
						printf(") type is ");
						type_print(left);
						printf(", right operand (");
						expr_print(e->right);
						printf(") type is ");
						type_print(right);
						printf("!\n");
	
						type_error_count += 1;
					}
					type_free(left);
					type_free(right);
					return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			}
			break;
		case EXPR_UNEQ:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(!type_equals(left, right)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the operands of the != operator expr (", e->line);
					expr_print(e);
					printf(") mismatch: left operand (");
					expr_print(e->left);
					printf(") type is ");
					type_print(left);
					printf(", right operand (");
					expr_print(e->right);
					printf(") type is ");
					type_print(right);
					printf("!\n");
	
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			} else {
				if(left->kind == TYPE_ARRAY || left->kind == TYPE_FUNCTION) {
					if(!silent_mode) {
						fprintf(stdout, "type error (line %d): the != operator does not apply to array and function types:", e->line);
						printf(" left operand (");
						expr_print(e->left);
						printf(") type is ");
						type_print(left);
						printf(", right operand (");
						expr_print(e->right);
						printf(") type is ");
						type_print(right);
						printf("!\n");
	
						type_error_count += 1;
					}
					type_free(left);
					type_free(right);
					return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			}
			break;
		case EXPR_AND:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(!type_equals(left, right)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the operands of the && operator mismatch:", e->line);
					printf(" left operand (");
					expr_print(e->left);
					printf(") type is ");
					type_print(left);
					printf(", right operand (");
					expr_print(e->right);
					printf(") type is ");
					type_print(right);
					printf("!\n");
	
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			} else {
				if(left->kind != TYPE_BOOLEAN) {
					if(!silent_mode) {
						fprintf(stdout, "type error (line %d): the && operator only applys to boolean types:", e->line);
						printf(" left operand (");
						expr_print(e->left);
						printf(") type is ");
						type_print(left);
						printf(", right operand (");
						expr_print(e->right);
						printf(") type is ");
						type_print(right);
						printf("!\n");
	
						type_error_count += 1;
					}
					type_free(left);
					type_free(right);
					return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
				}
				type_free(right);
				return left;
			}
			break;
		case EXPR_OR:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(!type_equals(left, right)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the operands of the || operator mismatch:", e->line);
					printf(" left operand (");
					expr_print(e->left);
					printf(") type is ");
					type_print(left);
					printf(", right operand (");
					expr_print(e->right);
					printf(") type is ");
					type_print(right);
					printf("!\n");
	
					type_error_count += 1;
				}
				type_free(left);
				type_free(right);
				return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			} else {
				if(left->kind != TYPE_BOOLEAN) {
					if(!silent_mode) {
						fprintf(stdout, "type error (line %d): the || operator only applys to boolean types:", e->line);
						printf(" left operand (");
						expr_print(e->left);
						printf(") type is ");
						type_print(left);
						printf(", right operand (");
						expr_print(e->right);
						printf(") type is ");
						type_print(right);
						printf("!\n");
	
						type_error_count += 1;
					}
					type_free(left);
					type_free(right);
					return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
				}
				type_free(right);
				return left;
			}
			break;
		case EXPR_ASSIGN:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(!type_equals(left, right)) {
				if(!silent_mode) {
					fprintf(stdout, "type error (line %d): the operands of the = operator mismatch:", e->line);
					printf(" left operand (");
					expr_print(e->left);
					printf(") type is ");
					type_print(left);
					printf(", right operand (");
					expr_print(e->right);
					printf(") type is ");
					type_print(right);
					printf("!\n");
	
					type_error_count += 1;
				}
				type_free(left);
				return right;
			} else {
				if(left->kind == TYPE_FUNCTION) {
					if(!silent_mode) {
						fprintf(stdout, "type error (line %d): the = operator does not apply to function types!\n", e->line);

						type_error_count += 1;
					}
					type_free(left);
					return right;
				}
				type_free(left);
				return right;
			}
			break;
		case EXPR_COMMA:
			left = expr_typecheck(e->left, is_array_initializer, silent_mode);
			right = expr_typecheck(e->right, is_array_initializer, silent_mode);
			if(is_array_initializer) {
				if(!type_equals(left, right)) {
					if(!silent_mode) {
						printf("type error (line %d): the elements of an array intializer ({", e->line);
						expr_print(e);
						printf("}) should have the same type:");
						printf(" (");
						expr_print(e->left);
						printf(") type is ");
						type_print(left);
						printf(", (");
						expr_print(e->right);
						printf(") type is ");
						type_print(right);
						printf("!\n");
	
						type_error_count += 1;
					}
				}
			}
			type_free(right);
			return left;
			break;
		case EXPR_IDENT_NAME:
			type_free(left);
			type_free(right);
			return e->symbol->type;
			break;
		case EXPR_BOOLEAN_LITERAL:
			type_free(left);
			type_free(right);
			return type_create(TYPE_BOOLEAN, 0, 0, 0, e->line, 0);
			break;
		case EXPR_INTEGER_LITERAL:
			type_free(left);
			type_free(right);
			return type_create(TYPE_INTEGER, 0, 0, 0, e->line, 0);
			break;
		case EXPR_CHARACTER_LITERAL: 
			type_free(left);
			type_free(right);
			return type_create(TYPE_CHARACTER, 0, 0, 0, e->line, 0);
			break;
		case EXPR_STRING_LITERAL: 
			type_free(left);
			type_free(right);
			return type_create(TYPE_STRING, 0, 0, 0, e->line, 0);
			break;
	}
	return 0;
}

//check function call arguments and function definition paramters
void expr_func_typecheck(struct expr *e, int silent_mode) {
	//search for the function in the global scope, get its type 
	struct symbol *s = scope_lookup(e->left->name, e->line, 0);
	struct param_list *p = s->type->params;

	//get the argument number of the function call
	int n = expr_count_item(e->right);

	int i = 0;
	struct expr *arg;
	while(p) {
		i += 1;	
		if(i > n) {
			if(!silent_mode) {
				fprintf(stdout, "type error (line %d): function call (", e->line);
				expr_print(e);
				printf(") does not have enough arguments, the correct function type is: ");
				type_print(s->type);
				printf("!\n");
				type_error_count += 1;
			}
			return;
		}
		arg = expr_get_item(e->right, n, i);
		struct type *t = expr_typecheck(arg, 0, silent_mode);
		if(!type_equals(p->type, t)) {
			if(!silent_mode) {
				fprintf(stdout, "type error (line %d): the types of function call arguments (", e->line);
				expr_print(e);
				printf(") do not match the types of function parameters: ");
				type_print(s->type);
				printf("!\n");
	
				type_error_count += 1;
			}
			type_free(t);
			return;
		}
		type_free(t);
		p = p->next;
	}

	if(i < n) {
		if(!silent_mode) {
			fprintf(stdout, "type error (line %d): function call (", e->line);
			expr_print(e);
			printf(") has too much arguments, the correct function type is: ");
			type_print(s->type);
			printf("!\n");

			type_error_count += 1;
		}
		return;
	}

	return;
}

struct expr *expr_get_item(struct expr *e, int n, int i) {
	if(i == n) {
		if(n == 1) {
			return e;
		} else {	
			return e->right;
		}
	}

	int x = n - i;
	while(x > 0) {
		e = e->left;	
		x -= 1; 
	}

	if(i == 1) {
		return e;
	} else {
		return e->right;
	}
}

int expr_count_item(struct expr *e) {
	if(!e) return 0;
	int n = 1;
	while(e->kind == EXPR_COMMA) {
		n += 1;
		e = e->left;
	}
	return n;
}

void expr_print_typecheck(struct expr *e) {
	int n = expr_count_item(e);

	int i;
	struct expr *arg;
	for(i = 1; i <= n; i++) {
		arg = expr_get_item(e, n, i);
		struct type *t = expr_typecheck(arg, 0, 1);
		if(t->kind == TYPE_FUNCTION) {
			fprintf(stdout, "type error (line %d): print_stmt can not print function (", e->line);
			expr_print(arg);
			printf(")!\n");
			type_error_count += 1;
			continue;
		}

		if(t->kind == TYPE_VOID) {
			fprintf(stdout, "type error (line %d): print_stmt can not print void type (", e->line);
			expr_print(arg);
			printf(")!\n");
			type_error_count += 1;
			continue;
		}
		type_free(t);
	}
}

int expr_equals(struct expr *s, struct expr *t) {
	if((!s) && (!t)) {
		return 1;
	} else if(s && t) {
		if((s->kind == EXPR_INTEGER_LITERAL) && (t->kind == EXPR_INTEGER_LITERAL) && (s->literal_value == t->literal_value)) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

void expr_codegen(struct expr *e, FILE *f) {
	if(!e) return;

	if(e->is_global) {
		if(e->left) e->left->is_global = 1;
		if(e->right) e->right->is_global = 1;
	}

	struct type *t;
	switch(e->kind) {
		case EXPR_LEFTCURLY: //array initializer 
			fprintf(stderr, "line %d: cminor does not support array currently!\n", e->line);
			exit(EXIT_FAILURE);
			break;
		case EXPR_LEFTPARENTHESS:
			/* the left child of ( can only be: id; there is no need to do codegen on it */
			if(e->left) { //function call
				expr_funccall_codegen(e->right, f);

				fprintf(f, "\tpushq\t%%r10\n");
				fprintf(f, "\tpushq\t%%r11\n");
				fprintf(f, "\tcall\t%s\n", e->left->name);
				fprintf(f, "\tpopq\t%%r11\n");
				fprintf(f, "\tpopq\t%%r10\n");

				e->reg = register_alloc();
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->reg));
			}
			break;
		case EXPR_LEFTBRACKET: //array subscript
			fprintf(stderr, "line %d: cminor does not support array currently!\n", e->line);
			exit(EXIT_FAILURE);
			break;
		case EXPR_INCREMENT:
			/* left child must be a EXPR_IDENT_NAME, right child is empty */
			expr_codegen(e->left, f);
			if(e->is_global) {
				e->literal_value = e->left->literal_value + 1;
			} else {
				if(e->left->symbol->kind == SYMBOL_PARAM) {
					fprintf(f, "\taddq\t$1, %d(%%rbp)\n", -8 * (e->left->symbol->which + 1));
				} else if(e->left->symbol->kind == SYMBOL_LOCAL) {
					fprintf(f, "\taddq\t$1, %d(%%rbp)\n", -8 * (cur_func->param_count + e->left->symbol->which + 1));
				} else if(e->left->symbol->kind == FUNC_NOT) {
					fprintf(f, "\taddq\t$1, %s(%%rip)\n", e->left->name);
				}
				e->reg = e->left->reg;
			}	
			break;
		case EXPR_DECREMENT:
			/* left child must be a EXPR_IDENT_NAME, right child is empty */
			expr_codegen(e->left, f);
			if(e->is_global) {
				e->literal_value = e->left->literal_value + 1;
			} else {
				if(e->left->symbol->kind == SYMBOL_PARAM) {
					fprintf(f, "\tsubq\t$1, %d(%%rbp)\n", -8 * (e->left->symbol->which + 1));
				} else if(e->left->symbol->kind == SYMBOL_LOCAL) {
					fprintf(f, "\tsubq\t$1, %d(%%rbp)\n", -8 * (cur_func->param_count + e->left->symbol->which + 1));
				} else if(e->left->symbol->kind == FUNC_NOT) {
					fprintf(f, "\tsubq\t$1, %s(%%rip)\n", e->left->name);
				}
				e->reg = e->left->reg;
			}	
			break;
		case EXPR_UNARY_NEG:
			/*left child is empty */
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = -1 * e->right->literal_value;
			} else {
				fprintf(f, "\tmovq\t$0, %%rax\n");
				fprintf(f, "\tsubq\t%%%s, %%rax\n", register_name(e->right->reg));
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
			}
			break;
		case EXPR_NOT:
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = 1 - e->right->literal_value;
			} else {
				fprintf(f, "\tcmpq\t$1, %%%s\n", register_name(e->right->reg));
				fprintf(f, "\tje\t.L%d\n", ctl_no);
				fprintf(f, "\tmovq\t$1, %%%s\n", register_name(e->right->reg));
				fprintf(f, "\tjmp\t.L%d\n", ctl_no+1);
				fprintf(f, ".L%d:\n", ctl_no);
				fprintf(f, "\tmovq\t$0, %%%s\n", register_name(e->right->reg));
				fprintf(f, ".L%d:\n", ctl_no+1);
				ctl_no += 2;
				e->reg = e->right->reg;
			}	
			break;
		case EXPR_POWER:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = (int)pow(e->left->literal_value, e->right->literal_value);
			} else {
				fprintf(f, "\tmovq\t%%%s, %%rdi\n", register_name(e->left->reg));
				fprintf(f, "\tmovq\t%%%s, %%rsi\n", register_name(e->right->reg));
				fprintf(f, "\tpushq\t%%r10\n");
				fprintf(f, "\tpushq\t%%r11\n");
				fprintf(f, "\tcall\tinteger_power\n");
				fprintf(f, "\tpopq\t%%r11\n");
				fprintf(f, "\tpopq\t%%r10\n");
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));

				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_MUL:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = e->left->literal_value * e->right->literal_value;
			} else {
				fprintf(f, "\tmovq\t%%%s, %%rax\n", register_name(e->left->reg));
				fprintf(f, "\timulq\t%%%s\n", register_name(e->right->reg));
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_DIV:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = e->left->literal_value / e->right->literal_value;
			} else {
				fprintf(f, "\tmovq\t%%%s, %%rax\n", register_name(e->left->reg));
				fprintf(f, "\tcqo\n");
				fprintf(f, "\tidivq\t%%%s\n", register_name(e->right->reg));
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_MOD:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = e->left->literal_value - (e->right->literal_value * (e->left->literal_value / e->right->literal_value));
			} else {
				fprintf(f, "\tmovq\t%%%s, %%rax\n", register_name(e->left->reg));
				fprintf(f, "\tcqo\n");
				fprintf(f, "\tidivq\t%%%s\n", register_name(e->right->reg));
				fprintf(f, "\tmovq\t%%rdx, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_ADD:
			/*
			 * the sequence of call expr_codegen on e->left and e->right matters. 
			 * the big lession: if the parser rules is construsted as left-recursive, then the expr_codegen should first be called on the left child.
			 * if expr_codegen(e->right, f) is called first here, we would get an error: there is no free register left! 
			 */
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = e->left->literal_value + e->right->literal_value;
			} else {
				fprintf(f, "\taddq\t%%%s, %%%s\n", register_name(e->left->reg), register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_SUB:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = e->left->literal_value - e->right->literal_value;
			} else {
				fprintf(f, "\tsubq\t%%%s, %%%s\n", register_name(e->right->reg), register_name(e->left->reg));
				e->reg = e->left->reg;
				register_free(e->right->reg);
				e->right->reg = -1;
			}
			break;
		case EXPR_LE:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);

			if(e->is_global) {
				e->literal_value = 0;
				if(e->left->literal_value <= e->right->literal_value) e->literal_value = 1;
			} else {
				fprintf(f, "\tcmpq\t%%%s, %%%s\n", register_name(e->right->reg), register_name(e->left->reg));
				fprintf(f, "\tsetle\t%%al\n");
				fprintf(f, "\tmovzx\t%%al, %%rax\n");
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_LT:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = 0;
				if(e->left->literal_value < e->right->literal_value) e->literal_value = 1;
			} else {
				fprintf(f, "\tcmpq\t%%%s, %%%s\n", register_name(e->right->reg), register_name(e->left->reg));
				fprintf(f, "\tsetl\t%%al\n");
				fprintf(f, "\tmovzx\t%%al, %%rax\n");
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_GE:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = 0;
				if(e->left->literal_value >= e->right->literal_value) e->literal_value = 1;
			} else {
				fprintf(f, "\tcmpq\t%%%s, %%%s\n", register_name(e->right->reg), register_name(e->left->reg));
				fprintf(f, "\tsetge\t%%al\n");
				fprintf(f, "\tmovzx\t%%al, %%rax\n");
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_GT:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = 0;
				if(e->left->literal_value > e->right->literal_value) e->literal_value = 1;
			} else {
				fprintf(f, "\tcmpq\t%%%s, %%%s\n", register_name(e->right->reg), register_name(e->left->reg));
				fprintf(f, "\tsetg\t%%al\n");
				fprintf(f, "\tmovzx\t%%al, %%rax\n");
				fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_EQ:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			t = expr_typecheck(e->left, 0, 1);
			if(e->is_global) {
				e->literal_value = 0;
				if(t->kind == TYPE_STRING) {
					if(!strcmp(e->left->string_literal, e->right->string_literal)) e->literal_value = 1;
				} else {
					if(e->left->literal_value == e->right->literal_value) e->literal_value = 1;
				}
			} else {
				if(t->kind == TYPE_STRING) {
					fprintf(f, "\tmovq\t%%%s, %%rdi\n", register_name(e->left->reg));
					fprintf(f, "\tmovq\t%%%s, %%rsi\n", register_name(e->right->reg));
					fprintf(f, "\tpushq\t%%r10\n");
					fprintf(f, "\tpushq\t%%r11\n");
					fprintf(f, "\tcall\tstrcmp\n");
					fprintf(f, "\tpopq\t%%r11\n");
					fprintf(f, "\tpopq\t%%r10\n");
					fprintf(f, "\tcmpq\t$0, %%rax\n");
					fprintf(f, "\tje\t.L%d\n", ctl_no);
					fprintf(f, "\tmovq\t$0, %%%s\n", register_name(e->right->reg));
					fprintf(f, "\tjmp\t.L%d\n", ctl_no+1);
					fprintf(f, ".L%d:\n", ctl_no);
					fprintf(f, "\tmovq\t$1, %%%s\n", register_name(e->right->reg));
					fprintf(f, ".L%d:\n", ctl_no+1);
					ctl_no += 2;
				} else {
					fprintf(f, "\tcmpq\t%%%s, %%%s\n", register_name(e->right->reg), register_name(e->left->reg));
					fprintf(f, "\tsete\t%%al\n");
					fprintf(f, "\tmovzx\t%%al, %%rax\n");
					fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				}
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			type_free(t);
			break;
		case EXPR_UNEQ:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			t = expr_typecheck(e->left, 0, 1);
			if(e->is_global) {
				e->literal_value = 0;
				if(t->kind == TYPE_STRING) {
					if(strcmp(e->left->string_literal, e->right->string_literal)) e->literal_value = 1;
				} else {
					if(e->left->literal_value != e->right->literal_value) e->literal_value = 1;
				}
			} else {
				if(t->kind == TYPE_STRING) {
					fprintf(f, "\tmovq\t%%%s, %%rdi\n", register_name(e->left->reg));
					fprintf(f, "\tmovq\t%%%s, %%rsi\n", register_name(e->right->reg));
					fprintf(f, "\tpushq\t%%r10\n");
					fprintf(f, "\tpushq\t%%r11\n");
					fprintf(f, "\tcall\tstrcmp\n");
					fprintf(f, "\tpopq\t%%r11\n");
					fprintf(f, "\tpopq\t%%r10\n");
					fprintf(f, "\tcmpq\t$0, %%rax\n");
					fprintf(f, "\tjne\t.L%d\n", ctl_no);
					fprintf(f, "\tmovq\t$0, %%%s\n", register_name(e->right->reg));
					fprintf(f, "\tjmp\t.L%d\n", ctl_no+1);
					fprintf(f, ".L%d:\n", ctl_no);
					fprintf(f, "\tmovq\t$1, %%%s\n", register_name(e->right->reg));
					fprintf(f, ".L%d:\n", ctl_no+1);
					ctl_no += 2;
				} else {
					fprintf(f, "\tcmpq\t%%%s, %%%s\n", register_name(e->right->reg), register_name(e->left->reg));
					fprintf(f, "\tsetne\t%%al\n");
					fprintf(f, "\tmovzx\t%%al, %%rax\n");
					fprintf(f, "\tmovq\t%%rax, %%%s\n", register_name(e->right->reg));
				}
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			type_free(t);
			break;
		case EXPR_AND:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);

			if(e->is_global) {
				e->literal_value = 0;
				if(e->left->literal_value && e->right->literal_value) e->literal_value = 1;
			} else {	
				fprintf(f, "\tcmpq\t$0, %%%s\n", register_name(e->left->reg));
				fprintf(f, "\tje\t.L%d\n", ctl_no);
				fprintf(f, "\tcmpq\t$0, %%%s\n", register_name(e->right->reg));
				fprintf(f, "\tje\t.L%d\n", ctl_no);
				fprintf(f, "\tmovq\t$1, %%%s\n", register_name(e->right->reg));
				fprintf(f, "\tjmp\t.L%d\n", ctl_no+1);
				fprintf(f, ".L%d:\n", ctl_no);
				fprintf(f, "\tmovq\t$0, %%%s\n", register_name(e->right->reg));
				fprintf(f, ".L%d:\n", ctl_no+1);
				ctl_no += 2;
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
			break;
		case EXPR_OR:
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = 0;
				if(e->left->literal_value || e->right->literal_value) e->literal_value = 1;
			} else {
				fprintf(f, "\tcmpq\t$0, %%%s\n", register_name(e->left->reg));
				fprintf(f, "\tjne\t.L%d\n", ctl_no);
				fprintf(f, "\tcmpq\t$0, %%%s\n", register_name(e->right->reg));
				fprintf(f, "\tjne\t.L%d\n", ctl_no);
				fprintf(f, "\tmovq\t$0, %%%s\n", register_name(e->right->reg));
				fprintf(f, "\tjmp\t.L%d\n", ctl_no+1);
				fprintf(f, ".L%d:\n", ctl_no);
				fprintf(f, "\tmovq\t$1, %%%s\n", register_name(e->right->reg));
				fprintf(f, ".L%d:\n", ctl_no+1);
				ctl_no += 2;
				e->reg = e->right->reg;
				register_free(e->left->reg);
				e->left->reg = -1;
			}
	
			break;
		case EXPR_ASSIGN:
			/* the left child of = can be: id (there is no necessity to do codegen on the left child);  the right child of = can be: id, = */
			expr_codegen(e->right, f);
			if(e->is_global) {
				e->literal_value = e->right->literal_value;
				e->string_literal = e->right->string_literal;
			} else {
				if(e->left->symbol->kind == SYMBOL_PARAM) {
					fprintf(f, "\tmovq\t%%%s, %d(%%rbp)\n", register_name(e->right->reg), -8 * (e->left->symbol->which + 1));
				} else if(e->left->symbol->kind == SYMBOL_LOCAL) {
					fprintf(f, "\tmovq\t%%%s, %d(%%rbp)\n", register_name(e->right->reg), -8 * (cur_func->param_count + e->left->symbol->which + 1));
				} else if(e->left->symbol->kind == FUNC_NOT) {
					fprintf(f, "\tmovq\t%%%s, %s(%%rip)\n", register_name(e->right->reg), e->left->name);
				}
				e->reg = e->right->reg;
			}	
			break;
		case EXPR_COMMA:
			break;
		case EXPR_IDENT_NAME:
			e->reg = register_alloc();
			if(e->symbol->kind == SYMBOL_PARAM) {
				fprintf(f, "\tmovq\t%d(%%rbp), %%%s\n", -8 * (e->symbol->which + 1), register_name(e->reg));
			} else if(e->symbol->kind == SYMBOL_LOCAL) {
				fprintf(f, "\tmovq\t%d(%%rbp), %%%s\n", -8 * (cur_func->param_count + e->symbol->which + 1), register_name(e->reg));
			} else if(e->symbol->t == FUNC_NOT) { //global variable
				fprintf(f, "\tmovq\t%s(%%rip), %%%s\n", e->name, register_name(e->reg));
			}
			break;
		case EXPR_BOOLEAN_LITERAL:
			if(!e->is_global) {
				e->reg = register_alloc();
				fprintf(f, "\tmovq\t$%d, %%%s\n", e->literal_value, register_name(e->reg));
			}
			break;
		case EXPR_INTEGER_LITERAL:
			if(!e->is_global) {
				e->reg = register_alloc();
				fprintf(f, "\tmovq\t$%d, %%%s\n", e->literal_value, register_name(e->reg));
			}
			break;
		case EXPR_CHARACTER_LITERAL: 
			e->literal_value = expr_getchar(e->string_literal[1], e->string_literal[2]);
			if(!e->is_global) {
				e->reg = register_alloc();
				fprintf(f, "\tmovq\t$%d, %%%s\n", e->literal_value, register_name(e->reg));
			}
			break;
		case EXPR_STRING_LITERAL: 
			fprintf(f, "\n\t.section\t.rodata\n");
			fprintf(f, ".str%d:\n", str_no);
			expr_codegen_str(e->string_literal, f);
			if(!e->is_global) {
				e->reg = register_alloc();
				fprintf(f, "\n.text\n");
				fprintf(f, "\tlea\t.str%d, %%%s\n", str_no, register_name(e->reg));
			}
			str_no += 1;
			break;
	}
	return;
}

void expr_codegen_str(const char *s, FILE *f) {
	int len = strlen(s);	
	int i = 1;
	fprintf(f, "\t.string \"");
	while(i < len - 1) {
		if(s[i] == '\\') {
			if((s[i+1] == '\\') || (s[i+1] == 't') || (s[i+1] == 'n') || (s[i+1] == 'r')) {
				fprintf(f, "%c%c", s[i], s[i+1]);
			} else if(s[i+1] == '0') {
				fprintf(f, "\"\n");
				if( (i+1) == (len-2)) {
					fprintf(f, "\t.string \"\"\n");
				} else {
					fprintf(f, "\t.string \"");
				}
				
			} else {
				fprintf(f, "%c", s[i+1]);
			}
			i += 2;
		} else {
			fprintf(f, "%c", s[i]);
			i += 1;
		}
	}

	if(!((s[len-3] == '\\') && (s[len-2] == '0'))) {
		fprintf(f, "\"\n");
	}
}

char expr_getchar(const char a, const char b) {
	if(a == '\\') {
		switch(b) {
			case '0':
				return '\0';
			case 't':
				return '\t';
			case 'n':
				return '\n';
			case 'r':
				return '\r';
			default:
				return b;
		}
	} else {
		return a;
	}
}

char *expr_funcarg_register_name(int i) {
	switch(i) {
		case 0:
			return "rdi";
		case 1:
			return "rsi";
		case 2:
			return "rdx";
		case 3:
			return "rcx";
		case 4:
			return "r8";
		case 5:
			return "r9";
		default:
			break;
	}
	return "";
}

void expr_funccall_codegen(struct expr *e, FILE *f) {
	if(!e) return;

	int n = expr_count_item(e);

	if(n > 6) {
		fprintf(stderr, "line %d: the function call has too many arguments (%d arguments)! Cminor supports at most 6 arguments!\n", e->line, n);
		exit(EXIT_FAILURE);
	}

	int i;
	struct expr *arg;
	for(i = 1; i <= n; i++) {
		arg = expr_get_item(e, n, i);
		expr_codegen(arg, f);
		fprintf(f, "\tmovq\t%%%s, %%%s\n", register_name(arg->reg), expr_funcarg_register_name(i-1));
		register_free(arg->reg);
	}
}

int expr_check_lvalue(struct expr *e) {
	if(!e) return 1;

	if((e->kind == EXPR_IDENT_NAME) || (e->kind == EXPR_LEFTPARENTHESS && (!(e->left)))) {
		return expr_check_lvalue(e->left) && expr_check_lvalue(e->right);
	} else {
		return 0;
	}
}

struct expr *expr_get_lvalue(struct expr *e) {
	if(e->kind == EXPR_LEFTPARENTHESS) {
		return expr_get_lvalue(e->right);
	} else {
		return e;
	}	
}




