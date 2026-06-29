#include "Generator.h"

using namespace std;

#define MAX_VALUE 1000
#define MAX_RESOURCE 1000

int main(int argc, char** argv){
	if(argc!=4 and argc !=6){
        cerr << "usage: "<< argv[0]<<"\n\t numberNodes : (int) number of nodes in the graph, at least 3" << "\n\t density : (double) density of the graph, between 0 and 1" << "\n\t numberLevels : (int) nb of maximum levels in the graph, between 3 and numberNodes" << endl;
		cerr << "\t [optionnal] maxValue : (int) maximum value on an arc\n\t [optionnal] maxResource : (int) maximum resource on an arc" << endl;
		return -1;
	}
	int numberNodes = atoi(argv[1]);
	double density = atof(argv[2]);
	int numberLevels = atoi(argv[3]);
	double maxValue = MAX_VALUE;
	double maxResource = MAX_RESOURCE;
	if(argc == 6){
		maxValue = atof(argv[4]);
		maxResource = atof(argv[4]);
	}
	if(numberNodes < 3)
		cerr << "numberNodes = " << numberNodes << " should be at least 3" << endl;
	if(density < 0 or density > 1)
		cerr << "density = " << density << " should be between 0 and 1" << endl;
	if(numberLevels < 3)
		cerr << "numberLevels = " << numberLevels << " should be at least 3" << endl;
	if(numberLevels > numberNodes)
		cerr << "numberLevels = " << numberLevels << " and numberNodes = " << numberNodes << " should be more nodes than levels" << endl;
	
	srand(time(0));

	Graph g = makeGraph(numberNodes, density, numberLevels, maxValue, maxResource);

	//printGraph(g, numberNodes);

	makeResourceWindows(&g);

	//printGraph(g, numberNodes);

	string folderName = ("N="+to_string(numberNodes)+"_D="+to_string(density)).c_str();
	folderName.erase(folderName.find_last_not_of('0') + 1, std::string::npos);
	folderName = (folderName+"_L="+to_string(numberLevels)).c_str();

	writeGraph(g, numberNodes, folderName);
}

/**
 * Make a graph corresponding to the given parameters
 *
 * @param int numberNodes: number of nodes in the graph
 * @param double density: the density of the graph
 * @param int numberLevels: the number of levels in the graph
 * @param double maxValue: the maximum value of an arc
 * @param double maxResource: the maximum resource of an arc
 *
 * @return the Graph g built
 */
Graph makeGraph(int numberNodes, double density, int numberLevels, double maxValue, double maxResource){
	
	Graph g;
	map<int, vector<string>> levels;
	for(int i=0; i<numberNodes; i++){
		Node currentNode;
		currentNode.id = to_string(i);
		currentNode.maxResource = INF;
		currentNode.minResource = 0;
		// add the source node at the first level
		if(i == 0){
			levels[0].push_back(currentNode.id);
			g.sourceNode=currentNode.id;
		}
		// add the target node at the last level
		else if(i == numberNodes-1){
			levels[numberLevels-1].push_back(currentNode.id);
			g.targetNode=currentNode.id;
		}
		// add all other nodes to randomly selected intermediate levels
		else{
			levels[rand() % (numberLevels-2) + 1].push_back(currentNode.id);
		}
		// add the node to the graph
		g.nodes[currentNode.id] = currentNode;
	}
	for(auto it=levels.begin(); it != levels.end(); it++){
		cout << it->first << " : ";
		for(auto itt : it->second){
			cout << itt<< " ";
		}
		cout << endl;
	}
	for(int parentLevel=0; parentLevel < numberLevels-1; parentLevel++){

		for(int childrenLevel=parentLevel+1; childrenLevel < numberLevels; childrenLevel++){
			for(string parentID:levels[parentLevel]){
				for(string childrenID:levels[childrenLevel]){
					// always add from source to the first level
					if( parentLevel == 0 and childrenLevel == 1){
						makeNewArc(&g, parentID, childrenID, maxValue, maxResource);
					}
					// always add from the penultimate level to the target
					else if(childrenLevel == numberLevels-1 and parentLevel == numberLevels-2){
						makeNewArc(&g, parentID, childrenID, maxValue, maxResource);
					}
					// follow the density probability but never add from source to target
					else if(childrenLevel - parentLevel < numberLevels-1 and ((double)rand()) / RAND_MAX < density){
						makeNewArc(&g, parentID, childrenID, maxValue, maxResource);
					}
				}
			}
		}
	}
	return g;
}

/**
 * Creates a new arc in a graph with random value and resource
 *
 * @param Graph* g: pointer to the graph in which we add the arc
 * @param string parentID: the id of the source of the arc
 * @param string childrenID: the id of the target of the arc
 * @param double maxValue: the maximum value for the arc
 * @param double maxResource: the maximum resource for the arc
 */
void makeNewArc(Graph* g, string parentID, string childrenID, double maxValue, double maxResource){
	Arc newArc;
	newArc.to = childrenID;
	newArc.value = makeRandomDouble(0, maxValue);
	newArc.resource = makeRandomDouble(0, maxResource);
	g->nodes[parentID].arcs[newArc.to]=newArc;
}

/**
 * Makes a random double value between two bounds
 *
 * @param double minValue: the minimum allowed value
 * @param double maxValue: the maximum allowed value
 */
double makeRandomDouble(double minValue, double maxValue){
	return minValue + (maxValue - minValue) * (rand() % (int(maxValue)*100)) / (int(maxValue)*100);
}

/**
 * Makes a resource window for the target node of a graph
 *
 * @param Graph* g: pointer to the graph for which we add the resource window
 * 
 * @see makeRandomDouble()
 */
void makeResourceWindows(Graph* g){
	Path pMaxResource = longestPath(g, 0.0, 1.0);
	Path pMinResource = longestPath(g, 0.0, -1.0);

	double lowerBound = pMinResource.resource;
	double upperBound = pMaxResource.resource;

	if(lowerBound == upperBound){
		cerr << "Un seul chemin réalisable" << endl;
	}

	double bound1 = makeRandomDouble(lowerBound, upperBound);
	double bound2 = makeRandomDouble(lowerBound, upperBound);
	while(bound1 == bound2){
		bound2 = makeRandomDouble(lowerBound, upperBound);
	}

	cout << " LB:" << lowerBound << " UB:" << upperBound << endl;
	cout << "Bounds:" << bound1 << " " << bound2 << endl;

	string targetID = g->targetNode;
	if(bound1<bound2){
		g->nodes[targetID].minResource=bound1;
		g->nodes[targetID].maxResource=bound2;
	}
	else{
		g->nodes[targetID].minResource=bound2;
		g->nodes[targetID].maxResource=bound1;
	}
}