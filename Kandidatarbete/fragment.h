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
	Fragment()
	{
	}
	vector<Cell> cells = vector<Cell>();
	Material material = Material(1, 1);
	float mass = 1;
	void merge(Fragment fragment)
	{

	}
private:
};

class Cell
{
public:
	Cell(sf::Vector2f site, vector<Cell*> neighbours, Fragment* fragment)
	{
		this->site = site;
		this->neighbours = neighbours;
		this->fragment = fragment;
	}
	sf::Vector2f site;
	// faces?
	vector<Cell*> neighbours;
	Fragment* fragment;
	bool visited;
private:
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