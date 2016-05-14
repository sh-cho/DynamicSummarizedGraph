#include "Graph.h"
#include "GraphLoader.hpp"
#include "DynamicSummarization.h"
#include "Experiment.hpp"
#include "random.hpp"
#include "debug.hpp"

int main()
{
	//string filename = "dataset/facebook_combined_randomized.txt";
	//ifstream fin("dataset/facebook_combined_edges.txt");
	
	string filename = "dataset/facebook_combined.txt";
	//ifstream fin("dataset/facebook_combined_edges.txt");


	//edge 파일들 불러와서 벡터에 넣어두기
	/*
	vector<pair<int, int>> addEdgeList;
	int from, to;
	while (fin >> from >> to)
	{
		addEdgeList.push_back({ from, to });
	}
	*/
	



	{
		int s, t;
		Graph* graph = readGraph(filename);
		Graph* sm_graph = readSummarizedGraph(filename);

		debug::time_bomb tb;
		

		//one edge test
		s = 2121;
		t = 2415;
		DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, s, t);
		

		//for (int i = 0; i < 1000; i++)
		//{
		//	//cout << i << " ";
		//	s = random::Int(0, 4000);
		//	t = random::Int(0, 4000);
		//	printf("i:%d, s:%d, t:%d\n", i, s, t);
		//	DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, s, t);
		//}

		/*for (auto edge : addEdgeList)
		{
			DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, edge.first, edge.second);
		}*/


		//cout << "end" << endl;
		//DynamicSummarization::countEdges(*sm_graph, *graph);


		delete graph;
		delete sm_graph;
	}
	cout << endl;




	return 0;
}