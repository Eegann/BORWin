#ifndef BORWIN_H
#define BORWIN_H

#include "Graph.h"
#include "set"
#include "algorithm"
#include "chrono"

Path longestPath(Graph* g, double coefValue, double coefResource);

Path getOptimisticPath(std::string node, Graph* g, double coefValue, double coefResource, std::unordered_map<std::string,Path>* optimisticPaths);

void firstPhase(Graph* g, double* resCoefValue, double* resCoefResource);

void secondPhase(Graph* g, double coefValue, double coefResource, Path* bestPath, int* nbIter);

void writeData(Path bestPath, std::string graphFile, double timeFirstPhase, double timeSecondPhase, int nbIter);

#endif
