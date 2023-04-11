#pragma once
#include <SFML/Graphics.hpp>
#include "voronoi.h"
#include "fragment.h"
#include <unordered_map>

class Scene
{
public:
	Voronoi* vdg;
	vector<VoronoiPoint*> ver;
	vector<VEdge> edges;
	vector<Fragment*> fragments;

	typedef std::tuple<int, int> key_t;

	struct key_hash : public std::unary_function<key_t, std::size_t>
	{
		std::size_t operator()(const key_t& k) const
		{
			return std::get<0>(k) ^ std::get<1>(k);
		}
	};

	struct  key_equal : public std::binary_function<key_t, key_t, bool>
	{
		bool operator()(const key_t& v0, const key_t& v1) const
		{
			return (
					std::get<0>(v0) == std::get<0>(v1) &&
					std::get<1>(v0) == std::get<1>(v1)
				);
		}
	};

	typedef std::unordered_map<key_t, int, key_hash, key_equal> VertexMap;

	Scene()
	{
		// for saving index or vertecies, users can search index by vertex position 
		VertexMap vMap = {};
		Fragment fragment = {};
		//FORTUNES ALGORITHM
		for (vector<VoronoiPoint*>::iterator i = ver.begin(); i != ver.end(); i++)
			delete((*i));
		ver.clear();
		edges.clear();
		for (int i = 0; i < 100; i++)
		{
			float x = rand() % 500;
			float y = rand() % 500;
			ver.push_back(new VoronoiPoint(x, y));
			vMap[std::make_tuple(int(x), int(y))] = i;
			fragment.cells.push_back(new Cell(sf::Vector2f(x, y), &fragment));
		}

		// generate voronoi diagram
		vdg = new Voronoi();
		double minY = 0;
		double maxY = 500;
		edges = vdg->ComputeVoronoiGraph(ver, minY, maxY);
		delete vdg;

		// loop through all edges, put in to corresponding cell and find neighbours
		for (int i = 0; i < edges.size(); i++)
		{
			VEdge edge = edges[i];
			
			// get left cell index 
			VoronoiPoint leftCell = edge.Left_Site;
			int leftCellIdx = vMap[std::make_tuple(int(leftCell.x), int(leftCell.y))];
			fragment.cells[leftCellIdx]->edges.push_back(CellEdge(edge));

			// get right cell index 
			VoronoiPoint rightCell = edge.Right_Site;
			int rightCellIdx = vMap[std::make_tuple(int(rightCell.x), int(rightCell.y))];
			fragment.cells[rightCellIdx]->edges.push_back(CellEdge(edge));

			fragment.cells[leftCellIdx]->neighbours.push_back(fragment.cells[rightCellIdx]);
			fragment.cells[rightCellIdx]->neighbours.push_back(fragment.cells[leftCellIdx]);
		}

		fragments.push_back(&fragment);
		printf("hi");
		// TODO: fit the voronoi diagram to the fragment structure as the paper
	}

	void integrateVelocity(double timeStep)
	{

	}

	void integratePosition(double timeStep)
	{
		// update fragement edges and cells according to velocity and timestep
		return;

		for (int i = 0; i < fragments.size(); i++)
		{
			Fragment* fragment = fragments[i];
			for (int j = 0; j < fragment->cells.size(); j++)
			{
				Cell* cell = fragment->cells[j];
				cell->site += fragment->velocity;
				// TODO: expect COM, we also need to update the vertices(or edges)
			}
		}
	}

	void intergrate(double timeStep)
	{
		integrateVelocity(timeStep);
		integratePosition(timeStep);
	}

	void handleImpact(const int x, const int y, const float force)
	{
		// find points near the impact point
		// call shatter fragment
		shatterFragment(fragments[0], sf::Vector2f(x, y), force);
	}

	void shatterFragment(Fragment* fragment, sf::Vector2f impactPoint, const float force)
	{
		for (int i = 0; i < fragment->cells.size(); i++)
		{
			Cell* cell = fragment->cells[i];
			cell->visited = false;

			vector<Cell*> removing = vector<Cell*>();
			for (int j = 0; j < cell->neighbours.size(); j++)
			{
				Cell n = *cell->neighbours[j];
				if (n.fragment == fragment)
				{
					removing.push_back(&n);
					// remove n from s.neighbours
				}
			}

			for (int j = 0; j < removing.size(); j++)
			{
				remove(cell->neighbours.begin(), cell->neighbours.end(), removing[j]);
			}

			removing.clear();
		}
		vector<Fragment*> L = {};

		for (int i = 0; i < fragment->cells.size(); i++)
		{
			Cell* cell = fragment->cells[i];
			if (cell->visited == false)
			{
				Fragment R = extractSubfragment(cell, impactPoint, force, fragment->material);
				// copy any additional properties from original fragment to new fragment
				R.mass = fragment->mass;
				R.material = fragment->material;
				L.push_back(&R);
			}
		}
	}

	Fragment extractSubfragment(Cell* cell, sf::Vector2f impactPoint, const float force, const Material material)
	{
		cell->visited = true;
		Fragment R = Fragment();
		R.material = material;
		R.cells.push_back(cell);
		cell->fragment = &R;

		for (int i = 0; i < cell->neighbours.size(); i++)
		{
			float factorA = shatterFactor(cell, impactPoint, force, material);
			float factorB = shatterFactor(cell->neighbours[i], impactPoint, force, material);

			if (abs(factorA - factorB) > material.durability)
			{
				R.merge(extractSubfragment(cell->neighbours[i], impactPoint, force, material));
			}

		}

		return R;
	}

	float shatterFactor(Cell* cell, sf::Vector2f impactPoint, const float force, const Material material)
	{
		sf::Vector2f site = cell->site;

		float distance = norm(site, impactPoint);

		return force / (pow(distance, material.shatterLocality) + 1);
	}

	float norm(sf::Vector2f v1, sf::Vector2f v2)
	{
		sf::Vector2f diff = v1 - v2;
		return std::sqrt(diff.x * diff.x + diff.y * diff.y);
	}
};