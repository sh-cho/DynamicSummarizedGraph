/**
 *	Graph 파일 읽어서 70%의 Edge만 남겨서 새로운 Graph 파일로 저장
 *	30%의 edge는 따로 파일로 저장
 */

#define	INPUT_NAME	"dataset/facebook_combined"
//#define	RANDOMIZED_FILE	"facebook_randomized.txt"
//#define EDGES_FILE	"facebook_randomized_edges.txt"

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>	//std::pair
#include <vector>
using namespace std;

int main()
{
	srand((unsigned)time(nullptr));

	string inputFileName = INPUT_NAME + string(".txt");
	string randomizedFileName = inputFileName.substr(0, inputFileName.find('.')) + "_randomized.txt";	//->randomly delete
	string edgesFileName = inputFileName.substr(0, inputFileName.find('.')) + "_edges.txt";

	
	ifstream fin(inputFileName);
	ofstream frand(randomizedFileName);
	ofstream fedge(edgesFileName);
	if (!fin || !frand || !fedge)
	{
		cerr << "fstream error" << endl;
		exit(1);
	}


	
	//모든 Edge 읽어서 pair<int, int> vector에 저장
	//70% 남을 때까지 중간에 빼서 erase 후 EDGES_FILE에 저장
	typedef pair<int, int> Edge;
	vector<Edge> inEdgeList;
	vector<Edge> outEdgeList;

	int from, to;
	int edgeCnt = 0;
	while (fin >> from >> to)
	{
		inEdgeList.push_back(Edge({ from, to }));
		++edgeCnt;
	}


	int delEdgeNum = (int)(edgeCnt * 0.3);
	int randIdx;
	while (delEdgeNum > 0)
	{
		randIdx = rand() % inEdgeList.size();
		
		outEdgeList.push_back(inEdgeList[randIdx]);
		inEdgeList.erase(inEdgeList.begin() + randIdx);

		--delEdgeNum;
	}


	//randomized file 출력
	for (auto edge : inEdgeList)
	{
		frand << edge.first << "\t" << edge.second << endl;
	}

	//edge file 출력
	for (auto edge : outEdgeList)
	{
		fedge << edge.first << "\t" << edge.second << endl;
	}


	fin.close();
	frand.close();
	fedge.close();

	return 0;
}