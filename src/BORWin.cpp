#include "BORWin.h"

using namespace std;


int main(int argc, char** argv){
	if(argc!=2){
                cerr << "usage: "<< argv[0]<<"\n\t graph_file" << endl;
		return -1;
	}
        string graphFile = argv[1];

	//error if file not found
	Graph g = readGraph(graphFile);
	//printGraph(g);

	double coefValue, coefResource;
	Path bestPath;
	int nbIter=0;

	auto startFirstPhaseTime=chrono::high_resolution_clock::now();
	firstPhase(&g, &coefValue, &coefResource);
	auto endFirstPhaseTime=chrono::high_resolution_clock::now();
	secondPhase(&g, coefValue, coefResource, &bestPath, &nbIter);
	auto endSecondPhaseTime=chrono::high_resolution_clock::now();
	double timeFirstPhase=double(chrono::duration_cast<chrono::milliseconds>(endFirstPhaseTime - startFirstPhaseTime).count())/1000;
	double timeSecondPhase=double(chrono::duration_cast<chrono::milliseconds>(endSecondPhaseTime - endFirstPhaseTime).count())/1000;
	cout << "Time first phase: " << timeFirstPhase << " second phase: " << timeSecondPhase << endl;
	writeData(bestPath, graphFile, timeFirstPhase, timeSecondPhase, nbIter);
}

/**
 * First phase of the BORWin algorithm. Identifies the case between:
 * -LF (locally feasible)
 * -LIE (locally infeasible by excess)
 * -LID (locally infeasible by deficiency)
 * then computes the optimal coefficients providing the smallest upper bound
 *
 * @param Graph* g: pointer to the graph used
 * @param doube* resCoefResource, resCoefValue: pointers to the coefficient respectively for the resource and the value
 */

void firstPhase(Graph* g, double* resCoefValue, double* resCoefResource){

	//Compute "pathValue" only considering the value in the objective function
	Path pathValue=longestPath(g, 1.0, 0.0);

	//Case LF: the path is already feasible without considering the volume in the objective function.

	if(pathValue.resource<=g->nodes[g->targetNode].maxResource and pathValue.resource>= g->nodes[g->targetNode].minResource){
		*resCoefValue=1.0;
		*resCoefResource=0.0;
		return;
	}

	// Identifying the case LIE or LID, and compute "pathResrouce" only considering the resource in the objective funciton
	bool caseLIE=false;
	bool caseLID=false;
	Path pathResource;
	double coefResource;
	if(pathValue.resource<g->nodes[g->targetNode].minResource){
		pathResource=longestPath(g, 0.0, 1.0);
		caseLID=true;
	}
	if(pathValue.resource>g->nodes[g->targetNode].maxResource){
		pathResource=longestPath(g, 0.0, -1.0);
		caseLIE=true;
	}
	Path newPath;
	//At each iteration of the loop, we compute the largest path "newPath" considering the aggregated value with "coefValue" and "coefResource".
	//We replace "pathResource" by "newPath" if it does verify the upper bound (resp. lower bound) for the target node in the case LIE (resp. LID)
	//We then update the value of "coefResource" and iterate until convergence.
	while(newPath.nodes.size()==0 or (newPath.value!=pathValue.value and newPath.value!=pathResource.value)){
		if(newPath.nodes.size()>0){
			if((caseLIE and newPath.resource<g->nodes[g->targetNode].maxResource) or (caseLID and newPath.resource>g->nodes[g->targetNode].minResource)){
				pathResource=newPath;
			}
			else{
				pathValue=newPath;
			}
		}
		coefResource=(pathValue.value-pathResource.value)/(pathResource.resource-pathValue.resource);
		newPath=longestPath(g, 1.0, coefResource);
	}
	*resCoefValue=1.0;
	*resCoefResource=coefResource;

}

/**
 * Computes the path with the largest aggregated value, considering coefficients "coefValue" and "coefResource", from vertex "node" to the target vertex of "g".
 * Stores the obtained value in map "optimisticPaths" so that there is no need to compute twice the value from a same vertex.
 *
 * @param string node: starting node for the desired Path
 * @param Graph* g: pointer to the graph used
 * @param double coefValue, coefResource: coefficients for the value and the resource to compute the aggregated value
 * @param unordered_map<string,Path>* optimisticPaths: pointer to a structure mapping a node "u" and the largest aggregated value, considering coefficients "coefValue" and "coefResource" from "u" to the target vertex of "g"
 *
 * @see secondPhase
 */

