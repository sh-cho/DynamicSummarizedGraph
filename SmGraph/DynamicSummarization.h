#pragma once
#include "Graph.h"
#include "SummarizedStructure.hpp"
#include "random.hpp"

class DynamicSummarization
{
public:
	static auto getNeighborNodes(Graph& sg, Graph& original, int id)
	{
		auto* np = sg.get(id);
		vector<int> neighbors;
		if ((unsigned)id < original.getNodeCount())
		{
			NormalNode* npp = (NormalNode*)original.get(id);
			auto& edges = npp->getEdges();
			for (Edge* e : edges)
				neighbors.emplace_back(e->getTarget());
		}
		else
		{
			SuperNode* spn = (SuperNode*)sg.get(id);
			auto& snis = spn->getSummarizedNodeIds();
			for (auto sid : snis)
			{
				Node* p = original.get(sid);
				auto& edges = p->getEdges();
				for (auto* e : edges)
					neighbors.emplace_back(e->getTarget());
			}
		}
		return neighbors;
	}

	static int getParent(Node* p)
	{
		NormalNode* np = (NormalNode*)p;
		return np->getParent();
	}

	static bool sameParentAndId(NormalNode* np1)
	{
		return np1->getParent() == np1->getId();
	}

	static void dynamicSummarize(Graph& sg, Graph& origin, int id1, int id2)
	{
		//cost 계산 뒤 case 나눠서 요약
		auto& ns1 = getNeighborNodes(sg, origin, id1);
		auto& ns2 = getNeighborNodes(sg, origin, id2);
		set<int> set1(ns1.begin(), ns1.end());
		set<int> set2(ns2.begin(), ns2.end());
		
		set<int> subgraph;
		set<int> checkedNodeList;


		//add subraph to A, A_2, B, B_2
		subgraph.emplace(id1);
		for (int nodeID : ns1)
			subgraph.emplace(nodeID);
		subgraph.emplace(id2);
		for (int nodeID : ns2)
			subgraph.emplace(nodeID);



		vector<int> result;
		//전부 check될 때까지 반복
		while (subgraph.size() > checkedNodeList.size())
		{
			//get u, v
			int u, v;
			auto enditer = checkedNodeList.end();
			
			bool noUExist = false,
				noU2Exist = false;
			

			//subgraph 안의 임의의 u, v를 get
			//u나 v가 checkNodeList에 있으면 다시 u, v를 받아옴
			do {
				u = getRandomNodeInSubgraph(sg, origin, subgraph);	//any unchecked node in subgraph
				v = getRandomNodeInSubgraph(sg, origin, subgraph, u);	//any unchecked U2 in subraph


				//debug
				printf("u:%d, v:%d\n", u, v);


				if (u == -1)
				{
					noUExist = false;
					break;
				}
				else if (v == -1)	//v is not exist
				{
					noU2Exist = true;
					break;
				}
			} while (checkedNodeList.find(u)!=enditer
				&& checkedNodeList.find(v)!=enditer);

			//u or v is not exist
			//if v is not exist -> check u
			if (noUExist)
			{
				continue;
			}
			else if (noU2Exist)
			{
				checkedNodeList.emplace(u);
				continue;
			}


			//summarize check
			result.clear();
			if (getSummarizeRatio(sg, origin, u, v) > 0)
			{
				//do summarize
				auto* sNode = sg.get(u);
				auto* tNode = sg.get(v);
				auto& sEdges = sNode->getEdges();
				auto& tEdges = tNode->getEdges();

				NormalNode* srcNode = (NormalNode*)sNode;
				NormalNode* trgNode = (NormalNode*)tNode;

				//임시. u 또는 v가 supernode일 때
				//checknodelist에 추가
				if (srcNode->getType() == SUPER_NODE)
				{
					checkedNodeList.emplace(u);
					continue;
				}
				else if (trgNode->getType() == SUPER_NODE)
				{
					checkedNodeList.emplace(v);
					continue;
				}



				//요약되는지 확인
				//1. in same supernode
				//2. in different supernode
				//3. supernode - normalnode
				//4. normalnode
				if (!sameParentAndId(srcNode) && !sameParentAndId(trgNode))	//1, 2
				{
					//이미 요약이 되어있기 때문에 checkedNodeList에 추가
					if (srcNode->getParent() == trgNode->getParent()) //in same supernode
					{
						//check v
						checkedNodeList.emplace(v);
						continue;
					}
					else //diff supernode
					{
						//SuperNode 합치기
						SuperNode* sp = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount() + 1)));
						sp->addSummarizedNode(u);
						sp->addSummarizedNode(v);	//superNode 추가 뒤 노드 추가
						auto& spNeighbor = sp->getEdges();
						int spID = sp->getId();



						//add in subgraph
						subgraph.emplace(spID);


						//이웃노드 체크 해제
						set<int>::iterator setIter;
						auto& snNeighbor = sp->getEdges();
						for (Edge* edge : snNeighbor)
						{
							int neighborID = edge->getOther(spID);

							//find and erase
							setIter = checkedNodeList.find(neighborID);
							if (setIter != checkedNodeList.end())
								checkedNodeList.erase(setIter);
						}
					}
				}
				else if (!sameParentAndId(srcNode))	//3-1. U is exist in supernode
				{
					//SuperNode 하나로 합치기
					SuperNode* sp = (SuperNode*)sg.get(srcNode->getParent());
					sp->addSummarizedNode(trgNode->getId());
					auto& spNeighbor = sp->getEdges();
					int spID = sp->getId();


					//add in subgraph
					subgraph.emplace(spID);

					//이웃노드 체크 해제
					set<int>::iterator setIter;
					auto& snNeighbor = sp->getEdges();
					for (Edge* edge : snNeighbor)
					{
						int neighborID = edge->getOther(spID);

						//find and erase
						setIter = checkedNodeList.find(neighborID);
						if (setIter != checkedNodeList.end())
							checkedNodeList.erase(setIter);
					}
				}
				else if (!sameParentAndId(trgNode))	//3-2. V is exist in supernode
				{
					//SuperNode 하나로 합치기
					SuperNode* sp = (SuperNode*)sg.get(trgNode->getParent());
					sp->addSummarizedNode(srcNode->getId());
					auto& spNeighbor = sp->getEdges();
					int spID = sp->getId();


					//add in subgraph
					subgraph.emplace(spID);


					//이웃노드 체크 해제
					set<int>::iterator setIter;
					auto& snNeighbor = sp->getEdges();
					for (Edge* edge : snNeighbor)
					{
						int neighborID = edge->getOther(spID);

						//find and erase
						setIter = checkedNodeList.find(neighborID);
						if (setIter != checkedNodeList.end())
							checkedNodeList.erase(setIter);
					}
				}
				else //4. 일반노드
				{
					//make new supernode
					SuperNode* sp = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount() + 1)));
					sp->addSummarizedNode(u);
					sp->addSummarizedNode(v);	//superNode 추가 뒤 노드 추가
					auto& spNeighbor = sp->getEdges();
					int spID = sp->getId();
					

					//add in subgraph
					subgraph.emplace(spID);


					//이웃노드 체크 해제
					set<int>::iterator setIter;
					auto& snNeighbor = sp->getEdges();
					for (Edge* edge : snNeighbor)
					{
						int neighborID = edge->getOther(spID);

						//find and erase
						setIter = checkedNodeList.find(neighborID);
						if (setIter != checkedNodeList.end())
							checkedNodeList.erase(setIter);
					}
				}
			}
			else
			{
				checkedNodeList.emplace(v);
			}
		} //while
	} //dynamicSummarize

	static void addEdgeAndSummarize(Graph& sg, Graph& origin, int src, int trg)
	{
		//1. add edge
		sg.add(new Edge(src, trg));

		//2. if nodes are exist in same supernode
		auto* sNode = sg.get(src);
		auto* tNode = sg.get(trg);
		if (((NormalNode*)sNode)->getParent() == ((NormalNode*)tNode)->getParent())
			return;

		//3. do summarize
		dynamicSummarize(sg, origin, src, trg);
	}

	static double getSummarizeRatio(Graph& sg, Graph& origin, int id1, int id2)
	{
		//return s(id1, id2)
		
		auto& ns1 = getNeighborNodes(sg, origin, id1);
		auto& ns2 = getNeighborNodes(sg, origin, id2);
		int total = (int)ns1.size() + (int)ns2.size();
		set<int> set1(ns1.begin(), ns1.end());
		set<int> set2(ns2.begin(), ns2.end());
		vector<int> interSection;
		set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(interSection));

		//debug
		cout << "ratio:" << (double)(set1.size() + set2.size() - interSection.size()) / (total) << endl;

		return (double)(set1.size() + set2.size() - interSection.size())/(total);
	}

	static int getRandomNodeInSubgraph(Graph& sg, Graph& origin, set<int> subgraph, int u = -1)
	{
		//if u == -1 (no input u)
		//  get random u
		//else
		//  get u's 2-hop node(u2) in subgraph


		int nodeID = -1;	//-1: Not found
		set<int>::iterator it = subgraph.begin();

		if (u == -1)	//get random u node
		{
			advance(it, random::Int(0, (int)subgraph.size()-1));

			if (it != subgraph.end())
				nodeID = *it;
			//else //not found
			//	nodeID = -1;
		}
		else	//get u2 node
		{
			auto& neighborNodes = getNeighborNodes(sg, origin, u);
			for (int neighborID : neighborNodes)
			{
				if (subgraph.find(neighborID) != subgraph.end())
				{
					nodeID = neighborID;
					break;
				}
			}
		}


		return nodeID;
	}
};

