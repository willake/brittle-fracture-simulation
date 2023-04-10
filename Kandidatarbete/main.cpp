
#include <SFML/Graphics.hpp>
#include "scene.h"

Scene scene;

void drawScene(sf::RenderWindow &window, Scene scene);

int main()
{
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
						scene.handleImpact(event.mouseButton.x, event.mouseButton.y, 10);
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

		drawScene(window, scene);
	}

	return 0; 
}

void drawScene(sf::RenderWindow &window, Scene scene)
{
	std::vector<sf::Vertex*> lines;

	for (int i = 0; i < scene.edges.size(); i++)
	{
		sf::Vertex* test = new sf::Vertex[2];
		test[0] = sf::Vector2f((scene.edges[i].VertexA.x), (scene.edges[i].VertexA.y));
		test[1] = sf::Vector2f((scene.edges[i].VertexB.x), (scene.edges[i].VertexB.y));
		lines.push_back(test);
	}

	window.clear();
	for (auto line : lines)
		window.draw(line, 2, sf::Lines);
	window.display();
}