#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "decl.h"
#include "scope.h"

// declare the parser function generated from parser.bison
extern int yyparse();
extern FILE *yyin;
extern struct decl *program;
extern int resolve_error_count;
extern int type_error_count;

int main(int argc, char *argv[]) {
	/* there is no difference between these three options. */
	if(strcmp(argv[1], "-scan") != 0 \
		&& strcmp(argv[1], "-parse") != 0 && strcmp(argv[1], "-print") != 0 \
		&& strcmp(argv[1], "-resolve") \
		&& strcmp(argv[1], "-typecheck") \
		&& strcmp(argv[1], "-codegen")) {
		fprintf(stdout, "The option of %s should be: -scan or -parse or -print or -codegen\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if((yyin = fopen(argv[2], "r")) == NULL) {
		fprintf(stdout, "fopen fails to open %s: %s.\n", argv[2], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(!strcmp(argv[1], "-codegen") && argc != 4) {
		fprintf(stderr, "the format of -codegen option should be: ./cminor -codegen <cminor.file> <output.file>\n");
		exit(EXIT_FAILURE);
	}

	if(yyparse() == 0) {
		if(!strcmp(argv[1], "-scan")) {
			fprintf(stdout, "scan succeeded!\n");
		} else if(!strcmp(argv[1], "-print") || !strcmp(argv[1], "-parse")) {
			decl_print(program, 0);
		} else if(!strcmp(argv[1], "-resolve")) {
			scope_init();
			decl_resolve(program);
			//scope_print();
			if(resolve_error_count > 0) {
				fprintf(stdout, "%d name resolution errors have been found!\n", resolve_error_count);
				exit(EXIT_FAILURE);
			}
		} else if(!strcmp(argv[1], "-typecheck")) {
			scope_init();
			decl_resolve(program);
			//scope_print();
			if(resolve_error_count > 0) {
				fprintf(stdout, "%d name resolution errors have been found!\n", resolve_error_count);
				exit(EXIT_FAILURE);
			}
			decl_typecheck(program);
			if(type_error_count > 0) {
				fprintf(stdout, "%d type errors have been found!\n", type_error_count);
				exit(EXIT_FAILURE);
			}
		} else if(!strcmp(argv[1], "-codegen")) {
			scope_init();
			decl_resolve(program);
			//scope_print();
			if(resolve_error_count > 0) {
				fprintf(stdout, "%d name resolution errors have been found!\n", resolve_error_count);
				exit(EXIT_FAILURE);
			}
			decl_typecheck(program);
			if(type_error_count > 0) {
				fprintf(stdout, "%d type errors have been found!\n", type_error_count);
				exit(EXIT_FAILURE);
			}

			FILE *f;
			if((f = fopen(argv[3], "w")) == NULL) {
				fprintf(stderr, "Fail to open %s: %s.\n", argv[3], strerror(errno));
				exit(EXIT_FAILURE);
			}

			decl_codegen(program, f);

			if(fclose(f)) {
				fprintf(stderr, "Fail to close %s: %s.\n", argv[3], strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	} else {
		exit(EXIT_FAILURE);
	}

	return 0;
}
