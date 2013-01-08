/*
   The interpreter's implementation.
*/

/* order of include is important! */
#include <stdio.h>
#include <assert.h>
#include "Interpreter.h"
#include "SymbolTable.h"
#include "vectorCalc.tab.h"
#include "Math.h"

/* functions prototypes */
payload* newResult(int);
vector3* newVector(double x, double y, double z);

/* The output file defined in MainApp.c */
extern FILE *dataFile;

payload* newResult(int type)
{
    payload *p;

    /* safely allocate space */
    if((p = (payload*)malloc(sizeof(payload))) == NULL)
        yyerror("Out of memory encountered.");

    assert(p);
    p->type = type;

    /* set invalid values to avoid any garbage results */
    p->data.vector = NULL;
    p->data.number = -1;
    p->data.bool = -1;

    return p;
}

payload* interpret(nodeType* p)
{
    /* if we're given NULL - return instantly */
    if(!p) return 0;

    switch(p->type)
    {
        case typeNumConstant:
        {
            payload *res = newResult(typeNumConstant);
            res->data.number = p->con.number;
            return res;
        }

        case typeVecConstant:
        {
            payload *res = newResult(typeVecConstant);
            res->data.vector = p->con.vector;
            return res;
        }

        case typeId:
        {
            /* get the type of the variable */
            int type = p->id.idType;
            /* create the appropriate result container */
            payload *res = newResult(type);
            
            switch(type)
            {
                case typeVecConstant:
                    getValue(p->id.id, type, &(res->data.vector));
                    break;
                case typeNumConstant:
                    getValue(p->id.id, type, &(res->data.number));
                    break;
            }

            return res;
        }

        case typeOperator:
        {
                switch(p->opr.oper)
                {
                    case WHILE:
                    {
                        /* evaluate the operands */
                        payload *condition = interpret(p->opr.op[0]);
                        /* all control statements return 0 */
                        payload *result = newResult(typeBool);
                        /* evaluate the operands */
                        if(condition->type != typeBool)
                            yyerror("WHILE failed, the condition is not of type BOOL.");

                        /* while the condition holds, */
                        while(condition->data.bool)
                        {
                            /* re-evaluate the condition */
                            condition = interpret(p->opr.op[0]);
                            /* evaluate the statement */
                            interpret(p->opr.op[1]);
                        }
                        
                        result->data.bool = 0;
                        return result;
                    }

                    case IF:
                    {
                        /* evaluate the operands */
                        payload *condition = interpret(p->opr.op[0]);
                        /* all control statements return 0 */
                        payload *result = newResult(typeBool);

                        /* evaluate the operands */
                        if(condition->type != typeBool)
                            yyerror("IF failed, the condition is not of type BOOL.");

                        if(condition->data.bool)
                            interpret(p->opr.op[1]);
                        /* this can be an if-else */
                        else if(p->opr.nops > 2)
                            interpret(p->opr.op[2]);

                        result->data.bool = 0;
                        return result;
                    }

                    case PRINT:
                    {
                        /* evaluate the operands */
                        payload *res;
						payload *toPrint;
						
						res = newResult(typeBool);

                        if(p->opr.op[0]->type == typeId)
                            fprintf(dataFile, "%s = ", p->opr.op[0]->id.id);

						toPrint = interpret(p->opr.op[0]);

                        switch(toPrint->type)
                        {
                            case typeVecConstant:
                                res->data.bool = fprintf(dataFile, "{%0.2f, %0.2f, %0.2f}\n",
                                                         toPrint->data.vector->x,
                                                         toPrint->data.vector->y,
                                                         toPrint->data.vector->z);
                            break;
                            case typeNumConstant:
                                res->data.bool = fprintf(dataFile, "%0.2f\n", toPrint->data.number);
                            break;
                            default:
                                yyerror("Wrong argument for printing.");
                        }
                        assert(res);
                        return res;
                    }
                        
                    case ';':
                    {
                        interpret(p->opr.op[0]);
                        return interpret(p->opr.op[1]);
                    }

                    case '=':
                    {
                        /* all control statements return 0 */
                        payload *result = newResult(typeBool);
                        nodeType *lhs = p->opr.op[0];
                        payload *rhs = interpret(p->opr.op[1]);
                        
                        assert(rhs);

                        /* Very simple type-checking */
                        if(isDeclared(lhs->id.id) && lhs->id.idType != rhs->type)
                            yyerror("ERROR: Incompatible types to assign.");
                        
                        switch(rhs->type)
                        {
                            case typeVecConstant:
                                result->data.bool = setValue(p->opr.op[0]->id.id,
                                                             rhs->type, rhs->data.vector);
                                break;
                            case typeNumConstant:
                                result->data.bool = setValue(p->opr.op[0]->id.id,
                                                             rhs->type, &(rhs->data.number));
                                break;
                        }
                        return result;
                    }

                    case UMINUS:
                    {
                        /* evaluate the operand */
                        payload *operand = interpret(p->opr.op[0]);
                        payload *result = NULL;
                        switch(operand->type)
                        {
                            case typeVecConstant:
                                result = newResult(typeVecConstant);
                                result->data.vector = vectorNeg_new(operand->data.vector);
                                break;
                            case typeNumConstant:
                                result = newResult(typeNumConstant);
                                result->data.number = -operand->data.number;
                                break;
                            default:
                                yyerror("Wrong argument for negation.");

                        }
                        assert(result);
                        return result;
                    }

                    case '+':
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        /* test if the types match */
                        if(op1->type != op2->type)
                            yyerror("Incompatible types: vector and a scalar.");
                        
                        /* decide if we're performing vector or scalar addition */
                        switch(op1->type)
                        {
                            case typeVecConstant:
                                /* set VECTOR as the return type */
                                result = newResult(typeVecConstant);
                                result->data.vector = vectorAdd_new(op1->data.vector,
                                                                    op2->data.vector);
                                break;
                            case typeNumConstant:
                                if(op2->type == typeVecConstant)
                                    yyerror("Incompatible types: vector and a scalar.");

                                /* set typeNumConstant as the return type */
                                result = newResult(typeNumConstant);
                                /* do the actual calculations */
                                result->data.number = op1->data.number + op2->data.number;
                                break;
                            default:
                                yyerror("Incompatible types: vector and a scalar.");
                        }

                        assert(result);
                        return result;
                    }

                    case '-':
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        /* test if the types match */
                        if(op1->type != op2->type)
                            yyerror("Incompatible types: vector and a scalar.");
                        
                        /* decide if we're performing vector or scalar subtraction */
                        switch(op1->type)
                        {
                            case typeVecConstant:
                                /* set VECTOR as the return type */
                                result = newResult(typeVecConstant);
                                result->data.vector = vectorSub_new(op1->data.vector,
                                                                    op2->data.vector);
                                break;
                            case typeNumConstant:
                                /* set typeNumConstant as the return type */
                                result = newResult(typeNumConstant);
                                /* do the actual calculations */
                                result->data.number = op1->data.number - op2->data.number;
                                break;
                            default:
                                yyerror("Incompatible types: vector and a scalar.");
                        }

                        assert(result);
                        return result;
                    }

                    case '*':
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        /* decide if we're performing vector or scalar multiplication */
                        switch(op1->type)
                        {
                            case typeNumConstant:
                                switch(op2->type)
                                {
                                    case typeNumConstant:
                                        /* set typeNumConstant as the return type */
                                        result = newResult(typeNumConstant);
                                        /* do the actual calculations */
                                        result->data.number = op1->data.number * op2->data.number;
                                        break;
                                    case typeVecConstant:
                                        /* set typeVecConstant as the return type */
                                        result = newResult(typeVecConstant);
                                        result->data.vector = vectorScale_new(op2->data.vector,
                                                                              op1->data.number);
                                        break;
                                }
                            case typeVecConstant:
                                switch(op2->type)
                                {
                                    case typeNumConstant:
                                        /* set typeNumConstant as the return type */
                                        result = newResult(typeVecConstant);
                                        /* do the actual calculations */
                                        result->data.vector = vectorScale_new(op1->data.vector,
                                                                              op2->data.number);
                                        break;
                                    case typeVecConstant:
                                        yyerror("ERROR: vector multiplication is undefined.");
                                        break;
                                }
                        }

                        assert(result);
                        return result;
                    }

                    case '/':
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        /* decide if we're performing vector or scalar division */
                        switch(op1->type)
                        {
                            case typeNumConstant:
                                switch(op2->type)
                                {
                                    case typeNumConstant:
                                        /* set typeNumConstant as the return type */
                                        result = newResult(typeNumConstant);
                                        /* do the actual calculations */
                                        result->data.number = op1->data.number * op2->data.number;
                                        break;
                                    case typeVecConstant:
                                        /* set typeVecConstant as the return type */
                                        result = newResult(typeVecConstant);
                                        result->data.vector = vectorScale_new(op2->data.vector,
                                                                              1/op1->data.number);
                                        break;
                                }
                            case typeVecConstant:
                                switch(op2->type)
                                {
                                    case typeNumConstant:
                                        /* set typeNumConstant as the return type */
                                        result = newResult(typeVecConstant);
                                        /* do the actual calculations */
                                        result->data.vector = vectorScale_new(op1->data.vector,
                                                                              1/op2->data.number);
                                        break;
                                    case typeVecConstant:
                                        yyerror("ERROR: vector multiplication is undefined.");
                                        break;
                                }
                        }
                        assert(result);
                        return result;
                    }

                    case CROSS:
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        /* cross product is only defined for vectors */
                        if(op1->type == typeNumConstant && op2->type == typeNumConstant)
                            yyerror("Incompatible types: vector and a scalar.");

                        /* set typeNumConstant as the return type */
                        result = newResult(typeVecConstant);
                        /* do the actual calculations */
                        result->data.vector = vectorCross_new(op1->data.vector,
                                                              op2->data.vector);

                        assert(result);
                        return result;
                    }

                    case DOT:
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        /* cross product is only defined for vectors */
                        if(op1->type == typeNumConstant && op2->type == typeNumConstant)
                            yyerror("Incompatible types: vector and a scalar.");

                        /* set typeNumConstant as the return type */
                        result = newResult(typeNumConstant);
                        /* do the actual calculations */
                        result->data.number = vectorDot(op1->data.vector,
                                                        op2->data.vector);

                        assert(result);
                        return result;
                    }

                    case '<':
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        if(op1->type == typeNumConstant)
                        {
                            if(op2->type == typeVecConstant)
                                yyerror("Incompatible types: vector and a scalar.");

                            /* set typeBool as the return type */
                            result = newResult(typeBool);
                            /* do the actual calculations */
                            result->data.bool = op1->data.number < op2->data.number;
                        }
                        else
                            yyerror("Incompatible types: operation can't be performed on vectors.");

                        assert(result);
                        return result;
                    }

                    case '>':
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        if(op1->type == typeNumConstant)
                        {
                            if(op2->type == typeVecConstant)
                                yyerror("Incompatible types: vector and a scalar.");

                            /* set typeBool as the return type */
                            result = newResult(typeBool);
                            /* do the actual calculations */
                            result->data.bool = op1->data.number > op2->data.number;
                        }
                        else
                            yyerror("Incompatible types: operation can't be performed on vectors.");

                        assert(result);
                        return result;
                    }

                    case GE:
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        if(op1->type == typeNumConstant)
                        {
                            if(op2->type == typeVecConstant)
                                yyerror("Incompatible types: vector and a scalar.");

                            /* set typeBool as the return type */
                            result = newResult(typeBool);
                            /* do the actual calculations */
                            result->data.bool = op1->data.number >= op2->data.number;
                        }
                        else
                            yyerror("Incompatible types: operation can't be performed on vectors.");

                        assert(result);
                        return result;
                    }

                    case LE:
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;
                        
                        if(op1->type == typeNumConstant)
                        {
                            if(op2->type == typeVecConstant)
                                yyerror("Incompatible types: vector and a scalar.");

                            /* set typeBool as the return type */
                            result = newResult(typeBool);
                            /* do the actual calculations */
                            result->data.bool = op1->data.number <= op2->data.number;
                        }
                        else
                            yyerror("Incompatible types: operation can't be performed on vectors.");

                        assert(result);
                        return result;
                    }

                    case NE:
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;

                        /* test if the types match */
                        if(op1->type != op2->type)
                            yyerror("Incompatible types: vector and a scalar.");
                        
                        /* decide if we're performing vector or scalar addition */
                        switch(op1->type)
                        {
                            case typeVecConstant:
                                /* set typeBool as the return type */
                                result = newResult(typeBool);
                                /* do the actual calculations */
                                if(vectorCompare(op1->data.vector, op2->data.vector))
                                    result->data.bool = 0;
                                else
                                    result->data.bool = 1;
                                break;
                            case typeNumConstant:
                                /* set typeNumConstant as the return type */
                                result = newResult(typeBool);
                                /* do the actual calculations */
                                if(numberCompare(op1->data.number, op2->data.number))
                                    result->data.bool = 0;
                                else
                                    result->data.bool = 1;
                                break;
                            default:
                                yyerror("Incompatible types: vector and a scalar.");
                        }

                        assert(result);
                        return result;
                    }

                    case EQ:
                    {
                        /* evaluate the operands */
                        payload *op1 = interpret(p->opr.op[0]);
                        payload *op2 = interpret(p->opr.op[1]);
                        payload *result = NULL;

                        /* test if the types match */
                        if(op1->type != op2->type)
                            yyerror("Incompatible types: vector and a scalar.");
                        
                        /* decide if we're performing vector or scalar addition */
                        switch(op1->type)
                        {
                            case typeVecConstant:
                                /* set typeBool as the return type */
                                result = newResult(typeBool);
                                /* do the actual calculations */
                                result->data.bool = vectorCompare(op1->data.vector, op2->data.vector);
                                break;
                            case typeNumConstant:
                                /* set typeNumConstant as the return type */
                                result = newResult(typeBool);
                                /* do the actual calculations */
                                result->data.bool = numberCompare(op1->data.number, op2->data.number);
                                break;
                            default:
                                yyerror("Incompatible types: vector and a scalar.");
                        }

                        assert(result);
                        return result;
                }
            }
        }
    }
    assert(!"Interpretting didn't match any rules");
}
