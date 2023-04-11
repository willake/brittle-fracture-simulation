#pragma once
#include <SFML/Graphics.hpp>
#include "voronoi.h"
#include "fragment.h"

class Scene
{
public:
	Voronoi* vdg;
	vector<VoronoiPoint*> ver;
	vector<VEdge> edges;
	vector<Fragment> fragments;

	Scene()
	{
		//FORTUNES ALGORITHM
		for (vector<VoronoiPoint*>::iterator i = ver.begin(); i != ver.end(); i++)
			delete((*i));
		ver.clear();
		edges.clear();
		for (int i = 0; i < 100; i++)
		{
			ver.push_back(new VoronoiPoint(rand() % 500, rand() % 500));
		}

		// generate voronoi diagram
		vdg = new Voronoi();
		double minY = 0;
		double maxY = 500;
		edges = vdg->ComputeVoronoiGraph(ver, minY, maxY);
		delete vdg;

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
			Fragment fragment = fragments[i];
			for (int j = 0; j < fragment.cells.size(); j++)
			{
				Cell cell = fragment.cells[j];
				cell.site += fragment.velocity;
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

	void shatterFragment(Fragment fragment, sf::Vector2f impactPoint, const float force)
	{
		for (int i = 0; i < fragment.cells.size(); i++)
		{
			Cell cell = fragment.cells[i];
			cell.visited = false;

			vector<Cell*> removing = vector<Cell*>();
			for (int j = 0; j < cell.neighbours.size(); j++)
			{
				Cell n = *cell.neighbours[j];
				if (n.fragment == &fragment)
				{
					removing.push_back(&n);
					// remove n from s.neighbours
				}
			}

			for (int j = 0; j < removing.size(); j++)
			{
				remove(cell.neighbours.begin(), cell.neighbours.end(), removing[j]);
			}

			removing.clear();
		}
		vector<Fragment> L = vector<Fragment>();

		for (int i = 0; i < fragment.cells.size(); i++)
		{
			Cell cell = fragment.cells[i];
			if (cell.visited == false)
			{
				Fragment R = extractSubfragment(cell, impactPoint, force, fragment.material);
				// copy any additional properties from original fragment to new fragment
				R.mass = fragment.mass;
				R.material = fragment.material;
				L.push_back(R);
			}
		}
	}

	Fragment extractSubfragment(Cell cell, sf::Vector2f impactPoint, const float force, const Material material)
	{
		cell.visited = true;
		Fragment R = Fragment();
		R.material = material;
		R.cells.push_back(cell);
		cell.fragment = &R;

		for (int i = 0; i < cell.neighbours.size(); i++)
		{
			float factorA = shatterFactor(&cell, impactPoint, force, material);
			float factorB = shatterFactor(cell.neighbours[i], impactPoint, force, material);

			if (abs(factorA - factorB) > material.durability)
			{
				R.merge(extractSubfragment(*cell.neighbours[i], impactPoint, force, material));
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