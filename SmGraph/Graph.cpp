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