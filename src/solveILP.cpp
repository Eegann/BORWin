#include "Graph.h"
#include "ILP.h"
#include "algorithm"
#include "chrono"

using namespace std;


int main(int argc, char** argv){
        if(argc!=2){
                cerr << "usage: "<< argv[0]<<"\n\t graphFile" << endl;
                return -1;
        }
        string graphFile = argv[1];

        //error if file not found
        Graph g = readGraph(graphFile);
        //printGraph(g);

        double coefValue, coefResource;
        cout << "### SOLVE WWITH ILP ###" << endl;
        auto startTime=chrono::high_resolution_clock::now();
        Path bestPath=solveILP(&g);
        auto endTime=chrono::high_resolution_clock::now();
        double solveTime=double(chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count())/1000;
        cout << "Solve Time: " << solveTime << endl;
        printPath(bestPath);
//writeData(bestPath, graphFile, timeFirstPhase, timeSecondPhase, nbIter);
}
