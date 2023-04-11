#pragma once
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "voronoi.h"
#include "unordered_set"
using namespace std;

class CellEdge;
class Cell;
class Fragment;
typedef tuple<int, int> IndexEdge;
struct Face;
using EdgeList = vector<IndexEdge>;
using VertexEdgesMap = unordered_map<int, EdgeList>;

struct Face
{
	int v1, v2, v3;
};

struct Material
{
public:
	Material(float shatterLocality, float durability)
	{
		this->durability = durability;
		this->shatterLocality = shatterLocality;
	}
	float durability;
	float shatterLocality;
private:

};

class Cell
{
public:
	Cell(sf::Vector2f site, Fragment* fragment)
	{
		this->site = site;
		//this->edges = {};
		this->neighbours = {};
		this->fragment = fragment;
		this->visited = false;
	}

	void setEdges(vector<CellEdge> edges)
	{
		// find all neighbour edges
		// two neighbour edges form a triangle, which is a face
		// a face is a three of vertice index, which is a triangle
		unordered_set<sf::Vector2f> vertexSet = {};
		// find all vertices
		for (int i = 0; i < edges.size(); i++)
		{
			if (vertexSet.count(edges[i].v1) == 0) vertexSet.insert(edges[i].v1);
			if (vertexSet.count(edges[i].v2) == 0) vertexSet.insert(edges[i].v2);
		}
		vertices.clear();
		vertices.insert(vertices.end(), vertexSet.begin(), vertexSet.end());

		EdgeList edgeList = {};
		for (int i = 0; i < edges.size(); i++)
		{
			int v1Idx;
			for (int j = 0; j < vertices.size(); j++)
			{
				int x1 = int(edges[i].v1.x), y1 = int(edges[i].v1.y);
				int x2 = int(vertices[j].x), y2 = int(vertices[j].y);
				if (x1 == x2 && y1 == y2) v1Idx = j;
			}

			int v2Idx;
			for (int j = 0; j < vertices.size(); j++)
			{
				int x1 = int(edges[i].v2.x), y1 = int(edges[i].v2.y);
				int x2 = int(vertices[j].x), y2 = int(vertices[j].y);
				if (x1 == x2 && y1 == y2) v1Idx = j;
			}

			edgeList.push_back(make_tuple(v1Idx, v2Idx));
		}

		VertexEdgesMap vertexEdges;
		for (int i = 0; i < edgeList.size(); i++)
		{
			vertexEdges[std::get<0>(edgeList[i])].push_back(edgeList[i]);
			vertexEdges[std::get<1>(edgeList[i])].push_back(edgeList[i]);
		}
		
		faces = {};
		for (int i = 0; i < edgeList.size(); i++)
		{
			EdgeList edges1 = vertexEdges[std::get<0>(edgeList[i])];
			EdgeList edges2 = vertexEdges[std::get<1>(edgeList[i])];

			for (const tuple<int, int>& adjacentEdge1 : edges1)
			{
				if (get<0>(adjacentEdge1) == get<0>(edgeList[i]) || get<1>(adjacentEdge1) == get<0>(edgeList[i]))
				{
					for (const tuple<int, int>& adjacentEdge2 : edges2)
					{
						if (get<0>(adjacentEdge2) == get<1>(edgeList[i]) || get<1>(adjacentEdge1) == get<1>(edgeList[i]))
						{
							Face face = {
								get<0>(edgeList[i]),
								get<1>(edgeList[i]),
								(get<0>(adjacentEdge1) == get<0>(edgeList[i])
								|| get<1>(adjacentEdge1) == get<0>(edgeList[i]))
								? get<1>(adjacentEdge1) : get<0>(adjacentEdge1)
							};
							faces.push_back(face);
						}
					}
				}
			}
		}

		printf("hi");
	}
	sf::Vector2f site;
	vector<sf::Vector2f> vertices;
	vector<Face> faces;
	//vector<CellEdge> edges;
	vector<Cell*> neighbours;
	Fragment* fragment;
	bool visited;
private:
};

class Fragment
{
public:
	Fragment()
	{
		COM = sf::Vector2f(0, 0);
	}
	vector<Cell*> cells = {};
	Material material = Material(1, 0.05);
	float mass = 1;
	sf::Vector2f velocity;
	sf::Vector2f COM;
	void merge(Fragment* fragment)
	{
		this->cells.insert(this->cells.end(), fragment->cells.begin(), fragment->cells.end());

		// assign parent fragment to the new one
		for (int i = 0; i < fragment->cells.size(); i++)
		{
			fragment->cells[i]->fragment = this;
		}
	}
private:
};

class CellEdge
{
public:
	CellEdge(VEdge &edge)
	{
		this->v1 = sf::Vector2f(edge.VertexA.x, edge.VertexA.y);
		this->v2 = sf::Vector2f(edge.VertexB.x, edge.VertexB.y);
	}
	CellEdge(sf::Vector2f v1, sf::Vector2f v2)
	{
		this->v1 = v1;
		this->v2 = v2;
	}
	sf::Vector2f v1;
	sf::Vector2f v2;
};