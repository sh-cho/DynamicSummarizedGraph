#pragma once
#include "Graph.h"
#include "SummarizedStructure.hpp"
//#include "random.hpp"

class DynamicSummarization
{
public:
	static auto getNeighborNodes(Graph& sg, Graph& original, int id)
	{
		//auto* np = sg.get(id);
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

	static void checkNormalNode(Graph& sg, Graph& origin, int src, int trg, double threshold)
	{
		//일반노드 두개 요약
		auto& sNbrs = getNeighborNodes(sg, origin, src);
		auto& tNbrs = getNeighborNodes(sg, origin, trg);
		std::sort(sNbrs.begin(), sNbrs.end());
		std::sort(tNbrs.begin(), tNbrs.end());


		Node *sNode, *tNode;
		NormalNode *snNode, *tnNode;
		
		vector<int> checked;
		vector<int> summarizedNodes;
		vector<int> twohops;
		

		int u;

		//////////////////////////////////////////////////////////////////
		//1. source --- source's 2-hop node
		///set_iter = twohops.begin();
		///copy(tNbrs.begin(), tNbrs.end(), inserter(twohops, set_iter));	//twohops에 target node의 neighbor 복사

		//두 배열을 한 배열로 복사(중복 제거)
		/*twohops.assign(sNbrs.begin(), sNbrs.end());
		for (int id : tNbrs)
		{
			if (find(twohops.begin(), twohops.end(), id) == twohops.end())
			{
				twohops.push_back(id);
			}
		}*/
		register int i = 0, j = 0;
		int size1, size2;
		size1 = (int)sNbrs.size();
		size2 = (int)tNbrs.size();
		while (i < size1 && j < size2)
		{
			if (sNbrs[i] == tNbrs[j])
			{
				twohops.push_back(sNbrs[i]);
				++i;
				++j;
			}
			else if (sNbrs[i] > tNbrs[j])
			{
				twohops.push_back(tNbrs[j]);
				++j;
			}
			else
			{
				twohops.push_back(sNbrs[i]);
				++i;
			}
		}

		
		u = src;
		while (1)
		{
			for (auto twohopNode : twohops)
			{
				if (find(checked.begin(), checked.end(), twohopNode) == checked.end())	//not found in checked list
				{
					//check cost
					//threshold와 비교
					if (getSummarizeRatio(sg, origin, u, twohopNode) > threshold)
					{
						sNode = sg.get(u);
						tNode = sg.get(twohopNode);
						snNode = (NormalNode*)sNode;
						tnNode = (NormalNode*)tNode;
						
						
						//슈퍼노드로 요약
						//1. u가 superNode 안에 있을 경우 -> twohopNode를 u 안에 넣기
						//2. 이외의 경우 -> 새 슈퍼노드 만들기
						if (isSummarizedNode(snNode)) //1.
						{
							SuperNode* spU = (SuperNode*)sg.get(snNode->getParent());
							int spID = spU->getId();

							spU->addSummarizedNode(twohopNode);
							tnNode->setParent(spID);
						}
						else //2.
						{
							SuperNode* spNew = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount())));
							int spID = spNew->getId();

							spNew->addSummarizedNode(u);
							spNew->addSummarizedNode(twohopNode);
							snNode->setParent(spID);
							tnNode->setParent(spID);
						}



						//printf("u:%d, v:%d\n", u, twohopNode);

						//push summarized node's neighbor to twohops
						//이 때 checked list에 없는지 확인
						auto& suNbrs = getNeighborNodes(sg, origin, twohopNode);
						for (auto neighbor : suNbrs)
						{
							if (find(checked.begin(), checked.end(), neighbor) == checked.end() && u != neighbor && find(twohops.begin(), twohops.end(), neighbor) == twohops.end())	//not found
							{
								twohops.push_back(neighbor);
							}
						}

						
						//if not found on vector
						if (std::find(summarizedNodes.begin(), summarizedNodes.end(), twohopNode) == summarizedNodes.end())
						{
							summarizedNodes.push_back(twohopNode);
						}
					}
					else
					{
						//check
						if (find(checked.begin(), checked.end(), twohopNode) == checked.end())
						{
							checked.push_back(twohopNode);
						}
					}
				}
			}


