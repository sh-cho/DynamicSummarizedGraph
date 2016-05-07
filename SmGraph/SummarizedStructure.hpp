#pragma once
#include "Graph.h"

class NormalNode : public Node
{
public:
    NormalNode(int id) :Node(id) { parent = id; }
	virtual ~NormalNode() {}
    void setParent(int id) { parent = id; }
    int getParent() { return parent; }
    void addCorrectionTarget(char c, int id) 
    { 
        if (c == '+')
            plus_correction.emplace_back(id);
        else if (c == '-')
            minus_correction.emplace_back(id);
    }
    auto& getCorrectionTargets(char c) 
    {
        if (c == '+')
            return plus_correction;
        else if (c == '-')
            return minus_correction;
        return vector<int>();
    }
    virtual int getType() { return NORMAL_NODE; }


private:
    int parent = -1;
    vector<int> plus_correction;
    vector<int> minus_correction;
};

class SuperNode : public Node
{
public:
    SuperNode(int id) :Node(id) {}
    void addSummarizedNode(int id) { summarized_nodes.emplace_back(id); }
    auto& getSummarizedNodeIds() { return summarized_nodes; }
    bool hasSummarizedNode(int id)
    {
        for (int nid : summarized_nodes)
        {
            if (nid == id)
                return true;
        }
        return false;
    }
    virtual int getType() { return SUPER_NODE; }


private:
    vector<int> summarized_nodes;
};

//class SuperEdge : Edge
//{
//public:
//    auto& getMinusCorrections() { return minus_corrections; }
//    auto isReal(int sid, int tid)
//    {
//        for (auto e : minus_corrections)
//        {
//            if ((e.first == sid && e.second == tid) ||
//                (e.second == sid && e.first == tid))
//                return false;
//        }
//        return true;
//    }
//private:
//    vector<pair<int, int>> minus_corrections;
//};