#include <assert.h>
#include "ParseTreeBuilder.h"
#include "SymbolTable.h"
#include "Math.h"

nodeType* constantNum(double value)
{
    nodeType *p;

    /* allocate node */
    if((p = malloc(sizeof(constantNodeType))) == NULL)
        yyerror("Out of memory encountered.");

    assert(p);
    /* copy information */
    p->type = typeNumConstant;
    p->con.number = value;

    /* set the vector to NULL to avoid garbage values */
    p->con.vector = NULL;

    return p;
}

nodeType* constantVec(double x, double y, double z)
{
    nodeType *p;

    /* allocate node */
    if((p = malloc(sizeof(constantNodeType))) == NULL)
        yyerror("Out of memory encountered.");

    assert(p);
    /* copy information */
    p->type = typeVecConstant;
    p->con.vector = newVector(x, y, z);

    return p;
}

nodeType* id(int type, char *id)
{
    nodeType *p;

    /* allocate node */
    if((p = malloc(sizeof(idNodeType))) == NULL)
        yyerror("Out of memory encountered.");

    assert(p);

    /* copy information */
    p->type = typeId;
    p->id.id = id;

    /* if type is NULL, then the id is defined, 
       but the type can not be retrieved for some
       reason.
       */
    if(type == -1)
        p->id.idType = getType(id);
    else
    {
        p->id.idType = type;
        /* if the type is given, we are defining
           the variable, so add it to the symtable
        */
        addId(id, type);
    }

    return p;
}

nodeType* operator(int oper, int nops, ...)
{
    /* argument list pointer - we're using variable arguments list here */
    va_list listPointer;
    nodeType *p;
    size_t size;
    int i;

    /* Allocate the node - we're still trying to catch the out of memory
       error, so need to precalculate the space required for all the
       suboperators. */
    size = sizeof(operatorNodeType) + (nops - 1) * sizeof(nodeType*);
    if((p = malloc(size)) == NULL)
        yyerror("Out of memory encountered.");

    assert(p);
    /* copy the information */
    p->type = typeOperator;
    p->opr.oper = oper;
    p->opr.nops = nops;
    
    /* initialise the argument list to point to the first element */
    va_start(listPointer, nops);
    /* and traverse the list */
    for(i = 0; i < nops; ++i)
        p->opr.op[i] = va_arg(listPointer, nodeType*);
    /* finally, after we're done, clean up the argument list */
    va_end(listPointer);

    return p;

}

void freeNode(nodeType* p)
{
    int i;

    /* check if the node to be free is free already; if yes - return */
    if(!p) return;

    /* if the node is an operator, free all suboperators */
    if(p->type == typeOperator)
    {
        for(i = 0; i < p->opr.nops; ++i)
            freeNode(p->opr.op[i]);
    }

    /* finally, free the node */
    free(p);
}
