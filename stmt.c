#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include <stdio.h>
#include <stdlib.h>

struct stmt *stmt_create(stmt_kind_t kind, struct decl *d, struct expr *init_expr, struct expr *e, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next) {
	struct stmt *s = (struct stmt *)malloc(sizeof(struct stmt));
	s->kind = kind;
	s->decl = d;
	s->init_expr = init_expr;
	s->expr = e;
	s->next_expr = next_expr;
	s->body = body;
	s->else_body = else_body;
	s->next = next;
	return s;
}

void stmt_print(struct stmt *s, int indent1, int indent2, int curly_newline) {
	if(!s) return;

	switch(s->kind) {
		case STMT_DECL:
			decl_print(s->decl, indent1);
			break;
		case STMT_EXPR:
			indent_process(indent2);
			expr_print(s->expr);
			printf(";\n");
			break;
		case STMT_PRINT:
			indent_process(indent2);
			printf("print");
			if(s->expr) {
				printf(" ");
				expr_print(s->expr);
			}
			printf(";\n");
			break;
		case STMT_RETURN:
			indent_process(indent2);
			printf("return");
			if(s->expr) {
				printf(" ");
				expr_print(s->expr);
			}
			printf(";\n");
			break;
		case STMT_IF_ELSE:
			indent_process(indent2);
			printf("if(");
			expr_print(s->expr);
			printf(")");
			if(s->body->kind == STMT_BLOCK) {
				printf(" ");
				stmt_print(s->body, 0, indent2, 0);
				printf(" ");
			} else {
				printf("\n");
				stmt_print(s->body, indent1 + 4, indent2 + 4, 1);
			}
			if(s->else_body) {
				if(s->body->kind == STMT_BLOCK) {
					printf("else");
					printf(" ");
					stmt_print(s->else_body, 0, indent2, 1);
				} else {
					indent_process(indent2);
					printf("else");
					printf("\n");
					stmt_print(s->else_body, indent1 + 4, indent2 + 4, 1);
				}
			} else {
				if(s->body->kind == STMT_BLOCK)
					printf("\n");
			}
			break;
		case STMT_FOR:
			indent_process(indent2);
			printf("for(");
			expr_print(s->init_expr);
			printf(";");
			expr_print(s->expr);
			printf(";");
			expr_print(s->next_expr);
			printf(")");
			if(s->body->kind == STMT_BLOCK) {
				printf(" ");
				stmt_print(s->body, 0, indent2, 1);
			} else {
				printf("\n");
				stmt_print(s->body, indent1 + 4, indent2 + 4, 1);
			}
			break;
		case STMT_BLOCK:	
			indent_process(indent1);
			printf("{\n");
			stmt_print(s->body, indent2 + 4, indent2 + 4, 1);
			indent_process(indent2);
			printf("}");
			if(curly_newline) 
				printf("\n");
			break;
	}	
	stmt_print(s->next, indent2, indent2, 1);
	return;
}

void indent_process(int indent) {
	/* indent process */
	if(indent > 0) {
		char *spaces = (char *)malloc(indent * sizeof(char));
		int i;
		for(i = 0; i < indent; i++) {
			spaces[i] = ' ';
		}
		printf("%s", spaces);
	}
}