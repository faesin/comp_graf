#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstring>

#include "tinyxml2.h"


using namespace std;
using namespace tinyxml2;

typedef unsigned int uint;
typedef pair<int,int> coord;
typedef pair<int, coord> vertex;

int main(int argc, char **argv)
{
	char cfgPath[256] =  "./";

	if(argc > 1)
	{
		strcat(cfgPath, argv[1]);
		strcat(cfgPath, "/");
	}

	strcat(cfgPath, "config.xml");

	cout << "Loading file from: " << cfgPath << endl;

	XMLDocument cfg;
	if(cfg.LoadFile(cfgPath) != XMLError(0))
	{
		cerr << "Could not load file" << endl;
		cerr << "Does the file exists, or is it well writen?" << endl;
		return cfg.ErrorID();
	}

	XMLElement *poligonElement = cfg.FirstChildElement()->FirstChildElement("poligono");
	int numOfVertex;
	if(!poligonElement)
	{
		cerr << "Could not file element: \"poligono\"" << endl;
		return cfg.ErrorID();
	}

	poligonElement->QueryIntAttribute("numeroDeVertices", &numOfVertex);

	vector<vertex> poligon(numOfVertex);

	XMLElement *vertexElement = poligonElement->FirstChildElement("vertice");
	if(!vertexElement)
	{
		cerr << "Could not file element: \"vertice\"" << endl;
		return cfg.ErrorID();
	}

	vertexElement->QueryIntAttribute("id", &poligon[0].first);
	vertexElement->QueryIntAttribute("coordX", &poligon[0].second.first);
	vertexElement->QueryIntAttribute("coordY", &poligon[0].second.second);

	for(int i = 1; i < numOfVertex; ++i)
	{
		vertexElement = vertexElement->NextSiblingElement("vertice");
		if(!vertexElement)
		{
			cerr << "Could not file element: \"vertice\"" << endl;
			return cfg.ErrorID();
		}

		vertexElement->QueryIntAttribute("id", &poligon[i].first);
		vertexElement->QueryIntAttribute("coordX", &poligon[i].second.first);
		vertexElement->QueryIntAttribute("coordY", &poligon[i].second.second);
	}

	for(int i = 0; i < numOfVertex; ++i)
		cout << poligon[i].first << " -> X: " << poligon[i].second.first << " , Y: " << poligon[i].second.second << endl;
	

	return 0;
}