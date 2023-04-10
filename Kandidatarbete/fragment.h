#pragma once
#include <iostream>
#include <cmath>
#include <cstdlib>
#include<vector>
#include<iomanip>
#include <string>
#include <SFML/Graphics.hpp>
using namespace std;

class Fragment
{
public:
	vector<Cell> cells;
	Material material;
private:
};

class Cell
{
public:
	sf::Vector2f site;
	// faces?
	vector<Cell*> neighbours;
	bool visited;
private:
};

class Material
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