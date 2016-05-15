#define	INPUT_NAME	"com-amazon.ungraph"	

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
	string outputFileName = inputFileName;

	ifstream fin(inputFileName);
	
	if (!fin)
	{
		cerr << "fstream error" << endl;
		exit(1);
	}


	typedef pair<int, int> Edge;
	vector<Edge> inEdgeList;

	int from, to;
	int wholeEdgeCnt = 0;
	while (fin >> from >> to)
	{
		double val = rand() % 100;
		inEdgeList.push_back(Edge({ from, to }));


		++wholeEdgeCnt;
	}
	cout << "* wholeEdgeCnt: " << wholeEdgeCnt << endl;
	fin.close();


	ofstream fout(outputFileName);
	cout << "* file output start" << endl;
	for (auto edge : inEdgeList)	//randomized file Ãâ·Â
	{
		fout << edge.first << " " << edge.second << endl;
	}
	cout << "* file output end" << endl;
	fout.close();

	return 0;
}