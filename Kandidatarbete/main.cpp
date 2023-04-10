
#include <SFML/Graphics.hpp>
#include "voronoi.h"

Voronoi* vdg;
vector<VoronoiPoint*> ver;
vector<VEdge> edges;

void handleImpact(int x, int y, float force);
float calculateImpactFactor(sf::Vector2f cell, sf::Vector2f impactPoint, float force);
bool isCracked(sf::Vector2f i1, sf::Vector2f i2, float durability);
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
	// loop related cells calculate the impact force
	
	// should remove this
	sf::Vector2f cell1 = sf::Vector2f(0, 0);
	sf::Vector2f cell2 = sf::Vector2f(0, 0);

	sf::Vector2f impactPoint = sf::Vector2f(x, y);

	float impactFactor1 = calculateImpactFactor(cell1, impactPoint, force);
	float impactFactor2 = calculateImpactFactor(cell2, impactPoint, force);

	isCracked(impactFactor2, impactFactor2, 1);
}

float calculateImpactFactor(sf::Vector2f cell, sf::Vector2f impactPoint, float force)
{
	sf::Vector2f cell = sf::Vector2f(0, 0);

	float distance = norm(cell, impactPoint);

	return force / (distance + 1);
}

bool isCracked(float i1, float i2, float durability)
{
	if (abs(i1 - i2) > durability) return true;
	
	return false;
}

float norm(sf::Vector2f v1, sf::Vector2f v2)
{
	sf::Vector2f diff = v1 - v2;
	float distance = (std::sqrt(diff.x * diff.x + diff.y * diff.y));
}