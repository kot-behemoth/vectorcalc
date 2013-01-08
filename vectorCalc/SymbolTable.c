/*
	The implementation of a symbol table for the Vector Calculator
    language.

	Notes on this version: 
	- uses standard C rather than object orientation or the STL.
	- The symbol table is implemented as a singly linked list. 
    - no optimisations for table searching.
	- all function calls result in a new table search for the required
    entry.

	Based largely on code by Allan c. Milne.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* include the interface function prototypes. */
#include "ParseTree.h"
#include "SymbolTable.h"
#include "vectorCalc.tab.h"
#include "Math.h"

/* A single entry in the symbol table. */
struct symbolNode {
	char *name;	                /* the identifier name. */
    int type;                   /* entry's type. */
	double numberVal;	        /* scalar value assigned to it. */
    vector3* vectorVal;         /* vector value assigned to it. */
	struct symbolNode *next;	/* pointer to next entry in the list. */
};
typedef struct symbolNode symbolEntry;

/* internal functions prototypes */
char* getTypeAsString(int);
symbolEntry* findEntry(char*);

/* the Symbol table. */
symbolEntry* symbolTable = NULL;

char* getTypeAsString(int type)
{
    static char *typeNumConstant_s = "Number\0";
    static char *typeVecConstant_s = "Vector\0";
    static char *typeId_s          = "Id\0";
    static char *typeOperator_s    = "Operator\0";

    switch(type)
    {
        case typeNumConstant:   return typeNumConstant_s; break;
        case typeVecConstant:   return typeVecConstant_s; break;
        case typeId:            return typeId_s; break;
        case typeOperator:      return typeOperator_s; break;
        default:                assert(!"Invalid type specified");
    }
}

/* local function to find an entry for the specified id name; returns null
 * if not found. */
symbolEntry* findEntry(char *id) {
	symbolEntry *found = NULL;
	symbolEntry *iterator = symbolTable;
	while(iterator != NULL)
    {
        if(!strcmp(id,iterator->name))
            found = iterator;
		iterator = iterator->next;
	}
	return found;
}

/* returns true/false if there is an entry in the table for the specified
 * id name. */
int isDeclared(char *id) {
	return findEntry(id) != NULL;
}

/* adds a new entry for the specified id name; returns false if already in
 * the table. */
int addId(char *id, int type) {
	symbolEntry *newEntry;
    
	if(isDeclared(id)) return 0;
    
	newEntry = (symbolEntry*) malloc(sizeof(symbolEntry));
	newEntry->name = id;
    switch(type)
    {
        case typeNumConstant: newEntry->numberVal = 0; break;
        case typeVecConstant: newEntry->vectorVal = newVector(0,0,0); break;
    }
    newEntry->type = type;
	newEntry->next = symbolTable;
	symbolTable = newEntry;

#ifdef DEBUG
    printf("Added a new <%s %s> to symtable\n",
            getTypeAsString(type), id);
#endif
	return 1;
}

/* sets the v parameter to the value associated in the table with the
 * specified id name; returns false if no table entry exists. */
int getValue(char *id, int type, void *v) {
	symbolEntry *entry = findEntry(id);
	if(entry == NULL) return 0;
    switch(type)
    {
        case typeNumConstant: *((double*)v) = entry->numberVal; break;
        case typeVecConstant: *((vector3**)v) = entry->vectorVal; break;
    }
	return 1;
}

int getType(char *id)
{
	symbolEntry *found = NULL;
    found = findEntry(id);
    /*assert(found);*/
    if(found == NULL) return 0;
	return found->type;
}

/* sets the value associated with the specified id name to v; returns false
 * if no table entry exists. */
int setValue(char *id, int type, void *v) {
	symbolEntry* entry = findEntry(id);
	if(entry == NULL) return 0;
    switch(type)
    {
        case typeNumConstant:
        {
            double number = *((double*)v);
            entry->numberVal = number;
#ifdef DEBUG
            printf("Set <%s %s> to %0.2f to symtable\n",
                    getTypeAsString(type), id, entry->numberVal);
#endif
            break;
        }
        case typeVecConstant:
        {
            vector3 *vector = (vector3*)v;
            entry->vectorVal = vector;
#ifdef DEBUG
            printf("Set <%s> to {%0.2f, %0.2f, %0.2f} to symtable\n",
                    id, vector->x, vector->y, vector->z);
#endif
            break;
        }
    }
	return 1;
}

/* end of symbol table implementation. */
