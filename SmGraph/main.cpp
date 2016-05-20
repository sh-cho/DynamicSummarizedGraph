//#define _SECURE_SCL 0


#include "Graph.h"
#include "GraphLoader.hpp"
#include "DynamicSummarization.h"
#include "Experiment.hpp"
//#include "random.hpp"
#include "debug.hpp"


//#include <time.h>	//clock_t


int main()
{
	//string filename = "dataset/com-amazon.ungraph.txt_randomized10p.txt";
	//ifstream fin("dataset/com-amazon.ungraph.txt_edges10p.txt");

	//string filename = "dataset/com-dblp.ungraph.txt_randomized10p.txt";
	//ifstream fin("dataset/com-dblp.ungraph.txt_edges10p.txt");
	
	//string filename = "dataset/Gowalla.txt_randomized25p.txt";
	//ifstream fin("dataset/Gowalla.txt_edges25p.txt");

	string filename = "dataset/facebook_combined_randomized30p.txt";
	ifstream fin("dataset/facebook_combined_edges30p.txt");

	vector<pair<int, int>> addEdgeList;	//edge 파일들 불러와서 벡터에 넣어두기
	int from, to;
	if (!fin)
	{
		cerr << "edge file error" << endl;
		exit(1);
	}

	cout << "* edge file load start" << endl;
	while (fin >> from >> to)
	{
		addEdgeList.push_back({ from, to });
	}
	cout << "* edge file load end" << endl;
	

	//when load orig data
	//string filename = "dataset/facebook_combined.txt";



	double threshold = 0.4;
	//int cnt = 0;
	cout << "* dynamic summarization start" << endl;
	cout << "* filename: " << filename << endl;
	cout << "* threshold: " << threshold << endl;
	{
		Graph* graph = readGraph(filename);
		Graph* sm_graph = readSummarizedGraph(filename);

		debug::time_bomb tb;
		

		//one edge test
		//int s, t;
		//s = 2121, t = 2415;
		//s = 1912, t = 2028;
		//DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, s, t, threshold);
		
		
		//clock_t before;
		//double result;
		for (auto edge : addEdgeList)
		{
			//++cnt;
			//cout << cnt << " ";

			//before = clock();
			DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, edge.first, edge.second, threshold);
			//result = (double)(clock() - before) / CLOCKS_PER_SEC;
			//printf("from:%d, to:%d, elapsed:%5.3f\n", edge.first, edge.second, result);
		}
		


		delete graph;
		delete sm_graph;
	}
	cout << endl;
	cout << "end" << endl;



	return 0;
}