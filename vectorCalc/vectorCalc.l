/*
 * Vector Calculator
 */

/* Shorthand notation to simplify complicated expressions. */
digit  [0-9]
letter [a-zA-Z]

%option never-interactive

%{
    #include <stdlib.h>
    #include "Defines.h"
    #include "ParseTree.h"
    #include "vectorCalc.tab.h"

    void yyerror(char*);
    void yylexerror(char);
	
	/* solve the cross-platform compilation problem */
	#define isatty   _isatty
	
    int yylinenum = 1;
%}


%x COMMENT COMMENT_MULTILINE

%%

    /* single-line comments */
"//"                        { BEGIN COMMENT; }
<COMMENT>{
.                           ;
\n                          { BEGIN 0; yylinenum++; }
}

    /* multi-line comments */
"/*"                        { BEGIN COMMENT_MULTILINE; }
<COMMENT_MULTILINE>{
"*/"                        { BEGIN 0; }
.                           ;
\n                          { yylinenum++; }
}

    /* reserved words */
">="                         return GE;
"<="                         return LE;
"=="                         return EQ;
"!="                         return NE;
"><"                         return CROSS;
"."                          return DOT;
"while"                      return WHILE;
"if"                         return IF;
"else"                       return ELSE;
"print"                      return PRINT;
"vector"                     return tVECTOR;
"number"                     return tNUMBER;

	/* identifiers */
{letter}({letter}|{digit})*  {
                                 yylval.id = (char*)malloc(yyleng+1);
                                 strcpy(yylval.id, yytext);
                                 return IDENTIFIER; 
	                         }

	/* integer and real numbers. */
{digit}+("."{digit}+)?	     { 
	                             SSCANF(yytext, "%lf", &yylval.numberVal);
	                             return NUMBER; 
	                         }

    /* operators */
[-+/*()<>=,;{}.]             {
                                 return *yytext;
                             }

    /* skip whitespace */
[ \t]	                     ;

    /* increment the linenumber */
\n			                 { yylinenum++; }

    /* everything else is an error */
.			                 { yylexerror(*yytext); }

%%

int yywrap() {
    return 1;
}

void yylexerror(char ch)
{
	char errmsg[200];
	if(ch == 0)
    {
		SPRINTF(errmsg, 200, "(Line %d) Premature end of prompt string.", yylinenum);
	}
    else
    {
		SPRINTF(errmsg, 200, "(Line %d) Invalid character '%c' found.", yylinenum, ch);
	}
	yyerror(errmsg);
}
