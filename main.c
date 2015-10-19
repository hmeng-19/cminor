#include "lex.yy.c"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// declare the parser function generated from parser.bison
extern int yyparse();

int main(int argc, char *argv[]) {
	int i;

//	for(i=0; i<argc; i++)
//		fprintf(stdout, "argc %d: %s\n", i, argv[i]);

	if(strcmp(argv[1], "-scan") != 0 || strcmp(argv[1], "-parse") != 0) {
		fprintf(stderr, "The option of %s should be: -scan or -parse\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if((yyin = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "fopen fails to open %s: %s.\n", argv[2], strerror(errno));
		exit(EXIT_FAILURE);
	}

	yyparse();

/*
	while(1) {
		int t;
		t = yylex(); // t means token, which is returned from the scanner
	
		// here, the scanner reaches the end of the input file	
		if(!t) 
			break;

		if(token_string(t, yytext, yyleng) != 0)
			exit(EXIT_FAILURE);
	}
*/
	return 0;
}