			if (summarizedNodes.size() != 0)
			{
				if (find(checked.begin(), checked.end(), u) == checked.end())
				{
					checked.push_back(u);
				}

				//u 변경
				//summarized에서 하나 빼서 사용
				u = summarizedNodes.back();
				summarizedNodes.pop_back();

				auto& newNbrs = getNeighborNodes(sg, origin, u);
				twohops.clear();

				for (auto twohop : newNbrs)
				{
					if (find(checked.begin(), checked.end(), twohop) == checked.end() && u != twohop && find(twohops.begin(), twohops.end(), twohop) == twohops.end())	//not found
					{
						twohops.push_back(twohop);
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
		//set_iter = twohops.begin();
		//copy(sNbrs.begin(), sNbrs.end(), inserter(twohops, set_iter));
		//두 배열을 한 배열로 복사(중복 제거)
		/*twohops.assign(sNbrs.begin(), sNbrs.end());
		for (int id : tNbrs)
		{
			if (find(twohops.begin(), twohops.end(), id) == twohops.end())
			{
				twohops.push_back(id);
			}
		}*/
		i = 0, j = 0;
		while (i < size1 && j < size2)
		{
			if (sNbrs[i] == tNbrs[j])
			{
				twohops.push_back(sNbrs[i]);
				++i;
				++j;
			}
			else if (sNbrs[i] > tNbrs[j])
			{
				twohops.push_back(tNbrs[j]);
				++j;
			}
			else
			{
				twohops.push_back(sNbrs[i]);
				++i;
			}
		}
		u = trg;
		while (1)
		{
			for (auto twohopNode : twohops)
			{
				if (find(checked.begin(), checked.end(), twohopNode) == checked.end())	//not found in checked list
				{
					//check cost
					//threshold와 비교
					if (getSummarizeRatio(sg, origin, u, twohopNode) > threshold)
					{
						sNode = sg.get(u);
						tNode = sg.get(twohopNode);
						snNode = (NormalNode*)sNode;
						tnNode = (NormalNode*)tNode;


						//슈퍼노드로 요약
						//1. u가 superNode 안에 있을 경우 -> twohopNode를 u 안에 넣기
						//2. 이외의 경우 -> 새 슈퍼노드 만들기
						if (isSummarizedNode(snNode)) //1.
						{
							SuperNode* spU = (SuperNode*)sg.get(snNode->getParent());
							int spID = spU->getId();

							spU->addSummarizedNode(twohopNode);
							tnNode->setParent(spID);
						}
						else //2.
						{
							SuperNode* spNew = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount())));
							int spID = spNew->getId();

							spNew->addSummarizedNode(u);
							spNew->addSummarizedNode(twohopNode);
							snNode->setParent(spID);
							tnNode->setParent(spID);
						}

						//printf("u:%d, v:%d\n", u, twohopNode);

						//push summarized node's neighbor to twohops
						//이 때 checked list에 없는지 확인
						auto& suNbrs = getNeighborNodes(sg, origin, twohopNode);
						for (auto neighbor : suNbrs)
						{
							if (find(checked.begin(), checked.end(), neighbor) == checked.end() && u != neighbor && find(twohops.begin(), twohops.end(), neighbor) == twohops.end())	//not found
							{
								twohops.push_back(neighbor);
							}
						}

						//if not found on vector
						if (std::find(summarizedNodes.begin(), summarizedNodes.end(), twohopNode) == summarizedNodes.end())
						{
							summarizedNodes.push_back(twohopNode);
						}
					}
					else
					{
						//check
						if (find(checked.begin(), checked.end(), twohopNode) == checked.end())
						{
							checked.push_back(twohopNode);
						}
					}
				}
			}


			if (summarizedNodes.size() != 0)
			{
				if (find(checked.begin(), checked.end(), u) == checked.end())
				{
					checked.push_back(u);
				}

				//u 변경
				//summarized에서 하나 빼서 사용
				u = summarizedNodes.back();
				summarizedNodes.pop_back();

				auto& newNbrs = getNeighborNodes(sg, origin, u);
				twohops.clear();

				for (auto twohop : newNbrs)
				{
					if (find(checked.begin(), checked.end(), twohop) == checked.end() && u != twohop && find(twohops.begin(), twohops.end(), twohop) == twohops.end())	//not found
					{
						twohops.push_back(twohop);
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
			//else
			//{
			//	auto& ns1 = getNeighborNodes(sg, origin, src);
			//	auto& ns2 = getNeighborNodes(sg, origin, trg);
			//	int total = (int)ns1.size() + (int)ns2.size();
			//	unordered_set<int> set1(ns1.begin(), ns1.end());
			//	unordered_set<int> set2(ns2.begin(), ns2.end());
			//	vector<int> result;
			//	//
			//	if (getSummarizeRatio(sg, origin, src, trg) > threshold)
			//	{
			//		auto& p1_items = sNode->getEdges();
			//		auto& p2_items = tNode->getEdges();
			//		vector<int> result;
			//		if (isSummarizedNode(snNode) && isSummarizedNode(tnNode))	//각자 다른 슈퍼노드에 속한 경우
			//		{
			//			result.clear();
			//			set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
			//			for (int id : result) {
			//				sg.add(new Edge(trg, id));
			//			}
			//			result.clear();
			//			set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
			//			for (int id : result) {
			//				sg.add(new Edge(src, id));
			//			}
			//		}
			//		else if (isSummarizedNode(snNode))	//슈퍼노드(src) --- 일반노드(trg)
			//		{
			//			result.clear();
			//			set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::back_inserter(result));
			//			for (int id : result) {
			//				sg.add(new Edge(trg, id));
			//			}
			//		}
			//		else if (isSummarizedNode(tnNode))	//일반노드(src) --- 슈퍼노드(trg)
			//		{
			//			result.clear();
			//			set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(result));
			//			for (int id : result) {
			//				sg.add(new Edge(src, id));
			//			}
			//		}
			//	}
			//	else //요약 안되는 경우
			//	{
			//		//add + correction
			//		snNode->addCorrectionTarget('+', trg);
			//		tnNode->addCorrectionTarget('+', src);
			//	}
			//}
			else
			{
				if (getSummarizeRatio(sg, origin, src, trg) > threshold)
				{
					int c_plus = 0, c_minus;
					vector<Edge> inexistEdges;

					if (isSummarizedNode(snNode) && isSummarizedNode(tnNode))	//서로 다른 슈퍼노드 안에 있을 경우
					{
						SuperNode *ssNode, *stNode;
						ssNode = (SuperNode*)sg.get(snNode->getParent());
						stNode = (SuperNode*)sg.get(tnNode->getParent());
						auto& sinNode = ssNode->getSummarizedNodeIds();
						auto& tinNode = stNode->getSummarizedNodeIds();

						//check c+
						for (auto sNodeID : sinNode)
						{
							auto inNode = sg.get(sNodeID);
							for (auto tNodeID : tinNode)
							{
								if (!inNode->hasEdge(sNodeID, tNodeID))
								{
									++c_plus;
									inexistEdges.push_back(Edge(sNodeID, tNodeID));
								}
							}
						}
						//check c-
						c_minus = ((int)sinNode.size() * (int)tinNode.size()) - c_plus;
						if (c_plus > c_minus) //if C+ > C-
						{
							//add (+) correction e(n1, n2)
							snNode->addCorrectionTarget('+', trg);
							tnNode->addCorrectionTarget('+', src);
						}
						else // C+ <= C-
						{
							//insert a super edge e(s1, s2) into G
							//and add (-) corrections
							sg.add(new Edge(ssNode->getId(), stNode->getId()));
							for (auto& edge : inexistEdges)
							{
								int srcid = edge.getSource(),
									trgid = edge.getTarget();
								auto sCorrNode = sg.get(srcid);
								auto tCorrNode = sg.get(trgid);
								((NormalNode*)sCorrNode)->addCorrectionTarget('-', trgid);
								((NormalNode*)tCorrNode)->addCorrectionTarget('-', srcid);
							}
						}
					}
					else if (isSummarizedNode(snNode))	//슈퍼노드(source) --- 일반노드(target)
					{
						SuperNode *ssNode = (SuperNode*)sg.get(snNode->getParent());
						vector<int>& sinNode = ssNode->getSummarizedNodeIds();


						//calc c_plus
						for (auto sNodeID : sinNode)
						{
							if (!tnNode->hasEdge(sNodeID, trg))
							{
								++c_plus;
								inexistEdges.push_back(Edge(sNodeID, trg));
							}
						}
						c_minus = (int)sinNode.size() - c_plus;
						if (c_plus > c_minus)
						{
							//add (+) correction e(n1, n2)
							snNode->addCorrectionTarget('+', trg);
							tnNode->addCorrectionTarget('+', src);
						}
						else //C+ <= C-
						{
							sg.add(new Edge(ssNode->getId(), trg));
							for (auto& edge : inexistEdges)
							{
								int srcid = edge.getSource(),
									trgid = edge.getTarget();
								auto sCorrNode = sg.get(srcid);
								auto tCorrNode = sg.get(trgid);
								((NormalNode*)sCorrNode)->addCorrectionTarget('-', trgid);
								((NormalNode*)tCorrNode)->addCorrectionTarget('-', srcid);
							}
						}
					}
					else if (isSummarizedNode(tnNode))	//일반노드(source) --- 슈퍼노드(target)
					{
						SuperNode *stNode = (SuperNode*)sg.get(tnNode->getParent());
						vector<int>& tinNode = stNode->getSummarizedNodeIds();

						//calc c_plus
						for (auto tNodeID : tinNode)
						{
							if (!tnNode->hasEdge(tNodeID, src))
							{
								++c_plus;
								inexistEdges.push_back(Edge(tNodeID, src));
							}
						}
						c_minus = (int)tinNode.size() - c_plus;
						if (c_plus > c_minus)
						{
							//add (+) correction e(n1, n2)
							snNode->addCorrectionTarget('+', trg);
							tnNode->addCorrectionTarget('+', src);
						}
						else //C+ <= C-
						{
							sg.add(new Edge(stNode->getId(), src));
							for (auto& edge : inexistEdges)
							{
								int srcid = edge.getSource(),
									trgid = edge.getTarget();
								auto sCorrNode = sg.get(srcid);
								auto tCorrNode = sg.get(trgid);
								((NormalNode*)sCorrNode)->addCorrectionTarget('-', trgid);
								((NormalNode*)tCorrNode)->addCorrectionTarget('-', srcid);
							}
						}
					}


					if (c_plus > c_minus)
					{

					}
					else
					{

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

		size1 = (int)ns1.size();
		size2 = (int)ns2.size();


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

