#ifndef ILP_H
#define ILP_H

#include <ilcplex/ilocplex.h>
#include "Graph.h"
#include "chrono"

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> IloNumVarArrayArray;

Path solveILP(Graph* g);


#endif
