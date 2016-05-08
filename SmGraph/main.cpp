#include "Graph.h"
#include "GraphLoader.hpp"
#include "DynamicSummarization.h"
#include "Experiment.hpp"
#include "random.hpp"
#include "debug.hpp"

int main()
{
	string filename = "dataset/facebook_combined.txt";

	//memory leak 체크 코드
	/*debug::memory_bomb mb;
	{
		Graph* graph = readGraph(filename);
		Graph* sm_graph = readSummarizedGraph(filename);
		delete graph;
		delete sm_graph;
	}
	mb.leakCheck();*/

	//prev test code
	/*int s, t;
	for (int i = 0; i < 1000; i++)
	{
		s = random::Int(0, 4000);
		t = random::Int(0, 4000);
		DynamicSummarization::addEdge(sm_graph, graph, s, t);
	}*/


	Graph* graph = readGraph(filename);
	Graph* sm_graph = readSummarizedGraph(filename);

	//unit test -> edge 1개 추가 후 테스트
	DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, 0, 1);

	delete graph;
	delete sm_graph;

	return 0;
}