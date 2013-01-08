/*
   Vector Calculator Interpreter
   This is quite a complex vector calculator, which lets the user
   try out various calculations with 2- and 3-dimensional vectors.
   The language incorporates controls constructs such as if-else and 
   while. The output can be specified to be both stdout and a file.
  
   The implementation includes
       - a symbol table that allows multi-character variable names;
       - a parse tree that is populated by the interpreter and is then
         traversed in Depth-First Search manner - the same way it is
         populated during parsing;
       - 2-character operators, such as <=, ==, !=;
       - variables are possible to define on-the-fly, to emphasise the
         scripting nature of this little language (that is mostly
         intended for quick computations and demonstrations);
       - memory managment - parse tree nodes actually get free'd when
         evaluated (via freeNode());
       - check for "Out of memory" errors.
 */

%{  
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h> /* for variable-length arguments in operators */
    #include "Interpreter.h"
    #include "Defines.h"
    #include "ParseTree.h"
    #include "SymbolTable.h"

    extern int yylinenum;

    /* prototypes */
    nodeType* operator(int oper, int nops, ...);
    nodeType* id(int i, char*);
    nodeType* constantNum(double);
    nodeType* constantVec(double, double, double);
    void freeNode(nodeType*);

    char* getTypeAsString(int);

    /* count the number of errors detected */
    int errorCount = 0;

    /* report semantic errors */
    void semanticError(int, char*);
    void yyerror(char *);

    int yylex();
%}

%union {
    double numberVal;       /* number value */
    char* id;               /* variable name */
    int type;               /* variable type */
    nodeType* nodePtr;      /* node pointer */
};

%token <numberVal> NUMBER
%token <id> IDENTIFIER

%token WHILE IF
%token PRINT

%token tVECTOR tNUMBER

%token GE LE EQ NE
%token CROSS DOT
/* here the implied non-associativity is used to solve the
   shift-reduce conflict in if-else ambiguity. It essentially gives
   an IF-ELSE statement a higher precedence than a simple IF
   statement. */
%nonassoc IFX
%nonassoc ELSE

%left '<' '>' GE LE EQ NE 
%left '+' '-'
%left '*' '/'
%left CROSS DOT
/* a unary minus must have higher precedence than the other operators */
%nonassoc UMINUS

%type <nodePtr> statement
%type <nodePtr> statementList
%type <nodePtr> expression
%type <type>    type

%expect 18

%%

script:
          function              { exit(0); }
        ;

function:
          function statement    { if(errorCount == 0)
                                  {
                                      interpret($2);
                                      freeNode($2);
                                  }
                                }
        |
        ;

statement:
          ';'                   { $$ = operator(';', 2, NULL, NULL); }
        | expression ';'        { $$ = $1; }
        | type IDENTIFIER '=' expression ';'
                                { $$ = operator('=', 2, id($1, $2), $4); }
        | PRINT expression ';'  { $$ = operator(PRINT, 1, $2); }
        | WHILE '(' expression ')' statement
                                { $$ = operator(WHILE, 2, $3, $5); }
        | IF '(' expression ')' statement %prec IFX
                                { $$ = operator(IF, 2, $3, $5); }
        | IF '(' expression ')' statement ELSE statement
                                { $$ = operator(IF, 3, $3, $5, $7); }
        | '{' statementList '}' { $$ = $2; }
        ;

statementList:
          statement             { $$ = $1; }
        | statementList statement
                                { $$ = operator(';', 2, $1, $2); }  
        ;

type:
        | tVECTOR               { $$ = typeVecConstant; }
        | tNUMBER               { $$ = typeNumConstant; }
        ;

expression:
          NUMBER                { $$ = constantNum($1); }
        | IDENTIFIER            { if(!isDeclared($1))
		                              semanticError(2, $1);
	                              $$ = id(-1, $1); }
        | IDENTIFIER '=' expression
                                { $$ = operator('=', 2, id(-1, $1), $3); }
        | '{' NUMBER ',' NUMBER ',' NUMBER '}'
                                { $$ = constantVec($2, $4, $6); }
        | '-' expression %prec UMINUS
                                { $$ = operator(UMINUS, 1, $2); }
        | expression '+' expression
                                { $$ = operator('+', 2, $1, $3); }
        | expression '-' expression
                                { $$ = operator('-', 2, $1, $3); }
        | expression '*' expression
                                { $$ = operator('*', 2, $1, $3); }
        | expression '/' expression
                                { $$ = operator('/', 2, $1, $3); }
        | expression CROSS expression
                                { $$ = operator(CROSS, 2, $1, $3); }
        | expression DOT expression
                                { $$ = operator(DOT, 2, $1, $3); }
        | expression '<' expression
                                { $$ = operator('<', 2, $1, $3); }
        | expression '>' expression
                                { $$ = operator('>', 2, $1, $3); }
        | expression GE expression
                                { $$ = operator(GE, 2, $1, $3); }
        | expression LE expression
                                { $$ = operator(LE, 2, $1, $3); }
        | expression NE expression
                                { $$ = operator(NE, 2, $1, $3); }
        | expression EQ expression
                                { $$ = operator(EQ, 2, $1, $3); }
        | '(' expression ')'    { $$ = $2; }
        ;

%%

void yyerror(char *msg) {
    fprintf(stderr, "line %d: %s\n", yylinenum, msg);
	errorCount++;
}

/* generate semantic error messages. */
void semanticError(int errType, char *info) {
	char errmsg[200];
	switch(errType)
    {
        case alreadyDeclared:
            SPRINTF(errmsg, 200, "Semantic error 1: variable '%s' is already declared.", info); break;
        case notDeclared:
            SPRINTF(errmsg, 200, "Semantic error 2: variable '%s' is not declared.", info); break;
	}
	yyerror(errmsg);
}