Path getOptimisticPath(string node, Graph* g, double coefValue, double coefResource, unordered_map<string,Path>* optimisticPaths){
	//Case where "node" does not exist yet in the optimistic path map, we need to compute the longest path for "node"
	if(optimisticPaths->find(node)==optimisticPaths->end()){
		//Create a new graph "g2" similar to "g", with "node" as the source node.
		Graph g2;
		g2.nodes=g->nodes;
		g2.targetNode=g->targetNode;
		g2.sourceNode=node;
		//Compute the path "p" with the largest aggregated value in graph "g2".
		Path p = longestPath(&g2, coefValue, coefResource);
		list<string> nodes=p.nodes;
		//One can derive, for each vertex in the longest path "p", the largest aggregated value, due to the Bellman principle.
		for(auto nodeIt=nodes.begin(); nodeIt!=nodes.end(); nodeIt++){
			if(optimisticPaths->find(*nodeIt)==optimisticPaths->end()){
				(*optimisticPaths)[*nodeIt]=p;
			}
			if(*nodeIt!=g->targetNode){
				auto successorIt=nodeIt;
				successorIt++;
				p.value-=g->nodes[*nodeIt].arcs[*successorIt].value;
				p.resource-=g->nodes[*nodeIt].arcs[*successorIt].resource;
			}
			p.nodes.pop_front();
		}
	}
	return (*optimisticPaths)[node];
}

/**
 * Second phase of the BORWin algorithm.
 *
 * @param Graph* g: pointer to the graph used
 * @param double coefResource, coefValue: the coefficient respectively for the resource and the value
 * @param Path* bestPath: pointer to the best path that will be found
 * @param int* nbIter: pointer to the number of iteration of the second phase
 */

