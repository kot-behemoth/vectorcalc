#ifndef DEFINES_H
#define DEFINES_H

/*
 * Defines for some cross-platform compatibilities
 *
 */

#ifdef _MSC_VER // maybe check the specific version, too...
    #define SSCANF sscanf_s
    #define SPRINTF sprintf_s
#else
    #define SSCANF sscanf
    #define SPRINTF snprintf
#endif

#include <stdlib.h>

/* error funciton prototype */
void yyerror(char *);

/* all the semantic errors are enumerated */
typedef enum { notDeclared, alreadyDeclared } serrorEnum;

typedef struct {
    double x, y, z;
} vector3;

#endif
