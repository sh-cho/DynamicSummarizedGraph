#pragma once
#include "base_header.h"
using namespace std;


enum NODE_TYPE { NORMAL_NODE = 1, SUPER_NODE };

/////////////////////////////////////////////////////
// Edge
class Edge
{
public:
    Edge(const int source, const int target)
    {
        nodes[0] = source;
        nodes[1] = target;
    }
    virtual ~Edge() {}

public:
    auto getSource(){ return nodes[0]; }
    auto getTarget(){ return nodes[1]; }
    auto getNodes() { return nodes; }
    auto getOther(int one);


protected:
    int nodes[2];
};
typedef Edge* pEdge;

/////////////////////////////////////////////////////
// Node
class Node
{
public:
    Node(int id) { this->id = id; }
    virtual ~Node();

public:
    auto setId(const int nid) { id = nid;}
    auto addEdge(Edge* edge){ edges.emplace_back(edge); }
    auto& getEdges() { return edges; }
    auto getId() { return id; }
    virtual int getType() { return 0; };


protected:
    int id;
    vector<Edge*> edges;
};
typedef Node* pNode;


/////////////////////////////////////////////////////
// Graph
class Graph
{
public:
    Graph();
    ~Graph();

public:
    Node* add(Node* node);
    Edge* add(Edge* edge);
    Node* get(int id);
    auto& getNodes() { return nodes; }
    auto getNodeCount(){ return nodes.size(); }
    auto setMaxNodeCount(int s) { nodes.reserve(s); }


protected:
    vector<pNode> nodes;
};

