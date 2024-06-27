#ifndef GRAPH_H
#define GRAPH_H


#include "unordered_map"
#include "cfloat"
#include "fstream"
#include "iostream"
#include "ostream"
#include "vector"
#include "string"
#include "list"


#define INF DBL_MAX

struct Arc{
	std::string to;
	double value=0.0;
	double resource=0.0;

};
struct Node{
	std::string id;
	double minResource=0.0;
	double maxResource=0.0;
	std::unordered_map<std::string, Arc> arcs;
};
struct Graph{
	std::unordered_map<std::string, Node> nodes;
	std::string sourceNode;
	std::string targetNode;
};
struct Path{
	std::list<std::string> nodes;
	double value=0.0;
	double resource=0.0;
};
struct HybridPath{
	Path feasiblePart;
	Path heuristicPart;
	double aggregatedValue=0.0;
};

Graph readGraph(std::string fileName);

void tokenize(std::string const &str, const char delim,
            std::vector<std::string> &out);

void printGraph(Graph g);

void printPath(Path p);

void printHybridPath(HybridPath h);


#endif
