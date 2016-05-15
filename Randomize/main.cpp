/**
 *	Graph ���� �о 70%�� Edge�� ���ܼ� ���ο� Graph ���Ϸ� ����
 *	30%�� edge�� ���� ���Ϸ� ����
 *
 *	INPUT_NAME�� .txt Ȯ���� ������ �̸� �Է�
 *	���Ͽ� �ּ� ���� �������
 *	ex) #define INPUT_NAME "dataset/facebook_combined"
 *
 *	ERASE_RATE�� ���� ���� (0 ~ 1)
 *	0.01 ���������� ������
 */

#define	INPUT_NAME	"dataset/com-dblp.ungraph"	
#define	ERASE_RATE	0.1

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
	string randomizedFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + "_randomized.txt";	//->randomly delete
	string edgesFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + "_edges.txt";

	
	ifstream fin(inputFileName);
	ofstream frand(randomizedFileName);
	ofstream fedge(edgesFileName);
	if (!fin || !frand || !fedge)
	{
		cerr << "fstream error" << endl;
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
		frand << edge.first << "\t" << edge.second << endl;
	}

	for (auto edge : outEdgeList)	//edge file ���
	{
		fedge << edge.first << "\t" << edge.second << endl;
	}
	cout << "* file output end" << endl;


	fin.close();
	frand.close();
	fedge.close();

	return 0;
}