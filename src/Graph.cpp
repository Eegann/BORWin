#include "Graph.h"

/**
 * Print a Graph
 * @param Graph g: the graph to be printed
 */
void printGraph(Graph g){
	std::cout << "Source: " << g.sourceNode << "\tTarget: " << g.targetNode << std::endl;
	std::cout << "Nodes:" << std::endl;
	for(auto nodeIt=g.nodes.begin(); nodeIt!=g.nodes.end(); nodeIt++){
		std::cout << "ID:" << nodeIt->second.id << " minResource: " << nodeIt->second.minResource << " maxResource: " << nodeIt->second.maxResource << " arcs: " << std::endl;
		for(auto arcIt=nodeIt->second.arcs.begin(); arcIt!=nodeIt->second.arcs.end(); arcIt++){
			std::cout << "\tto: " << arcIt->second.to << " value: " << arcIt->second.value << " resource: " << arcIt->second.resource << std::endl;
		}
	}

}

/**
 * Create a graph from a data file
 * @param string filename: file containing the graph data
 *
 * @eturn the Graph build from the data file
 */
Graph readGraph(std::string fileName){
	std::ifstream graphFile(("../data/"+fileName).c_str());

        if (!graphFile){
                std::cerr<<"file "<<fileName<<" not found"<<std::endl;
        }
	std::string line;
	Node currentNode;
	Graph g;
	Arc currentArc;
	std::vector<std::string> tokens;
	while(std::getline(graphFile,line)){
		tokens.clear();
		if(line.rfind("\t", 0)==0){
			tokenize(line, ' ', tokens);
			currentArc.to=tokens[1];
			currentArc.value=std::stod(tokens[3]);
			currentArc.resource=std::stod(tokens[5]);
			currentNode.arcs[currentArc.to]=currentArc;
		}
		else if(line.rfind("id", 0)==0){
			if(currentNode.id!=""){
				g.nodes[currentNode.id]=currentNode;
			}
			currentNode={};
			tokenize(line, ' ', tokens);
			currentNode.id=tokens[1];
			currentNode.minResource=std::stod(tokens[3]);
			currentNode.maxResource=std::stod(tokens[5]);
		}
		else if(line.rfind("source", 0)==0){
			g.nodes[currentNode.id]=currentNode;
			tokenize(line, ' ', tokens);
			g.sourceNode=tokens[1];
			g.targetNode=tokens[3];
		}
	}
	return g;
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
