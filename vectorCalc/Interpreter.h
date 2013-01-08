/*
   Prototype functions for the interpreter.
*/

#include "ParseTree.h"

typedef struct {
    int type;                       /* simple typing */
    union {
        vector3* vector;            /* vector results */
        double number;              /* for numeric results */
        int bool;                   /* for true/false results */
    } data;
} payload;

payload* interpret(nodeType*);
