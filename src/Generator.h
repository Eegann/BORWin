#ifndef GENERATOR_H
#define GENERATOR_H

#include "Graph.h"
#include "map"


Graph makeGraph(int numberNodes, double density, int numberLevels, double maxValue, double maxResource);

void makeNewArc(Graph* g, std::string parentID, std::string childrenID, double maxValue, double maxResource);

double makeRandomDouble(double minValue, double maxValue);

void makeResourceWindows(Graph* g);

#endif