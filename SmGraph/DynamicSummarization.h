#pragma once
#include "Graph.h"
#include "SummarizedStructure.hpp"

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
	static double cham_go_yong(Graph& sg, Graph& origin, int id1, int id2)
	{
		//참고용
		auto& ns1 = getNeighborNodes(sg, origin, id1);
		auto& ns2 = getNeighborNodes(sg, origin, id2);
		int total = ns1.size() + ns2.size();
		set<int> set1(ns1.begin(), ns1.end());
		set<int> set2(ns2.begin(), ns2.end());
		vector<int> result;
		set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
		double case1_cost = (double)(result.size() + set1.size() - result.size() + set2.size() - result.size()) / total;
		static double advance_count = 0;
		advance_count += (1 - case1_cost) * total;
		cout << "====>advance_count:" << advance_count << endl;
		//모두 +로 묵는경우
		if (1 - case1_cost > 0)
		{
			auto* p1 = sg.get(id1);
			auto* p2 = sg.get(id2);
			auto& p1_items = p1->getEdges();
			auto& p2_items = p2->getEdges();
			NormalNode* np1 = (NormalNode*)p1;
			NormalNode* np2 = (NormalNode*)p2;
			if (!sameParentAndId(np1) && !sameParentAndId(np2))
			{
				result.clear();
				set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
				for (int id : result) {
					sg.add(new Edge(id2, id));
				}
				result.clear();
				set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
				for (int id : result) {
					sg.add(new Edge(id1, id));
				}
			}
			else if (!sameParentAndId(np1))
			{
				result.clear();
				set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
				for (int id : result) {
					sg.add(new Edge(id2, id));
				}
			}
			else if (!sameParentAndId(np2))
			{
				result.clear();
				set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
				for (int id : result) {
					sg.add(new Edge(id1, id));
				}
			}
		}
		return 1;
	}
	static void dynamicSummarize(Graph& sg, Graph& origin, int id1, int id2)
	{
		//cost 계산 뒤 case 나눠서 요약
	}
	static void addEdge(Graph& sg, Graph& origin, int src, int trg)
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
};

