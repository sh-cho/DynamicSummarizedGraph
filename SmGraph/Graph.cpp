#include "Graph.h"



Graph::Graph()
{
}


Graph::~Graph()
{
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i] != NULL)
		{
			delete nodes[i];
		}
	}
}

Node* Graph::add(Node * pnode)
{
	int id = pnode->getId();
	if (nodes.size() <= (unsigned)id)
		nodes.resize(id + 1, NULL);
	nodes[id] = pnode;
	return pnode;
}

Edge* Graph::add(Edge* edge)
{
	nodes[edge->getSource()]->addEdge(edge);
	return edge;
}

Node* Graph::get(int id)
{
	Node* p = nodes[id];
	if (p == NULL)
	{
		p = new Node(id);
		nodes[id] = p;
	}
	return p;
}

void Graph::printInfo()
{
	//print supernode, normalnode, edge, superedge
	int normalNodes = 0, superNodes = 0, normalEdges = 0, superEdges = 0;

	for (auto node : nodes)
	{
		if (!node)
			continue;


		int nodeType = node->getType();
		auto& edges = node->getEdges();

		if (nodeType == NORMAL_NODE)
		{
			++normalNodes;
			normalEdges += (int)edges.size();
		}
		else //supernode
		{
			++superNodes;
			superEdges += (int)edges.size();
		}
	}


	printf("* normal nodes: %d\n", normalNodes);
	printf("* super nodes: %d\n", superNodes);
	printf("* normal edges: %d\n", normalEdges /2);
	printf("* super edges: %d\n", superEdges/2);
}

int Edge::getOther(int one)
{
	if (nodes[0] == one)
		return nodes[1];
	if (nodes[1] == one)
		return nodes[0];
	throw std::exception("invalid id");
}

Node::~Node()
{
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		if (edges[i] != NULL)
		{
			delete edges[i];
		}
	}
}