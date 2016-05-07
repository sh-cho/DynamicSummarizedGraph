#include "Graph.h"
#include "SummarizedStructure.hpp"

vector<pair<int, int>> other_edges;
int default_size = 80000;

Graph* readGraph(string filename)
{
	Graph* graph = new Graph();
    ifstream fin(filename);
    int s, t;
    int max_id = -1;
    vector<pair<int, int>> edges;
    while (fin >> s >> t)
    {
        edges.emplace_back(pair<int, int>{ s, t });
        max_id = std::max({ s, t, max_id });
    }

    for (int i = 0; i <= max_id; ++i)
        graph->add(new Node(i));

    for (pair<int, int> e : edges)
    {
        graph->add(new Edge(e.first, e.second));
        graph->add(new Edge(e.second, e.first));
    }
    fin.close();
    return graph;
}

Graph* readSummarizedGraph(string filename)
{
	Graph* graph = new Graph();
    unordered_map<int, int> id_map;
    ifstream fin;
    {
        int s, t;
        int max_id = -1;
        vector<pair<int, int>> edges;

        fin.open(filename + ".ids");
        while (fin >> s >> t)
        {
            id_map[t] = s;
            max_id = max({ s, t, max_id });
        }
        fin.close();
    }
    {
        fin.open(filename + ".map");
        int s, t;
        string record;
        while (getline(fin, record))
        {
            stringstream ss(record);
            vector<int> items;
            int item;
            while (ss >> item)
                items.emplace_back(item);
            if (items.size() > 2)
            {
                SuperNode* sp = (SuperNode*)graph->add((Node*)(new SuperNode(items[0])));
                auto& snode_idset = sp->getSummarizedNodeIds();
                for (int i = 1; i < items.size(); ++i)
                {
                    snode_idset.emplace_back(items[i]);
                    NormalNode* np = new NormalNode(items[i]);
                    np->setParent(items[0]);
                    graph->add((Node*)np);
                }
            }
            else
            {
                graph->add((Node*)new NormalNode(items[0]));
            }
        }
        fin.close();
    }
    {
        fin.open(filename + ".sum");
        int s, t;
        while (fin >> s >> t)
            graph->add(new Edge(s, t));
        fin.close();
    }
    {
        fin.open(filename + ".cor");
        char c;
        int s, t;
        while (fin >> c >> s >> t)
        {
            NormalNode* src = (NormalNode*)graph->get(s);
            NormalNode* trg = (NormalNode*)graph->get(t);
            src->addCorrectionTarget(c, t);
            trg->addCorrectionTarget(c, s);
        }
        fin.close();
    }
    return graph;
}