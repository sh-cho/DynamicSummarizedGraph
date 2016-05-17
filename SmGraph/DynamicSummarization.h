#pragma once
#include "Graph.h"
#include "SummarizedStructure.hpp"
#include "random.hpp"

#include <unordered_set>
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
			const auto& edges = npp->getEdges();
			neighbors.resize(edges.size());
			const auto size = edges.size();
			for (register int i = 0; i < size; ++i)
				neighbors[i] = edges[i]->getTarget();
		}
		else
		{
			SuperNode* spn = (SuperNode*)sg.get(id);
			auto& snis = spn->getSummarizedNodeIds();
			for (auto sid : snis)
			{
				Node* p = original.get(sid);
				auto& edges = p->getEdges();
				neighbors.reserve(neighbors.size() + edges.size());
				for (auto*& e : edges)
					neighbors.emplace_back(e->getTarget());
			}
		}
		return move(neighbors);
	}

	static int getParent(Node* p)
	{
		NormalNode* np = (NormalNode*)p;
		return np->getParent();
	}

	static bool isSummarizedNode(NormalNode* np1)
	{
		return !(np1->getParent() == np1->getId());
	}

	//static void dynamicSummarize(Graph& sg, Graph& origin, int id1, int id2)
	//{
	//	//cost 계산 뒤 case 나눠서 요약
	//	auto& ns1 = getNeighborNodes(sg, origin, id1);
	//	auto& ns2 = getNeighborNodes(sg, origin, id2);
	//	//set<int> set1(ns1.begin(), ns1.end());
	//	//set<int> set2(ns2.begin(), ns2.end());
	//	
	//	set<int> subgraph;
	//	set<int> checkedNodeList;
	//	
	//	set<int>::iterator set_iter;
	//
	//
	//	bool noUExist, noU2Exist;
	//
	//
	//	//add subraph to A, A_2, B, B_2
	//	subgraph.emplace(id1);
	//	for (int nodeID : ns1)
	//		subgraph.emplace(nodeID);
	//	subgraph.emplace(id2);
	//	for (int nodeID : ns2)
	//		subgraph.emplace(nodeID);
	//
	//
	//	vector<Edge*> *snNeighbor = nullptr;
	//	Node *sNode, *tNode;
	//	NormalNode *srcNode, *trgNode;
	//	SuperNode *spU, *spV;
	//
	//	//전부 check될 때까지 반복
	//	while (subgraph.size() > checkedNodeList.size())
	//	{
	//		//get u, v
	//		int u, v;
	//		auto enditer = checkedNodeList.end();
	//		
	//		noUExist = false;
	//		noU2Exist = false;
	//		
	//
	//		//subgraph 안의 임의의 u, v를 get
	//		//u나 v가 checkNodeList에 있으면 다시 u, v를 받아옴
	//		do {
	//			u = getRandomNodeInSubgraph(sg, origin, subgraph);	//any unchecked node in subgraph
	//			v = getRandomNodeInSubgraph(sg, origin, subgraph, u);	//any unchecked U2 in subraph
	//
	//
	//			//debug
	//			//printf("u:%d, v:%d\n", u, v);
	//
	//
	//			if (u == -1)
	//			{
	//				noUExist = false;
	//				break;
	//			}
	//			else if (v == -1)	//v is not exist
	//			{
	//				noU2Exist = true;
	//				break;
	//			}
	//			else if (u == v)
	//			{
	//				continue;
	//			}
	//		} while (checkedNodeList.find(u)!=enditer
	//			&& checkedNodeList.find(v)!=enditer);
	//
	//		//u or v is not exist
	//		//if v is not exist -> check u
	//		if (noUExist)
	//		{
	//			continue;
	//		}
	//		else if (noU2Exist)
	//		{
	//			checkedNodeList.emplace(u);
	//			continue;
	//		}
	//
	//		//printf("u:%d, v:%d\n", u, v);
	//		
	//		sNode = sg.get(u);
	//		tNode = sg.get(v);
	//		srcNode = (NormalNode*)sNode;
	//		trgNode = (NormalNode*)tNode;
	//
	//		//임시. u 또는 v가 supernode일 때
	//		//checknodelist에 추가
	//		if (srcNode->getType() == SUPER_NODE)
	//		{
	//			checkedNodeList.emplace(u);
	//			continue;
	//		}
	//		else if (trgNode->getType() == SUPER_NODE)
	//		{
	//			checkedNodeList.emplace(v);
	//			continue;
	//		}
	//
	//
	//		//summarize check
	//		if (getSummarizeRatio(sg, origin, u, v) > 0)
	//		{
	//			//do summarize
	//			//1. in same supernode
	//			//2. in different supernode
	//			//3. supernode - normalnode
	//			//4. normalnode
	//			if (isSummarizedNode(srcNode) && isSummarizedNode(trgNode))	//1, 2
	//			{
	//				//이미 요약이 되어있기 때문에 checkedNodeList에 추가
	//				if (srcNode->getParent() == trgNode->getParent()) //in same supernode
	//				{
	//					//check v
	//					checkedNodeList.emplace(v);
	//					continue;
	//				}
	//				else //diff supernode
	//				{
	//					//SuperNode 합치기 -> A 기준으로 합침
	//					spU = (SuperNode*)sg.get(srcNode->getParent());
	//					spV = (SuperNode*)sg.get(trgNode->getParent());
	//					int spID = spU->getId();
	//					auto& vNodes = spV->getSummarizedNodeIds();
	//					for (auto node : vNodes)
	//					{
	//						spU->addSummarizedNode(node);
	//						((NormalNode*)sg.get(node))->setParent(spID);
	//					}
	//
	//
	//					//spV delete
	//					
	//					
	//
	//					//add new Supernode in subgraph
	//					subgraph.emplace(spID);
	//
	//
	//					//Supernode U의 이웃노드 체크 해제
	//					snNeighbor = &spU->getEdges();
	//					for (Edge* edge : *snNeighbor)
	//					{
	//						int neighborID = edge->getOther(spID);
	//
	//						//find and erase
	//						set_iter = checkedNodeList.find(neighborID);
	//						if (set_iter != checkedNodeList.end())
	//							checkedNodeList.erase(set_iter);
	//					}
	//				}
	//			}
	//			else if (isSummarizedNode(srcNode))	//3-1. U is exist in supernode
	//			{
	//				//SuperNode 하나로 합치기
	//				spU = (SuperNode*)sg.get(srcNode->getParent());
	//				int spID = spU->getId();
	//				spU->addSummarizedNode(trgNode->getId());
	//				trgNode->setParent(spID);
	//
	//
	//				//add new Supernode in subgraph
	//				subgraph.emplace(spID);
	//
	//				//이웃노드 체크 해제
	//				snNeighbor = &spU->getEdges();
	//				for (Edge* edge : *snNeighbor)
	//				{
	//					int neighborID = edge->getOther(spID);
	//
	//					//find and erase
	//					set_iter = checkedNodeList.find(neighborID);
	//					if (set_iter != checkedNodeList.end())
	//						checkedNodeList.erase(set_iter);
	//				}
	//			}
	//			else if (isSummarizedNode(trgNode))	//3-2. V is exist in supernode
	//			{
	//				//SuperNode 하나로 합치기
	//				spV = (SuperNode*)sg.get(trgNode->getParent());
	//				int spID = spV->getId();
	//				spV->addSummarizedNode(srcNode->getId());
	//				srcNode->setParent(spID);
	//
	//
	//				//add new Supernode in subgraph
	//				subgraph.emplace(spID);
	//
	//
	//				//이웃노드 체크 해제
	//				snNeighbor = &spV->getEdges();
	//				for (Edge* edge : *snNeighbor)
	//				{
	//					int neighborID = edge->getOther(spID);
	//
	//					//find and erase
	//					set_iter = checkedNodeList.find(neighborID);
	//					if (set_iter != checkedNodeList.end())
	//						checkedNodeList.erase(set_iter);
	//				}
	//			}
	//			else //4. 일반노드
	//			{
	//				//make new supernode
	//				SuperNode* spNew = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount())));
	//				int spID = spNew->getId();
	//
	//				spNew->addSummarizedNode(u);
	//				spNew->addSummarizedNode(v);	//superNode 추가 뒤 노드 추가
	//				srcNode->setParent(spID);
	//				trgNode->setParent(spID);
	//				
	//				
	//				
	//
	//				//add new Supernode in subgraph
	//				subgraph.emplace(spID);
	//
	//
	//				//이웃노드 체크 해제
	//				snNeighbor = &spNew->getEdges();
	//				for (Edge* edge : *snNeighbor)
	//				{
	//					int neighborID = edge->getOther(spID);
	//
	//					//find and erase
	//					set_iter = checkedNodeList.find(neighborID);
	//					if (set_iter != checkedNodeList.end())
	//						checkedNodeList.erase(set_iter);
	//				}
	//			}
	//
	//
	//
	//		}
	//		else
	//		{
	//			//correction table에 추가
	//			srcNode->addCorrectionTarget('+', v);
	//			trgNode->addCorrectionTarget('+', u);
	//
	//			checkedNodeList.emplace(u);
	//			checkedNodeList.emplace(v);
	//		}
	//	} //while
	//} //dynamicSummarize

	static void checkNormalNode(Graph& sg, Graph& origin, int src, int trg, double threshold)
	{
		//일반노드 두개 요약
		auto& sNbrs = getNeighborNodes(sg, origin, src);
		auto& tNbrs = getNeighborNodes(sg, origin, trg);

		Node *sNode, *tNode;
		NormalNode *snNode, *tnNode;
		
		unordered_set<int> checked;
		unordered_set<int> summarizedNodes;
		unordered_set<int> twohops;
		unordered_set<int>::iterator set_iter;

		int u;


		//1. source --- source's 2-hop node
		set_iter = twohops.begin();
		copy(tNbrs.begin(), tNbrs.end(), inserter(twohops, set_iter));	//twohops에 target node의 neighbor 복사
		u = src;
		while (1)
		{
			for (auto twohopNode : twohops)
			{
				if (checked.find(twohopNode) == checked.end())	//not found in checked list
				{
					//check cost
					//threshold와 비교
					if (getSummarizeRatio(sg, origin, u, twohopNode) > threshold)
					{
						//슈퍼노드로 요약
						SuperNode* spNew = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount())));
						int spID = spNew->getId();

						sNode = sg.get(u);
						tNode = sg.get(twohopNode);
						snNode = (NormalNode*)sNode;
						tnNode = (NormalNode*)tNode;

						spNew->addSummarizedNode(u);
						spNew->addSummarizedNode(twohopNode);
						snNode->setParent(spID);
						tnNode->setParent(spID);

						//printf("u:%d, v:%d\n", u, twohopNode);

						//push summarized node's neighbor to twohops
						//이 때 checked list에 없는지 확인
						auto& suNbrs = getNeighborNodes(sg, origin, twohopNode);
						for (auto neighbor : suNbrs)
						{
							if (checked.find(neighbor) == checked.end() && u != neighbor)	//not found
							{
								twohops.emplace(neighbor);
							}
						}

						summarizedNodes.emplace(twohopNode);
					}
					else
					{
						//check
						checked.emplace(twohopNode);
					}
				}
			}


			if (summarizedNodes.size() != 0)
			{
				checked.emplace(u);

				//u 변경
				//summarized에서 하나 빼서 사용
				set_iter = summarizedNodes.begin();
				advance(set_iter, 0);
				u = (*set_iter);
				summarizedNodes.erase(set_iter);

				auto& newNbrs = getNeighborNodes(sg, origin, u);
				twohops.clear();

				for (auto twohop : newNbrs)
				{
					if (checked.find(twohop) == checked.end() && u != twohop)	//not found
					{
						twohops.emplace(twohop);
					}
				}
			}
			else //not summarized or 요약노드 다 봤을 때
			{
				break;
			}
		}



		//////////////////////////////////////////////////////////////////
		//2. target --- target's 2-hop node
		checked.clear();
		summarizedNodes.clear();
		twohops.clear();
		set_iter = twohops.begin();
		copy(sNbrs.begin(), sNbrs.end(), inserter(twohops, set_iter));
		u = trg;
		while (1)
		{
			for (auto twohopNode : twohops)
			{
				if (checked.find(twohopNode) == checked.end())	//not found in checked list
				{
					//check cost
					//threshold와 비교
					if (getSummarizeRatio(sg, origin, u, twohopNode) > threshold)
					{
						//슈퍼노드로 요약
						SuperNode* spNew = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount())));
						int spID = spNew->getId();

						sNode = sg.get(u);
						tNode = sg.get(twohopNode);
						snNode = (NormalNode*)sNode;
						tnNode = (NormalNode*)tNode;

						spNew->addSummarizedNode(u);
						spNew->addSummarizedNode(twohopNode);
						snNode->setParent(spID);
						tnNode->setParent(spID);

						//printf("u:%d, v:%d\n", u, twohopNode);

						//push summarized node's neighbor to twohops
						//이 때 checked list에 없는지 확인
						auto& suNbrs = getNeighborNodes(sg, origin, twohopNode);
						for (auto neighbor : suNbrs)
						{
							if (checked.find(neighbor) == checked.end() && u != neighbor)	//not found
							{
								twohops.emplace(neighbor);
							}
						}

						summarizedNodes.emplace(twohopNode);
					}
					else
					{
						//check
						checked.emplace(twohopNode);
					}
				}
			}


			if (summarizedNodes.size() != 0)
			{
				checked.emplace(u);
				//u 변경
				//summarized에서 하나 빼서 사용
				set_iter = summarizedNodes.begin();
				advance(set_iter, 0);
				u = (*set_iter);
				summarizedNodes.erase(set_iter);

				auto& newNbrs = getNeighborNodes(sg, origin, u);
				twohops.clear();

				for (auto twohop : newNbrs)
				{
					if (checked.find(twohop) == checked.end() && u != twohop)	//not found
					{
						twohops.emplace(twohop);
					}
				}
			}
			else //not summarized
			{
				break;
			}
		}
	}

	static void addEdgeAndSummarize(Graph& sg, Graph& origin, int src, int trg, double threshold)
	{
		//double threshold = 0.7;

		//add edge
		//sg.add(new Edge(src, trg));
		auto* sNode = sg.get(src);
		auto* tNode = sg.get(trg);
		int sType = sNode->getType(),
			tType = tNode->getType();
		NormalNode *snNode = (NormalNode*)sNode,
					*tnNode = (NormalNode*)tNode;

		if (sType == NORMAL_NODE && tType == NORMAL_NODE)
		{
			if (snNode->getParent() == tnNode->getParent())	//in same supernode
			{
				snNode->addCorrectionTarget('+', trg);
				tnNode->addCorrectionTarget('+', src);
			}
			else if (snNode->getParent() == src && tnNode->getParent() == trg)	//일반노드 - 일반노드
			{
				checkNormalNode(sg, origin, src, trg, threshold);
			}
			else //different supernode
			{
				auto& ns1 = getNeighborNodes(sg, origin, src);
				auto& ns2 = getNeighborNodes(sg, origin, trg);
				int total = ns1.size() + ns2.size();
				set<int> set1(ns1.begin(), ns1.end());
				set<int> set2(ns2.begin(), ns2.end());
				vector<int> result;


				//
				if (getSummarizeRatio(sg, origin, src, trg) > threshold)
				{
					auto& p1_items = sNode->getEdges();
					auto& p2_items = tNode->getEdges();
					vector<int> result;

					if (isSummarizedNode(snNode) && isSummarizedNode(tnNode))	//각자 다른 슈퍼노드에 속한 경우
					{
						//-, + 교정 갯수 세서 케이스 나누기
						//if C+ > C-
						//	;
						//else
						//	;
						

						result.clear();
						set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
						for (int id : result) {
							sg.add(new Edge(trg, id));
						}

						result.clear();
						set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
						for (int id : result) {
							sg.add(new Edge(src, id));
						}
					}
					else if (isSummarizedNode(snNode))	//슈퍼노드(src) --- 일반노드(trg)
					{
						result.clear();
						set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
						for (int id : result) {
							sg.add(new Edge(trg, id));
						}
					}
					else if (isSummarizedNode(tnNode))	//일반노드(src) --- 슈퍼노드(trg)
					{
						result.clear();
						set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
						for (int id : result) {
							sg.add(new Edge(src, id));
						}
					}
				}
				else //요약 안되는 경우
				{
					//add + correction
					snNode->addCorrectionTarget('+', trg);
					tnNode->addCorrectionTarget('+', src);
				}
			}
		}
	}

	static double getSummarizeRatio(Graph& sg, Graph& origin, int id1, int id2)
	{
		//set -> 배열로 바꿈

		int size1,
			size2,	//1, 2의 크기
			sizeInter = 0;	//교집합 크기
		int i = 0, j = 0;	//index

		vector<int>& ns1 = getNeighborNodes(sg, origin, id1);
		vector<int>& ns2 = getNeighborNodes(sg, origin, id2);

		std::sort(ns1.begin(), ns1.end());
		std::sort(ns2.begin(), ns2.end());

		size1 = ns1.size();
		size2 = ns2.size();


		//vector 두개 index를 이용해 교집합 크기 구함
		while (i < size1 && j < size2)
		{
			if (ns1[i] == ns2[j])
			{
				++sizeInter;
				++i;
				++j;
			}
			else if (ns1[i] > ns2[j])
			{
				++j;
			}
			else
			{
				++i;
			}
		}


		//set<int> set1(ns1.begin(), ns1.end());
		//set<int> set2(ns2.begin(), ns2.end());
		//vector<int> interSection;
		//set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(interSection));

		//int c_w = (int)set1.size() + (int)set2.size() - (int)interSection.size();
		////cout << "ratio: " << (double)(set1.size() + set2.size() - c_w) / (set1.size() + set2.size()) << endl;
		//return (double)(set1.size() + set2.size() - c_w) / (set1.size() + set2.size());

		//
		return (double)(sizeInter) / (size1 + size2);
	}

	//static int getRandomNodeInSubgraph(Graph& sg, Graph& origin, set<int> subgraph, int u = -1)
	//{
	//	//if u == -1 (no input u)
	//	//  get random u
	//	//else
	//	//  get u's 2-hop node(u2) in subgraph
	//	int nodeID = -1;	//-1: Not found
	//	set<int>::iterator it = subgraph.begin();
	//
	//	if (u == -1)	//get random u node
	//	{
	//		advance(it, random::Int(0, (int)subgraph.size()-1));
	//		nodeID = (*it);
	//	}
	//	else	//get random u2 node
	//	{
	//		auto& neighborNodes = getNeighborNodes(sg, origin, u);
	//		if (neighborNodes.size() == 0)
	//			return nodeID;
	//		vector<int>::iterator vecit = neighborNodes.begin();
	//		
	//		advance(vecit, random::Int(0, (int)neighborNodes.size()-1));
	//		auto& twohopNodes = getNeighborNodes(sg, origin, *vecit);
	//		if (twohopNodes.size() == 0)
	//			return nodeID;
	//
	//		vecit = twohopNodes.begin();
	//		advance(vecit, random::Int(0, (int)twohopNodes.size() - 1));
	//		if (vecit != std::end(twohopNodes) && subgraph.find(*vecit) != subgraph.end())
	//			nodeID = (*vecit);
	//	}
	//
	//
	//	return nodeID;
	//}

	//static void countEdges(Graph& sg, Graph& origin)
	//{
	//	int sgCnt = 0,
	//		origCnt = 0;
	//
	//	auto sNodes = sg.getNodes();
	//	auto oNodes = origin.getNodes();
	//
	//	for (auto node : sNodes)
	//	{
	//		if (!node)
	//			continue;
	//		sgCnt += node->getEdges().size();
	//		
	//		//pluscorr
	//		if (node->getType() == SUPER_NODE)
	//		{
	//			SuperNode* sp = (SuperNode*)node;
	//			sgCnt += sp->getEdges().size();
	//		}
	//	}
	//
	//	for (auto node : oNodes)
	//	{
	//		if (!node)
	//			continue;
	//		origCnt += node->getEdges().size();
	//	}
	//
	//	//printf("sg:%d, orig:%d\n", sgCnt/2, origCnt/2);
	//}
};

