/*
	Application entry point for the Vector Calculator interpreter.
	Largely based on code by Allan C. Milne, November 2010.

	Usage: vectorCalc <script-file> {<dataset-file>}
	if <dataset-file> is not supplied then stdout is used.

	All user messages are output on stderr; avoids conflicts with possible
    dataset output on stdout.

	This is the entry point for the vectorCalc interpreter - it provides
    the wrapper user interface for calling the Yacc parser and subsequent
    interpreter.

	This interpreter is architectured into separate files reflecting the
    functional components of a compiler.

	Interpreter components:
	 - main.c : command-line application entry point; handles command-line 
       arguments; sets up input/output files and calls yyparse();
	 - vectorCalc.y : Yacc parser; maintains symbol table; creates a parse
       tree representing the input script; interprets parse tree.
	 - vectorCalc.l : Lex scanner; identifies input tokens; returns token
       types with any associated semantic values.
	 - SymbolTable.c/.h : exposes functions for adding and retrieving
       symbol table entries.
	 - ParseTree.h : defines data structure for the parse tree nodes.
	 - ParseTreeBuilder.c/.h : exposes functions for building different
       parse tree branches.
	 - Interpreter.c/.h : exposes interpret() function and internal functions
       to interpret different parse tree branches.
*/

#include <stdlib.h>
#include <stdio.h>

/* number of errors detected - defined in GenVal.y */
extern int errorCount;

/* source of input script - defined in Lex. */
extern FILE *yyin;

 /* output file for generated dataset. */
FILE *dataFile;

#undef DEBUG

/* internal function prototypes. */
int prologue(int, char**);
void epilogue(void);
void openFiles(int, char**);
void closeFiles(int);

/* the Yacc parser prototype. */
int yyparse();

/*--- the vectorCalc application entry point. ---*/
int main(int argc, char *argv[]) {
	if(!prologue(argc, argv))
        exit(-1);

	openFiles(argc, argv);

	yyparse(); 	/*** call Yacc parser and subsequent interpreter. **/

	closeFiles(argc);
	epilogue();
	return 0; 
} /* end of main function. */

/* Display prologue information and check correct usage. */
int prologue(int argc, char *argv[]) {
	fprintf(stderr, "=== Vector Calculator Interpreter. \n");
	fprintf(stderr, "(c) Grigory Goltsov, January 2012. \n\n");
	if(argc<2 || argc>3)
    {
		fprintf(stderr, "invalid usage: vectorCalc <script-file> {<dataset-file>}\n");
		return 0;
	}
    else
    {
		fprintf(stderr, "Interpreting vectorCalc script '%s'. \n", argv[1]);
		fprintf(stderr, "Dataset values output to ");
		if(argc == 3)
			fprintf(stderr, "file '%s'. \n\n", argv[2]);
		else	fprintf(stderr, "standard output. \n\n");
		return 1;
	}
} /* end prologue function. */

/* output closing information. */
void epilogue(void) {
	if(errorCount == 0)
		fprintf(stderr, "... dataset successfully created. \n");
	else if(errorCount == 1)
        fprintf(stderr, "\n%d error found; no dataset created. \n", errorCount);
	else
        fprintf(stderr, "\n%d errors found; no dataset created. \n", errorCount);
} /* end epilogue function. */

/* open and close files depending on command-line arguments. */
void openFiles(int argc, char *argv[]) {
	yyin = fopen(argv[1], "r"); 
	if(argc > 2)
		dataFile = fopen(argv[2], "w");
	else
        dataFile = stdout;
}
void closeFiles(int argc) {
	fclose(yyin);
	if(argc > 2)
        fclose(dataFile);
}

