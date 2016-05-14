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
		//set<int> set1(ns1.begin(), ns1.end());
		//set<int> set2(ns2.begin(), ns2.end());
		
		set<int> subgraph;
		set<int> checkedNodeList;
		
		set<int>::iterator set_iter;


		//add subraph to A, A_2, B, B_2
		subgraph.emplace(id1);
		for (int nodeID : ns1)
			subgraph.emplace(nodeID);
		subgraph.emplace(id2);
		for (int nodeID : ns2)
			subgraph.emplace(nodeID);



		//vector<int> result;
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
				//printf("u:%d, v:%d\n", u, v);


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

			printf("u:%d, v:%d\n", u, v);


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

				SuperNode *spU, *spV;



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
						//SuperNode 합치기 -> A 기준으로 합침
						spU = (SuperNode*)sg.get(srcNode->getParent());
						spV = (SuperNode*)sg.get(trgNode->getParent());
						int spID = spU->getId();
						auto& vNodes = spV->getSummarizedNodeIds();
						for (auto node : vNodes)
						{
							spU->addSummarizedNode(node);
						}
						

						//add new Supernode in subgraph
						subgraph.emplace(spID);


						//Supernode U의 이웃노드 체크 해제
						//set<int>::iterator setIter;
						auto& snNeighbor = spU->getEdges();
						for (Edge* edge : snNeighbor)
						{
							int neighborID = edge->getOther(spID);

							//find and erase
							set_iter = checkedNodeList.find(neighborID);
							if (set_iter != checkedNodeList.end())
								checkedNodeList.erase(set_iter);
						}
					}
				}
				else if (!sameParentAndId(srcNode))	//3-1. U is exist in supernode
				{
					//SuperNode 하나로 합치기
					spU = (SuperNode*)sg.get(srcNode->getParent());
					int spID = spU->getId();
					spU->addSummarizedNode(trgNode->getId());


					//add new Supernode in subgraph
					subgraph.emplace(spID);

					//이웃노드 체크 해제
					//set<int>::iterator setIter;
					auto& snNeighbor = spU->getEdges();
					for (Edge* edge : snNeighbor)
					{
						int neighborID = edge->getOther(spID);

						//find and erase
						set_iter = checkedNodeList.find(neighborID);
						if (set_iter != checkedNodeList.end())
							checkedNodeList.erase(set_iter);
					}
				}
				else if (!sameParentAndId(trgNode))	//3-2. V is exist in supernode
				{
					//SuperNode 하나로 합치기
					spV = (SuperNode*)sg.get(trgNode->getParent());
					int spID = spV->getId();
					spV->addSummarizedNode(srcNode->getId());


					//add new Supernode in subgraph
					subgraph.emplace(spID);


					//이웃노드 체크 해제
					//set<int>::iterator setIter;
					auto& snNeighbor = spV->getEdges();
					for (Edge* edge : snNeighbor)
					{
						int neighborID = edge->getOther(spID);

						//find and erase
						set_iter = checkedNodeList.find(neighborID);
						if (set_iter != checkedNodeList.end())
							checkedNodeList.erase(set_iter);
					}
				}
				else //4. 일반노드
				{
					//make new supernode
					SuperNode* spNew = (SuperNode*)sg.add((Node*)(new SuperNode((int)sg.getNodeCount())));
					int spID = spNew->getId();

					spNew->addSummarizedNode(u);
					spNew->addSummarizedNode(v);	//superNode 추가 뒤 노드 추가
					srcNode->setParent(spID);
					trgNode->setParent(spID);
					
					
					

					//add new Supernode in subgraph
					subgraph.emplace(spID);


					//이웃노드 체크 해제
					//set<int>::iterator setIter;
					auto& snNeighbor = spNew->getEdges();
					for (Edge* edge : snNeighbor)
					{
						int neighborID = edge->getOther(spID);

						//find and erase
						set_iter = checkedNodeList.find(neighborID);
						if (set_iter != checkedNodeList.end())
							checkedNodeList.erase(set_iter);
					}
				}


				//임시
				//탐색 range를 두 노드로 설정
				//return;
			}
			else
			{
				checkedNodeList.emplace(u);
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
		//auto* uNode = sg.get(id1);
		//auto* vNode = sg.get(id2);

		auto& ns1 = getNeighborNodes(sg, origin, id1);
		auto& ns2 = getNeighborNodes(sg, origin, id2);
		//int total = (int)ns1.size() + (int)ns2.size();      // c_u + c_v
		set<int> set1(ns1.begin(), ns1.end());
		set<int> set2(ns2.begin(), ns2.end());
		vector<int> interSection;
		set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(interSection));
		
		/*
		int c_u = 0, c_v = 0;
		
		//get cost of u, cost of v
		//int pi_uv, a_uv, a_u, a_v;
		double s_uv;


		//get c_u
		for (auto x : ns1)
		{
			int pi_ux = 0, a_ux = 0;

			//get pi_ux
			//pi_ux = (int)set1.size() * (int)set2.size();
			if (uNode->getType() == SUPER_NODE)
			{
				pi_ux = ((SuperNode*)uNode)->getSummarizedNodeIds().size();	
			}
			else
			{
				pi_ux = 1;
			}

			if (sg.get(x)->getType() == SUPER_NODE)	//x is supernode
			{
				pi_ux *= ((SuperNode*)sg.get(x))->getSummarizedNodeIds().size();
			}


			//get a_ux
			if (uNode->getType() == SUPER_NODE)
			{
				auto& inNodes = ((SuperNode*)uNode)->getSummarizedNodeIds();

				for (auto node : inNodes)
				{
					auto* pNode = sg.get(node);
					auto edges = pNode->getEdges();
					for (auto edge : edges)
					{
						if (edge->getTarget() == x || edge->getSource() == x)
							++a_ux;
					}
				}

				auto edges = uNode->getEdges();
				for (auto edge : edges)
				{
					if (edge->getTarget() == x || edge->getSource() == x)
						++a_ux;
				}
			}
			else
			{
				auto edges = uNode->getEdges();
				for (auto edge : edges)
				{
					if (edge->getTarget() == x || edge->getSource() == x)
						++a_ux;
				}
			}

			printf("pi_ux:%d, a_ux:%d\n", pi_ux, a_ux);
			c_u += std::min({abs(pi_ux - a_ux) + 1, abs(a_ux)});
		}

		//get c_v
		for (auto x : ns2)
		{
			int pi_vx = 0, a_vx = 0;

			//get pi_vx
			pi_vx = (int)set1.size() * (int)set2.size();

			//get a_vx
			if (vNode->getType() == SUPER_NODE)
			{
				auto& inNodes = ((SuperNode*)vNode)->getSummarizedNodeIds();

				for (auto node : inNodes)
				{
					auto* pNode = sg.get(node);
					auto edges = pNode->getEdges();
					for (auto edge : edges)
					{
						if (edge->getTarget() == x || edge->getSource() == x)
							++a_vx;
					}
				}

				auto edges = vNode->getEdges();
				for (auto edge : edges)
				{
					if (edge->getTarget() == x || edge->getSource() == x)
						++a_vx;
				}
			}
			else
			{
				auto edges = vNode->getEdges();
				for (auto edge : edges)
				{
					if (edge->getTarget() == x || edge->getSource() == x)
						++a_vx;
				}
			}

			c_v += std::min({ abs(pi_vx - a_vx) + 1, abs(a_vx) });
		}


		int c_w = (int)set1.size() + (int)set2.size() - (int)interSection.size();

		//debug
		//cout << "ratio:" << (double)(set1.size() + set2.size() - c_w) / (total) << endl;

		if (c_u + c_v != 0)
		{
			s_uv = (c_u + c_v - c_w) / (double)(c_u + c_v);
		}
		else
		{
			s_uv = -1;
		}


		//debug
		printf("c_u:%d\tc_v:%d\tc_w:%d\ts_uv:%f\n", c_u, c_v, c_w, s_uv);
		
		return s_uv;
		*/


		int c_w = (int)set1.size() + (int)set2.size() - (int)interSection.size();
		//cout << "ratio: " << (double)(set1.size() + set2.size() - c_w) / (set1.size() + set2.size()) << endl;
		return (double)(set1.size() + set2.size() - c_w) / (set1.size() + set2.size());
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

			//if (it != subgraph.end())
			//	nodeID = *it;
			//else //not found
			//	nodeID = -1;
			nodeID = (*it);
		}
		else	//get random u2 node
		{
			auto& neighborNodes = getNeighborNodes(sg, origin, u);
			if (neighborNodes.size() == 0)
				return nodeID;
			vector<int>::iterator vecit = neighborNodes.begin();
			
			advance(vecit, random::Int(0, (int)neighborNodes.size()-1));
			auto& twohopNodes = getNeighborNodes(sg, origin, *vecit);
			if (twohopNodes.size() == 0)
				return nodeID;

			vecit = twohopNodes.begin();
			advance(vecit, random::Int(0, (int)twohopNodes.size() - 1));
			if (vecit != std::end(twohopNodes) && subgraph.find(*vecit) != subgraph.end())
				nodeID = (*vecit);

			/*for (int neighborID : neighborNodes)
			{
				if (subgraph.find(neighborID) != subgraph.end())
				{
					nodeID = neighborID;
					break;
				}
			}*/
		}


		return nodeID;
	}

	//static void countEdges(Graph& sg, Graph& origin)
	//{
	//	int sgCnt = 0,
	//		origCnt = 0;

	//	auto sNodes = sg.getNodes();
	//	auto oNodes = origin.getNodes();

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

	//	for (auto node : oNodes)
	//	{
	//		if (!node)
	//			continue;
	//		origCnt += node->getEdges().size();
	//	}

	//	//printf("sg:%d, orig:%d\n", sgCnt/2, origCnt/2);
	//}
};

