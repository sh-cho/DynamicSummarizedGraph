/**
 *	Graph 파일 읽어서 70%의 Edge만 남겨서 새로운 Graph 파일로 저장(~~~_randomized.txt)
 *	30%의 edge는 따로 파일로 저장(~~~_edges.txt)
 *
 *	INPUT_NAME에 .txt 확장자 제외한 이름 입력
 *	파일에 주석 등이 없어야함
 *	ex) #define INPUT_NAME "dataset/facebook_combined"
 *
 *	ERASE_RATE는 지울 비율 (0 ~ 1)
 *	0.01 단위까지만 지원함
 *		-> approximate한 비율
 */

#define	INPUT_NAME	"dataset/facebook_combined"
#define	ERASE_RATE	0.2


#include <algorithm>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>	//std::pair
#include <vector>
using namespace std;


int myrandom(int i) { return std::rand() % i; }

int main()
{
	srand((unsigned)time(nullptr));

	string inputFileName = INPUT_NAME + string(".txt");
	//string inputFileName = INPUT_NAME + string(".txt.txr");	//txr version
	string randomizedFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + "_randomized20p.txt";	//->randomly delete
	string edgesFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + "_edges20p.txt";

	
	ifstream fin(inputFileName);
	ofstream frand(randomizedFileName);
	ofstream fedge(edgesFileName);
	if (!fin || !frand || !fedge)
	{
		cerr << "fstream error" << endl;
		exit(1);
	}

	
	if (ERASE_RATE >= 1 || ERASE_RATE <= 0)
	{
		cerr << "erase rate error" << endl;
		exit(1);
	}

	cout << "* Start randomly delete " << ERASE_RATE*100 << "% edge" << endl;
	
	//모든 Edge 읽어서 pair<int, int> vector에 저장
	//70% 남을 때까지 중간에 빼서 erase 후 EDGES_FILE에 저장
	typedef pair<int, int> Edge;
	vector<Edge> inEdgeList;
	vector<Edge> outEdgeList;

	int from, to;
	int wholeEdgeCnt = 0, delCnt = 0;
	while (fin >> from >> to)
	{
		double val = rand() % 100;
		if (val < ERASE_RATE * 100)
		{
			outEdgeList.push_back(Edge({ from, to }));
			++delCnt;
		}
		else
		{
			inEdgeList.push_back(Edge({ from, to }));
		}

		++wholeEdgeCnt;
	}
	cout << "* wholeEdgeCnt: " << wholeEdgeCnt << endl;
	cout << "* randomized Edge Count: " << wholeEdgeCnt - delCnt << endl;
	cout << "* deleteCnt: " << delCnt << endl;

	/*int delEdgeNum = (int)(edgeCnt * 0.3);
	int randIdx;

	cout << "* delEdgeNum: " << delEdgeNum << endl;
	while (delEdgeNum > 0)
	{
		randIdx = rand() % inEdgeList.size();
		
		outEdgeList.push_back(inEdgeList[randIdx]);
		inEdgeList.erase(inEdgeList.begin() + randIdx);

		--delEdgeNum;
	}*/


	cout << "* file output start" << endl;
	for (auto edge : inEdgeList)	//randomized file 출력
	{
		frand << edge.first << " " << edge.second << endl;
	}

	//edge random shuffle 후 출력
	std::random_shuffle(outEdgeList.begin(), outEdgeList.end(), myrandom);
	for (auto edge : outEdgeList)	//edge file 출력
	{
		fedge << edge.first << " " << edge.second << endl;
	}
	cout << "* file output end" << endl;


	fin.close();
	frand.close();
	fedge.close();

	return 0;
}