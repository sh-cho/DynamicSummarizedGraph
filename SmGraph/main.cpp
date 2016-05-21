//#define _SECURE_SCL 0


#include "Graph.h"
#include "GraphLoader.hpp"
#include "DynamicSummarization.h"
#include "Experiment.hpp"
#include "random.hpp"
#include "debug.hpp"


/**
 *	FILENAME : dataset file name.
 *	ex)	"datset/com-amazon.ungraph.txt"
 *	ex2) "dataset/facebook-combined"
 *
 *	RAND_PERCENT : Randomized 비율. 0~1
 *	ex) 0.15
 */

//	#define	FILENAME	"dataset/com-amazon.ungraph.txt"
	#define	FILENAME	"dataset/com-dblp.ungraph.txt"
//	#define	FILENAME	"dataset/Gowalla.txt"
//	#define	FILENAME	"dataset/facebook_combined"

//	#define	RAND_RATIO	0.05
	#define	RAND_RATIO	0.10
//	#define	RAND_RATIO	0.15
//	#define	RAND_RATIO	0.20
//	#define	RAND_RATIO	0.25
//	#define	RAND_RATIO	0.30


//	#define	CSH_DEBUG	true



int main()
{
	///string filename = "dataset/com-amazon.ungraph.txt_randomized15p.txt";
	///ifstream fin("dataset/com-amazon.ungraph.txt_edges15p.txt");

	///string filename = "dataset/com-dblp.ungraph.txt_randomized10p.txt";
	///ifstream fin("dataset/com-dblp.ungraph.txt_edges10p.txt");
	
	///string filename = "dataset/Gowalla.txt_randomized15p.txt";
	///ifstream fin("dataset/Gowalla.txt_edges15p.txt");

	///string filename = "dataset/facebook_combined_randomized30p.txt";
	///ifstream fin("dataset/facebook_combined_edges30p.txt");


	string filename;
	ifstream fin;
	int erase_percent = RAND_RATIO * 100;

	filename = string(FILENAME) + "_randomized" + to_string(erase_percent) + "p.txt";
	fin.open(string(FILENAME) + "_edges" + to_string(erase_percent) +  "p.txt");

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
	int cnt = 0;
	cout << "* dynamic summarization start" << endl;
	cout << "\n* filename: " << FILENAME << endl;
	cout << "* randomized percent: " << erase_percent << endl;
	cout << "* threshold: " << threshold << endl;
	cout << "\n*** time: ";
	{
		Graph* graph = readGraph(filename);
		Graph* sm_graph = readSummarizedGraph(filename);

		{
			debug::time_bomb tb;


			//one edge test
			//int s, t;
			//s = 2121, t = 2415;
			//s = 1912, t = 2028;
			//DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, s, t, threshold);


			for (auto edge : addEdgeList)
			{
				cout << ++cnt << " ";
				cout << edge.first << " " << edge.second << endl;

				DynamicSummarization::addEdgeAndSummarize(*sm_graph, *graph, edge.first, edge.second, threshold);
			}
		}
		cout << " second(s)" << endl << endl;


		//print graph info
		cout << "*** print graph info" << endl;
		sm_graph->printInfo();


		cout << "* graph deletion start" << endl;
		delete graph;
		delete sm_graph;
		cout << "* graph deletion end" << endl;
	}
	cout << "* dynamic summarization end" << endl;



	return 0;
}