#pragma once
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "voronoi.h"
#include "unordered_set"
#include "unordered_map"
using namespace std;

class CellEdge;
class Cell;
class Fragment;

struct IndexEdge
{
	int v1, v2;
};
struct Face
{
	int v1, v2, v3;
};

using EdgeList = vector<IndexEdge>;
using VertexEdgesMap = unordered_map<int, EdgeList>;

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

class CellEdge
{
public:
	CellEdge(VEdge& edge)
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

	struct key_hash : public std::unary_function <sf::Vector2f, std::size_t >
	{
		std::size_t operator()(const sf::Vector2f& k) const
		{
			return k.x * k.y;
		}
	};

	struct  key_equal : public std::binary_function<sf::Vector2f, sf::Vector2f, bool>
	{
		bool operator()(const sf::Vector2f& v0, const sf::Vector2f& v1) const
		{
			return (
				int(v0.x) == int(v1.x) &&
				int(v0.y) == int(v1.y)
				);
		}
	};

	void setEdges(vector<CellEdge> edges)
	{
		// find all neighbour edges
		// two neighbour edges form a triangle, which is a face
		// a face is a three of vertice index, which is a triangle
		unordered_set<sf::Vector2f, key_hash, key_equal> vertexSet = {};
		// find all vertices
		for (int i = 0; i < edges.size(); i++)
		{
			if (vertexSet.count(edges[i].v1) == 0) vertexSet.insert(edges[i].v1);
			if (vertexSet.count(edges[i].v2) == 0) vertexSet.insert(edges[i].v2);
		}
		vertices.clear();
		vertices.insert(vertices.end(), vertexSet.begin(), vertexSet.end());

		// turing CellEdge to IdxEdge, which use vertex indices
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
				if (x1 == x2 && y1 == y2) v2Idx = j;
			}
			IndexEdge edge = { v1Idx, v2Idx };
			edgeList.push_back(edge);
		}

		VertexEdgesMap vertexEdges;
		for (int i = 0; i < edgeList.size(); i++)
		{
			vertexEdges[edgeList[i].v1].push_back(edgeList[i]);
			vertexEdges[edgeList[i].v2].push_back(edgeList[i]);
		}
		
		faces = {};
		for (int i = 0; i < edgeList.size(); i++)
		{
			IndexEdge edge = edgeList[i];
			EdgeList edges1 = vertexEdges[edge.v1];
			EdgeList edges2 = vertexEdges[edge.v2];

			for (const IndexEdge& adjacentEdge1 : edges1)
			{
				if (adjacentEdge1.v1 == edge.v1 || adjacentEdge1.v2 == edge.v1)
				{
					for (const IndexEdge& adjacentEdge2 : edges2)
					{
						if (adjacentEdge2.v1 == edge.v2 || adjacentEdge2.v2 == edge.v2)
						{
							Face face = {
								edge.v1,
								edge.v2,
								(adjacentEdge1.v1 == edge.v1
								|| adjacentEdge1.v2 == edge.v1)
								? adjacentEdge1.v2 : adjacentEdge1.v1
							};
							faces.push_back(face);
						}
					}
				}
			}
		}

		edges.clear();
	}
	sf::Vector2f site;
	vector<sf::Vector2f> vertices;
	vector<Face> faces;
	vector<CellEdge> edges;
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