void secondPhase(Graph* g, double coefValue, double coefResource, Path* bestPath, int* nbIter){
	// will store a mapping of a node "u" with the largest aggregated value, considering coefficients "coefValue" and "coefResource" from "u" to the target vertex of "g"
	unordered_map<string,Path> optimisticPaths;
	// will store a mapping of a node "u" with another map, which maps a resource value "r" with the highest value of a path from the source node to "u" consuming exactly "r".
	unordered_map<string,unordered_map<double,double>> bestExploredPaths;

	// will store the hybrid paths that are not dominated by decreasing aggregated value
	list<HybridPath> hybridPaths;

	// create a source hybrid path
	HybridPath srcHybridPath;

	// heuristic part from source node to the target node of "g"
	srcHybridPath.heuristicPart=getOptimisticPath(g->sourceNode, g, coefValue, coefResource, &optimisticPaths);
	// feasible part only with the source node
	srcHybridPath.feasiblePart.nodes.push_back(g->sourceNode);
	// value and resource at the source node = 0
	srcHybridPath.feasiblePart.value=0.0;
	srcHybridPath.feasiblePart.resource=0.0;
	srcHybridPath.aggregatedValue=coefValue*srcHybridPath.heuristicPart.value + coefResource*srcHybridPath.heuristicPart.resource;
	// add the source hybrid path to the list of hybrid paths
	hybridPaths.push_back(srcHybridPath);

	// create variables that will be used in the algorithm
	HybridPath currentHybridPath, tempHybridPath, newHybridPath;
	bool isFeasible;
	string newSuccessor;
	double lowerBoundAggregatedValue=-INF;
	double newBound;

	// time managment
	auto startTime=chrono::high_resolution_clock::now();
	auto currentTime=chrono::high_resolution_clock::now();
	int nextPrintTime=5;

	//core loop of the algorithm
	while(hybridPaths.size()>0){
		(*nbIter)++;
		currentTime=chrono::high_resolution_clock::now();
		//Print information every 5 seconds
		if (chrono::duration_cast<chrono::seconds>(currentTime - startTime).count()>nextPrintTime){
			nextPrintTime+=5;
			cout << "Max Min aggregatedValues: " << hybridPaths.front().aggregatedValue << " " << hybridPaths.back().aggregatedValue << " lowerBound: ";
			cout << lowerBoundAggregatedValue << endl;
		}

		//Select and remove from hybridPaths the first path of hybridPaths, i.e., the one with the highest aggregated value.
		currentHybridPath=*(hybridPaths.begin());
		hybridPaths.erase(hybridPaths.begin());
		tempHybridPath.feasiblePart=currentHybridPath.feasiblePart;

		//For each node of the heuristic part of currentHybridPath
		for(auto nodeIt=currentHybridPath.heuristicPart.nodes.begin(); nodeIt!=currentHybridPath.heuristicPart.nodes.end(); nodeIt++){
			auto lastNode=tempHybridPath.feasiblePart.nodes.back();
			//Extending the feasible part
			if(*nodeIt!=lastNode){
				tempHybridPath.feasiblePart.value+=g->nodes[lastNode].arcs[*nodeIt].value;
				tempHybridPath.feasiblePart.resource+=g->nodes[lastNode].arcs[*nodeIt].resource;
				tempHybridPath.feasiblePart.nodes.push_back(*nodeIt);
				if(tempHybridPath.feasiblePart.resource<g->nodes[*nodeIt].minResource or tempHybridPath.feasiblePart.resource>g->nodes[*nodeIt].maxResource){
					break;
				}
			}
			//Checks if there was a feasible part of a path that ended at the same node, with the same resource and a higher value.
			if(bestExploredPaths.find(*nodeIt)==bestExploredPaths.end() or bestExploredPaths[*nodeIt].find(tempHybridPath.feasiblePart.resource)==bestExploredPaths[*nodeIt].end()){
				bestExploredPaths[*nodeIt][tempHybridPath.feasiblePart.resource]=tempHybridPath.feasiblePart.value;
			}
			else if(bestExploredPaths[*nodeIt][tempHybridPath.feasiblePart.resource]>=tempHybridPath.feasiblePart.value){
				break;
			}
			else{
				bestExploredPaths[*nodeIt][tempHybridPath.feasiblePart.resource]=tempHybridPath.feasiblePart.value;
			}

			//Case a new best path is found i.e., one of the following cases:
			//-no existing feasible path
			//-new path with higher value
			//-new path with equal value and lower resource
			if(*nodeIt==g->targetNode and (bestPath->nodes.size()==0 or bestPath->value<tempHybridPath.feasiblePart.value or (bestPath->value==tempHybridPath.feasiblePart.value and bestPath->resource>tempHybridPath.feasiblePart.resource))){
				//Update the lower bound on the aggregated value
				newBound=coefValue*tempHybridPath.feasiblePart.value+min(coefResource*g->nodes[g->targetNode].minResource, coefResource*g->nodes[g->targetNode].maxResource);
				lowerBoundAggregatedValue=max(lowerBoundAggregatedValue, newBound);
				//Update the best path found
				(*bestPath)=tempHybridPath.feasiblePart;
				//remove from the list all path high lower aggregated value than the lower bound
				auto listHybridPathIt=hybridPaths.begin();
				while((*listHybridPathIt).aggregatedValue>=lowerBoundAggregatedValue && listHybridPathIt!=hybridPaths.end())
					listHybridPathIt++;
				if(listHybridPathIt!=hybridPaths.end())
					hybridPaths.erase(listHybridPathIt,hybridPaths.end());
			}
			auto successorIt=nodeIt;
			successorIt++;
			for(auto arcIt=g->nodes[*nodeIt].arcs.begin(); arcIt!=g->nodes[*nodeIt].arcs.end(); arcIt++){
				newSuccessor=arcIt->first;
				//Extend for nodes different than the ones in currentHybridPath
				if(newSuccessor!=*successorIt){
					//Create new hybrid path with the new extended node
					newHybridPath=tempHybridPath;
					newHybridPath.feasiblePart.value+=g->nodes[*nodeIt].arcs[newSuccessor].value;
					newHybridPath.feasiblePart.resource+=g->nodes[*nodeIt].arcs[newSuccessor].resource;
					newHybridPath.feasiblePart.nodes.push_back(newSuccessor);

					//If the feasible part is indeed feasible, we compute the heuristic part and add the new hybrid path in the hybridPaths list
					if(newHybridPath.feasiblePart.resource>=g->nodes[newSuccessor].minResource and newHybridPath.feasiblePart.resource<=g->nodes[newSuccessor].maxResource){
						newHybridPath.heuristicPart=getOptimisticPath(newSuccessor, g, coefValue, coefResource, &optimisticPaths);
						newHybridPath.aggregatedValue=coefValue*(newHybridPath.feasiblePart.value+newHybridPath.heuristicPart.value);
						newHybridPath.aggregatedValue+=coefResource*(newHybridPath.feasiblePart.resource+newHybridPath.heuristicPart.resource);
						if(newHybridPath.aggregatedValue>=lowerBoundAggregatedValue){
							//Keeping the list sorted by decreasing aggregated value
							auto listHybridPathIt = lower_bound(hybridPaths.begin(), hybridPaths.end(), newHybridPath, [](HybridPath a, HybridPath b) {return a.aggregatedValue > b.aggregatedValue;});
							hybridPaths.insert(listHybridPathIt, newHybridPath);
						}
					}
				}
			}
		}
		//cout << "\ncurrentList:" << endl;
		//for(auto it=hybridPaths.begin(); it!=hybridPaths.end(); it++){
		//	printHybridPath(*it);
		//}
	}
	//Print the solution
	cout << "\nSolution:" << endl;
	printPath(*bestPath);
	double value=0.0;
	for(auto it=bestPath->nodes.begin(); it!=bestPath->nodes.end(); it++){
		auto succ=it;
		succ++;
		if(succ!=bestPath->nodes.end())
			value+=g->nodes[*it].arcs[*succ].value;
	}
	cout << value << endl;
}
/**
 * Computes the longest path in a directed acyclic graph considering an aggregated value
 *
 * @param Graph* g: pointer to the graph used
 * @param double coefResource, coefValue: the coefficient respectively for the resource and the value
 *
 * @return the Path "p" being the longest path in "g"
 *
 * @see firstPhase()
 * @see getOptimisticPath()
 */
