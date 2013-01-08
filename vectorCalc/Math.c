/*
   Mathematics helper implementation.
*/

#include <assert.h>
#include "Defines.h"
#include "Math.h"

int numberCompare(double n1, double n2)
{
    double delta = 999999;

    delta = n2 - n1;
    return delta < EPSILON;
}

int vectorCompare(vector3* v1, vector3* v2)
{
    return numberCompare(v1->x, v2->x) &&
           numberCompare(v1->y, v2->y) &&
           numberCompare(v1->z, v2->z);
}

vector3* newVector(double x, double y, double z)
{
    vector3 *vector = NULL;

    /* safely allocate a vector */
    if((vector = (vector3*)malloc(sizeof(vector3))) == NULL)
        yyerror("Out of memory encountered when creating a vector.");

    assert(vector);
    vector->x = x;
    vector->y = y;
    vector->z = z;
    
    assert(vector);

    return vector;
}

void vectorAdd(vector3* v1, vector3* v2, vector3* result)
{
    assert(result);
    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->z = v1->z + v2->z;
}

vector3* vectorAdd_new(vector3* v1, vector3* v2)
{
    vector3* result = newVector(0,0,0);
    assert(result);
    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->z = v1->z + v2->z;

    return result;
}

void vectorSub(vector3* v1, vector3* v2, vector3* result)
{
    assert(result);
    result->x = v1->x - v2->x;
    result->y = v1->y - v2->y;
    result->z = v1->z - v2->z;
}

vector3* vectorSub_new(vector3* v1, vector3* v2)
{
    vector3* result = newVector(0,0,0);
    assert(result);
    result->x = v1->x - v2->x;
    result->y = v1->y - v2->y;
    result->z = v1->z - v2->z;

    return result;
}

vector3* vectorScale_new(vector3* v, double s)
{
    return newVector(v->x * s, v->y * s, v->z *s);
}

void vectorNeg(vector3* v, vector3* result)
{
    assert(result);
    result->x = -v->x;
    result->y = -v->y;
    result->z = -v->z;
}

vector3* vectorNeg_new(vector3* v)
{
    vector3* result = newVector(0,0,0);
    assert(result);
    result->x = -v->x;
    result->y = -v->y;
    result->z = -v->z;

    return result;
}

double vectorDot(vector3* v1, vector3* v2)
{
    return v1->x * v2->x + 
           v1->y * v2->y +
           v1->z * v2->z;
}

void vectorCross(vector3* v1, vector3* v2, vector3* result)
{
    assert(result);
    result->x = v1->y * v2->z - v1->z * v2->y;
    result->y = v1->z * v2->x - v1->x * v2->z;
    result->z = v1->x * v2->y - v1->y * v2->x;
}

vector3* vectorCross_new(vector3* v1, vector3* v2)
{
    return newVector(v1->y * v2->z - v1->z * v2->y,
                     v1->z * v2->x - v1->x * v2->z,
                     v1->x * v2->y - v1->y * v2->x);
}

