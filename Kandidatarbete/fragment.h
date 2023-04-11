#pragma once
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "voronoi.h"
using namespace std;

class CellEdge;
class Cell;
class Fragment;

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
		this->edges = {};
		this->neighbours = {};
		this->fragment = fragment;
		this->visited = false;
	}
	sf::Vector2f site;
	// faces?
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
	}
	vector<Cell*> cells = {};
	Material material = Material(1, 0.05);
	float mass = 1;
	sf::Vector2f velocity;
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