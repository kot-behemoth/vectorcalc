/* 
   Various mathematical functions, as well as all the
   linear algebra for the vector calculations.
*/

/* define epsilon for floats comparison */
#define EPSILON 0.0001

int numberCompare(double, double);
int vectorCompare(vector3*, vector3*);

/* Creates a new vector with given dimensions */
vector3* newVector(double, double, double);

/* Given two vectors, add them together and stores the
   result in the third one. */
void vectorAdd(vector3*, vector3*, vector3*);
vector3* vectorAdd_new(vector3*, vector3*);

void vectorSub(vector3*, vector3*, vector3*);
vector3* vectorSub_new(vector3*, vector3*);

vector3* vectorScale_new(vector3*, double);

void vectorNeg(vector3*, vector3*);
vector3* vectorNeg_new(vector3*);

double vectorDot(vector3*, vector3*);

void vectorCross(vector3*, vector3*, vector3*);
vector3* vectorCross_new(vector3*, vector3*);

