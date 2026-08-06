#include "ILP.h"

using namespace std;

Path solveILP(Graph* g) {

   IloEnv   env;
   try {
      IloModel model(env);
      int nbArcs=countArcs(g);
      IloNumVarArray x(env,nbArcs,0,1,ILOINT);
      IloNumArray V(env,nbArcs);
      IloNumArray R(env,nbArcs);
      IloNumArray from(env,nbArcs);
      IloNumArray to(env,nbArcs);
      IloNumVar TR(env);

      int idArc=0;
      for(int id=0; id < g->nodes.size(); id++){
        string nodeID=to_string(id);
        for(auto it=g->nodes[nodeID].arcs.begin(); it!=g->nodes[nodeID].arcs.end(); it++){
                from[idArc]=id;
                to[idArc]=stoi(it->second.to);
                V[idArc]=it->second.value;
                R[idArc]=it->second.resource;
                idArc++;
        }
      }
      // Objective
      model.add(IloMaximize(env,IloScalProd(V,x)));
      // flow conservation out of source node
      int s=stoi(g->sourceNode);
      IloExpr outflowofsource(env);
      for (int a=0;a<nbArcs;a++)
        if (from[a]==s)
                outflowofsource+=x[a];
      model.add(outflowofsource==1);

      // flow conservation to target node
      int t=stoi(g->targetNode);
      IloExpr inflowoftarget(env);
      for (int a=0;a<nbArcs;a++)
        if (to[a]==t)
                inflowoftarget+=x[a];
      model.add(inflowoftarget==1);

      // flow conservation elsewhere
      for(int id=0; id < g->nodes.size(); id++)
        if (id !=s && id !=t) {
                IloExpr inflowofnode(env);
                IloExpr outflowofnode(env);
                for (int a=0;a<nbArcs;a++)
                        if (to[a]==id)
                                inflowofnode+=x[a];
                        else if (from[a]==id)
                                outflowofnode+=x[a];
                model.add(inflowofnode-outflowofnode==0);
        }

      // resource window constraint on the target node
      model.add(IloRange(env, g->nodes[g->targetNode].minResource,IloScalProd(R,x),g->nodes[g->targetNode].maxResource));

      //to have the resource value
      model.add(TR==IloScalProd(R,x));

      // Optimize
      IloCplex cplex(model);
      //cplex.setOut(env.getNullStream());
      //cplex.setWarning(env.getNullStream());
      cplex.solve();

      if (cplex.getStatus() == IloAlgorithm::Infeasible) {
         env.out() << "No Solution" << endl;
         Path p;
         p.value=0.0;
         p.resource=0.0;
         return p;
      }

      env.out() << "Solution status: " << cplex.getStatus() << endl;

      // Print results abd store path
      env.out() << "Value" << cplex.getObjValue() << endl;
      Path p;
      int indArc=0;
      p.nodes.push_back(g->sourceNode);
      int dest=stoi(g->sourceNode);
      do {
        int orig=dest;
        for (int a=0;a<nbArcs;a++)
                if (from[a]==orig && cplex.getValue(x[a])>=0.99) {
                        dest=to[a];
                        p.nodes.push_back(to_string(dest));
                }
      } while (dest != stoi(g->targetNode));
      p.value=cplex.getObjValue();
      p.resource=cplex.getValue(TR);
      return p;
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Error" << endl;
   }
   env.end();
   Path p;
   p.value=0.0;
   p.resource=0.0;
   return p;


}

