
#include <SFML/Graphics.hpp>
#include "voronoi.h"
#include "fragment.h"

Voronoi* vdg;
vector<VoronoiPoint*> ver;
vector<VEdge> edges;

void handleImpact(int x, int y, float force);
void shatterFragment(Fragment fragment, sf::Vector2f impactPoint, float force);
Fragment extractSubfragment(Cell cell, sf::Vector2f impactPoint, float force, Material material);
float shatterFactor(Cell* cell, sf::Vector2f impactPoint, float force, Material material);
float norm(sf::Vector2f v1, sf::Vector2f v2);

int main()
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
	std::vector<sf::Vertex*> lines;

	for (int i = 0; i < edges.size(); i++)
	{
		sf::Vertex* test = new sf::Vertex[2];
		test[0] = sf::Vector2f((edges[i].VertexA.x), (edges[i].VertexA.y));
		test[1] = sf::Vector2f((edges[i].VertexB.x), (edges[i].VertexB.y));
		lines.push_back(test);
	}

	sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::MouseButtonPressed:
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						std::cout << event.mouseButton.x << " " << event.mouseButton.y << '\n';
						handleImpact(event.mouseButton.x, event.mouseButton.y, 10);
					}
					break;
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape)
						window.close();
					break;
			}

		}

		window.clear();
		for(auto line : lines)
			window.draw(line, 2, sf::Lines);
		window.display();
	}

	return 0; 
}

void handleImpact(int x, int y, float force)
{
	// find points near the impact point
	// call shatter fragment
}

void shatterFragment(Fragment fragment, sf::Vector2f impactPoint, float force)
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

Fragment extractSubfragment(Cell cell, sf::Vector2f impactPoint, float force, Material material)
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

float shatterFactor(Cell *cell, sf::Vector2f impactPoint, float force, Material material)
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