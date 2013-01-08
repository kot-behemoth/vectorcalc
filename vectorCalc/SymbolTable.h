/*
	Definition of the symbol table interface functions for vectorCalc.
	This version is in standard C rather than being object-oriented or
    using the STL.

	Based largely on code by Allan C. Milne.
*/

/* Returns true/false if there is an entry in the table for the specified
 * id name. */
int isDeclared(char *id);

/* Adds a new entry for the specified id name; returns false if already in
 * the table. */
int addId(char *id, int type);

/* Sets the v parameter to the value associated in the table with the
 * specified id name; returns false if no table entry exists. */
int getValue(char *id, int type, void *v);

int getType(char *id);

/* Sets the value associated with the specified id name to v; returns false
 * if no table entry exists. */
int setValue(char *id, int type, void *data);

/* End of symbol table header file. */
