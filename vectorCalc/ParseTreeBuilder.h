#include <stdarg.h> /* for variable-length arguments in operators */
#include "ParseTree.h"

nodeType* constantNum(double);

nodeType* constantVec(double, double, double);

nodeType* id(int, char*);

nodeType* operator(int, int, ...);

void freeNode(nodeType*);