Path longestPath(Graph* g, double coefValue, double coefResource){
	//maps each node "u" to the value of the longest path form the source node to "u"
	unordered_map<string, double> longestValue;

	//maps each node "u" to its predecessor in the longest path form the source node to "u"
	unordered_map<string, string> longestPrevious;

	//set of nodes to open (element can only exist once in a set, no duplicates)
	set<string> nodesToOpen;

	// initializ the longest values to INF for all nodes exept the source node for which it's 0
	for(auto it=g->nodes.begin(); it!=g->nodes.end(); it++){
		longestValue[it->first]=INF;
	}
	longestValue[g->sourceNode]=0.0;

	
	nodesToOpen.insert(g->sourceNode);

	double newValue;

	string currentNode;
	//core loop of the algorithm
	while(nodesToOpen.size()>0){
		//retrieve the first node of the list of nodes to open
		currentNode=(*nodesToOpen.begin());
		//for each neighbor of the node to open
		for(auto arcIt=g->nodes[currentNode].arcs.begin(); arcIt!=g->nodes[currentNode].arcs.end(); arcIt++){
			if(longestValue[currentNode]<INF-1){
				//compute the new value
				newValue=longestValue[currentNode]+coefResource*arcIt->second.resource + coefValue*arcIt->second.value;
				//update the value and the predecessors of a node "u" if the new value is better than best value for "u" so far, or if no value exists for "u"
				if(newValue>longestValue[arcIt->second.to] or longestValue[arcIt->second.to]>=INF-1){
					nodesToOpen.insert(arcIt->second.to);
					longestValue[arcIt->second.to]=newValue;
					longestPrevious[arcIt->second.to]=currentNode;
				}
			}
		}
		nodesToOpen.erase(currentNode);
	}
	
	Path p;
	p.nodes.push_back(g->targetNode);
	currentNode=g->targetNode;
	while(currentNode!=g->sourceNode){
		p.nodes.push_front(longestPrevious[currentNode]);
		currentNode=longestPrevious[currentNode];
	}
	p.value=0.0;
	p.resource=0.0;
	for(auto nodeIt=p.nodes.begin(); nodeIt!=--p.nodes.end(); nodeIt++){
		auto successorIt=nodeIt;
		successorIt++;
		p.value+=g->nodes[*nodeIt].arcs[*(successorIt)].value;
		p.resource+=g->nodes[*nodeIt].arcs[*(successorIt)].resource;
	}
	return p;
}

/**
 * Writes the solution to a CSV file
 * Write the solution in a CSV file
 */
void writeData(Path bestPath, string graphFile, double timeFirstPhase, double timeSecondPhase, int nbIter){

	string fileOut = "../out/"+graphFile;
	fileOut.erase(fileOut.size()-3);
	fileOut+="csv";
	cout << "Result written at: " << fileOut << endl;

        ofstream result(fileOut.c_str(), ios::out);

	result << "Objective value:," << bestPath.value << "\n";
	result << "Resource used:," << bestPath.resource << "\n";
	result << "Time:," << timeFirstPhase+timeSecondPhase << ", First phase:," << timeFirstPhase << ", Second phase:," << timeSecondPhase << "\n";
	result << "#Iterations:," << nbIter << "\n";
	result << "Path:,";
	for(auto it=bestPath.nodes.begin(); it!=bestPath.nodes.end(); it++){
		if(it!=bestPath.nodes.begin()){
			result << ",";
		}
		result << "(" << *it << ")";
	}
	result.close();
}
