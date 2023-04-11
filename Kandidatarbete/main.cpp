#pragma once
#include <SFML/Graphics.hpp>
#include "scene.h"
#include "voronoi.h"

Scene* scene;

// https://www.sfml-dev.org/tutorials/2.5/system-time.php
sf::Clock gameClock;

void drawScene(sf::RenderWindow &window, Scene* scene);

int main()
{
	scene = new Scene();
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
						scene->handleImpact(event.mouseButton.x, event.mouseButton.y, 10);
						scene->clicked = true;
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

		sf::Time elapsed = gameClock.restart();
		scene->intergrate(elapsed.asSeconds());
		drawScene(window, scene);
	}

	return 0; 
}

void drawScene(sf::RenderWindow &window, Scene* scene)
{
	window.clear();
	if (scene->clicked)
	{
		for (int i = 0; i < scene->fragments.size(); i++)
		{
			Fragment* fragment = scene->fragments[i];
			for (int j = 0; j < fragment->cells.size(); j++)
			{
				Cell* cell = fragment->cells[j];
				for (int k = 0; k < cell->faces.size(); k++)
				{
					sf::Vertex* triangle = new sf::Vertex[3];
					triangle[0] = cell->vertices[cell->faces[k].v1];
					triangle[1] = cell->vertices[cell->faces[k].v2];
					triangle[2] = cell->vertices[cell->faces[k].v3];
					window.draw(triangle, 3, sf::Triangles);
				}
			}
		}
	}
	window.display();
}