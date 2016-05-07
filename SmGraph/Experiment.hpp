#include "Graph.h"
namespace ex
{
    void printState(Graph& graph)
    {
        int ecount = 0;
        printf("node count:%d\n", graph.getNodeCount());
        int s =  graph.getNodeCount();
        for (int i = 0; i < s; ++i)
            ecount += graph.get(i)->getEdges().size();
            
        printf("edge count:%d\n", ecount);
        printf("avg degree:%f\n", (double)ecount/graph.getNodeCount());
    }
    void printCost(Graph& origin, Graph& summarized)
    {
        printf("original graph\n");
        printState(origin);
        printf("summarized graph\n");
        printState(summarized);
    }
}