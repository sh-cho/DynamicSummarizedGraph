/**
 *	Graph ���� �о 70%�� Edge�� ���ܼ� ���ο� Graph ���Ϸ� ����(~~~_randomized.txt)
 *	30%�� edge�� ���� ���Ϸ� ����(~~~_edges.txt)
 *
 *	INPUT_NAME�� .txt Ȯ���� ������ �̸� �Է�
 *	���Ͽ� �ּ� ���� �������
 *	ex) #define INPUT_NAME "dataset/facebook_combined"
 *
 *	ERASE_RATE�� ���� ���� (0 ~ 1)
 *	0.01 ���������� ������
 *		-> approximate�� ����
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
	
	//��� Edge �о pair<int, int> vector�� ����
	//70% ���� ������ �߰��� ���� erase �� EDGES_FILE�� ����
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
	for (auto edge : inEdgeList)	//randomized file ���
	{
		frand << edge.first << " " << edge.second << endl;
	}

	//edge random shuffle �� ���
	std::random_shuffle(outEdgeList.begin(), outEdgeList.end(), myrandom);
	for (auto edge : outEdgeList)	//edge file ���
	{
		fedge << edge.first << " " << edge.second << endl;
	}
	cout << "* file output end" << endl;


	fin.close();
	frand.close();
	fedge.close();

	return 0;
}