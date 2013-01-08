#ifndef PARSE_TREE_H
#define PARSE_TREE_H
/*
 * Vector Calculator uses an alternative structure of the parse tree -
 * all the nodes are of different types, but have an "umbrella" type
 * of nodeType. This simplifies the structure and cleanly separates
 * the types.
 *
 */

#include "Defines.h"

/* All the node types are enumerated. */
typedef enum {
    typeNumConstant,
    typeVecConstant,
    typeBool,
    typeId,
    typeOperator
} nodeEnum;

/* constants */
typedef struct {
    nodeEnum type;              /* type of node */
    double number;              /* value of numeric constant */
    vector3* vector;            /* value of vector constant */
} constantNodeType;

/* identifiers */
typedef struct {
    nodeEnum type;              /* type of node */
    char* id;                   /* id string */
    int idType;                 /* the type of the id */
} idNodeType;

/* operators */
typedef struct {
    nodeEnum type;              /* type of node */
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    union nodeTypeTag *op[1];   /* operands (expandable) */
} operatorNodeType;

typedef union nodeTypeTag {
    nodeEnum type;              /* type of node */
    constantNodeType con;       /* constants */
    idNodeType id;              /* identifiers  */
    operatorNodeType opr;       /* operators */
} nodeType;

#endif
