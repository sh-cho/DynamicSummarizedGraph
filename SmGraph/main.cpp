#include "Graph.h"
#include "GraphLoader.hpp"
#include "DynamicSummarization.h"
#include "Experiment.hpp"
#include "random.hpp"
#include "debug.hpp"


#include <time.h>	//clock_t


int main()
{
	//when load randomized data
	string filename = "dataset/facebook_combined_randomized.txt";
	ifstream fin("dataset/facebook_combined_edges.txt");
	//edge 파일들 불러와서 벡터에 넣어두기
	vector<pair<int, int>> addEdgeList;
	int from, to;
	while (fin >> from >> to)
	{
		addEdgeList.push_back({ from, to });
	}
	

	//when load orig data
	//string filename = "dataset/facebook_combined.txt";



	double threshold = 0.4;
	
	cout << "start" << endl;
	{
		Graph* graph = readGraph(filename);
		Graph* sm_graph = readSummarizedGraph(filename);

		debug::time_bomb tb;
		

		//one edge test
		//int s, t;
		//s = 2121, t = 2415;
		//s = 1912, t = 2028;
		//DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, s, t, threshold);
		

		//for (int i = 0; i < 1000; i++)
		//{
		//	//cout << i << " ";
		//	s = random::Int(0, 4000);
		//	t = random::Int(0, 4000);
		//	//printf("i:%d, s:%d, t:%d\n", i, s, t);
		//	DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, s, t, threshold);
		//}
		

		
		clock_t before;
		double result;
		for (auto edge : addEdgeList)
		{
			before = clock();
			DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, edge.first, edge.second, threshold);
			result = (double)(clock() - before) / CLOCKS_PER_SEC;
			printf("from:%d, to:%d, elapsed:%5.3f\n", edge.first, edge.second, result);
		}


		delete graph;
		delete sm_graph;
	}
	cout << endl;
	cout << "end" << endl;



	return 0;
}