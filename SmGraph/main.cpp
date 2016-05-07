//#include "random.hpp"
#include "Graph.h"
#include "GraphLoader.hpp"
#include "DynamicSummarization.h"
#include "Experiment.hpp"
#include "random.hpp"
#include "debug.hpp"

int main()
{
    string filename = "dataset/facebook_combined.txt";
    
	debug::memory_bomb mb;
	{
		Graph* graph = readGraph(filename);
		Graph* sm_graph = readSummarizedGraph(filename);
		delete graph;
		delete sm_graph;
	}
	mb.leakCheck();
    int s, t;
    for (int i = 0; i < 1000; i++)
    {
        s = random::Int(0, 4000);
        t = random::Int(0, 4000);
        //DynamicSummarization::addEdge(sm_graph, graph, 0, 100);
    }
    
    return 0;
}