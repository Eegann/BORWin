#include "Graph.h"

using namespace std;

/**
 * Print a Graph
 * @param Graph g: the graph to be printed
 */
void printGraph(Graph g){
	
	for(int id=0; id < g.nodes.size(); id++){
		string nodeID = to_string(id);
		cout << "ID : " << g.nodes[nodeID].id << ", Min resource : " << g.nodes[nodeID].minResource << " Max resource : " << g.nodes[nodeID].maxResource << " Arcs : {" << endl;
		for(auto it=g.nodes[nodeID].arcs.begin(); it!=g.nodes[nodeID].arcs.end(); it++){
			cout << "\tTowards " << it->second.to << ", Value : " << it->second.value << ", Resource : " << it->second.resource << endl;
		}
		cout << "\t}" << endl;
	}
	cout << "Source : " << g.sourceNode << " | Target : " << g.targetNode << endl;
}

/**
 * Create a graph from a data file
 * @param string filename: file containing the graph data
 *
 * @return the Graph build from the data file
 */
Graph readGraph(std::string fileName){
	std::ifstream graphFile(("../data/"+fileName+".txt").c_str());

	if (!graphFile){
		std::cerr<<"file "<<fileName<<" not found"<<std::endl;
	}
	std::string line;
	Node currentNode;
	Graph g;
	Arc currentArc;
	std::vector<std::string> lineTokens;
	std::vector<std::string> nodeTokens;
	std::vector<std::string> arcTokens;
	int x=0;
	while(std::getline(graphFile,line)){
		x += 1;
		cout << x << endl;
		lineTokens.clear();
		if(line.rfind("id", 0)==0){
			cout << 'A' << endl;
			tokenize(line, '|', lineTokens);
			nodeTokens.clear();
			tokenize(lineTokens[0], ' ', nodeTokens);
			currentNode.id=nodeTokens[1];
			currentNode.minResource=std::stod(nodeTokens[3]);
			currentNode.maxResource=std::stod(nodeTokens[5]);
			currentNode.arcs.clear();

			for(int i=1; i<lineTokens.size(); i++){
				arcTokens.clear();
				tokenize(lineTokens[i], ' ', arcTokens);
				currentArc.to=arcTokens[1];
				currentArc.value=std::stod(arcTokens[3]);
				currentArc.resource=std::stod(arcTokens[5]);
				currentNode.arcs[currentArc.to]=currentArc;
			}
			g.nodes[currentNode.id]=currentNode;
			cout << 'B' << endl;
		}
		else if(line.rfind("source", 0)==0){
			cout << 'C' << endl;
			tokenize(line, ' ', lineTokens);
			g.sourceNode=lineTokens[1];
			g.targetNode=lineTokens[3];
			cout << 'D' << endl;
		}
		cout << 'E' << endl;
	}
	cout << 'F' << endl;
	return g;
}

/**
 * Write graph data into a file
 * @param Graph g: the graph to write in a file
 * @param int numberNodes: the number of nodes in the graph
 * @param string folderName: name of the folder in which we store the graph data file
 */
void writeGraph(Graph g, int numberNodes, string folderName){
	
	int i=1;

	filesystem::create_directory("../data/"+folderName);
	
	ifstream tempFichier("../data/"+folderName+"/instance_"+to_string(i)+".txt");

	while (tempFichier.good()){
		tempFichier.close();
		i += 1;
		tempFichier.open("../data/"+folderName+"/instance_"+to_string(i)+".txt");
	} 
	tempFichier.close();
	
	
  	ofstream graphFile("../data/"+folderName+"/instance_"+to_string(i)+".txt");

	for(int id=0; id < numberNodes; id++){
		string nodeID = to_string(id);
		graphFile << "id " << g.nodes[nodeID].id << " minResource " << g.nodes[nodeID].minResource << " maxResource " << g.nodes[nodeID].maxResource;
		for(auto it=g.nodes[nodeID].arcs.begin(); it!=g.nodes[nodeID].arcs.end(); it++){
			graphFile << "|to " << it->second.to << " value " << it->second.value << " resource " << it->second.resource;
		}
		graphFile << endl;
	}
	graphFile << "source " << g.sourceNode << " target " << g.targetNode << endl;
	graphFile.close();
	cout << "Graph written at: " << ("../data/"+folderName+"/instance_"+to_string(i)+".txt").c_str() << endl;
}

/**
 * Split a string with respect to a given delimiter
 *
 * @param string const &str: the string to be split
 * @param const char delim: the delimiter
 * @param vector<string> &out: the vector containing the split string
 *
 * @see readGraph()
 */
void tokenize(std::string const &str, const char delim,
            std::vector<std::string> &out){
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
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
 * Print a path p
 *
 * @param Path p: the path to be printed
 */
void printPath(Path p){
	std::cout << "path: ";
	for(auto it=p.nodes.begin(); it!=p.nodes.end(); it++){
		if(it!=p.nodes.begin()){
			std::cout << " -> ";
		}
		std::cout << "(" << *it << ")";
	}
	std::cout << "\nValue: " << p.value << " Resource: " << p.resource << std::endl;
}


/**
 * Print a hybrid path p
 *
 * @param HybridPath p: the hybrid path to be printed
 */
void printHybridPath(HybridPath h){
	std::cout << "Feasible ";
	printPath(h.feasiblePart);
	std::cout << "Heuristic ";
	printPath(h.heuristicPart);
	std::cout << "Aggregated value: " << h.aggregatedValue << std::endl;
}
