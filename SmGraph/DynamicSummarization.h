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
			

			do {
				u = getRandomNodeInSubgraph(sg, origin, subgraph);	//any unchecked node in subgraph
				v = getRandomNodeInSubgraph(sg, origin, subgraph, u);	//any unchecked U2 in subraph


				if (u == -1)
				{
					noUExist = false;
					break;
				}

				if (v == -1)	//v is not exist
				{
					noU2Exist = true;
					break;
				}
			} while (checkedNodeList.find(u)!=enditer
				&& checkedNodeList.find(v)!=enditer);

			if (noUExist)	//u가 없을 경우
			{
				continue;
			}
			else if (noU2Exist)	//u에 해당하는 u2가 없을 경우 -> check u
			{
				checkedNodeList.emplace(u);
				continue;
			}

			
			result.clear();
			//summarize check
			if (getSummarizeRatio(sg, origin, u, v) > 0)
			{
				//do summarize

				auto* sNode = sg.get(u);
				auto* tNode = sg.get(v);
				auto& sEdges = sNode->getEdges();
				auto& tEdges = tNode->getEdges();

				NormalNode* srcNode = (NormalNode*)sNode;
				NormalNode* trgNode = (NormalNode*)tNode;

				//1. in same supernode
				//2. in diff supernode
				//3. supernode - normalnode
				//4. normalnode
				if (!sameParentAndId(srcNode) && !sameParentAndId(trgNode))	//1, 2
				{
					std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
					for (int nodeID : result)
						sg.add(new Edge(id2, nodeID));

					result.clear();
					std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
					for (int nodeID : result)
						sg.add(new Edge(id1, nodeID));


					//w, w2 추가
				}
				else if (!sameParentAndId(srcNode)) //3-1. src is in SuperNode
				{
					result.clear();
					std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
					for (int nodeID : result)
						sg.add(new Edge(id2, nodeID));
				}
				else if (!sameParentAndId(trgNode)) //3-2. trg is in SuperNode
				{
					std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
					for (int nodeID : result)
						sg.add(new Edge(id1, nodeID));
				}
				else //4
				{
					//

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
		//    auto* snode = sg.get(src);
		//    auto* tnode = sg.get(trg);
		//    int stype = snode->getType();
		//    int ttype = tnode->getType();
		//    if (stype == 1 && ttype == 1)
		//    {
		//        NormalNode* srcParticleNode = (NormalNode*)snode;
		//        NormalNode* trgParticleNode = (NormalNode*)tnode;
		//        if (srcParticleNode->getParent() == trgParticleNode->getParent())	//A와 B가 같은 슈퍼노드 안에 있을 때
		//        {
		//            srcParticleNode->addCorrectionTarget('+', trg);
		//            trgParticleNode->addCorrectionTarget('+', src);
		//        }
		//        else if (srcParticleNode->getParent() == src && trgParticleNode->getParent() == trg)	//A와 B가 일반노드일 때
		//        {
		//            sg.add(new Edge(src, trg));
		//        }
		//        else
		//        {
		//            //getCost(sg, origin, src, trg);
		//dynamicSummarize(sg, origin, src, trg);
		//        }
		//    }

		//1. add edge
		sg.add(new Edge(src, trg));

		//2. do summarize
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

