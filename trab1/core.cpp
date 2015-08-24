#include "core.h"

Core::Core(int *argcp, char **argv)
{
	char cfgPath[256] =  "./";

	if(*argcp > 1)
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
		cerr << "\t=> Does the file exists, or is it well writen?" << endl;
		throw cfg.ErrorID();
	}

	XMLElement *poligonElement = cfg.FirstChildElement()->FirstChildElement("poligono");
	int numOfVertex;
	if(!poligonElement)
	{
		cerr << "Could not find element: \"poligono\"" << endl;
		throw cfg.ErrorID();
	}

	poligonElement->QueryIntAttribute("numeroDeVertices", &numOfVertex);


	XMLElement *vertexElement = poligonElement->FirstChildElement("vertice");
	if(!vertexElement)
	{
		cerr << "Could not find element: \"vertice\"" << endl;
		throw cfg.ErrorID();
	}

	int id, x, y;
	vertexElement->QueryIntAttribute("id", &id);
	vertexElement->QueryIntAttribute("coordX", &x);
	vertexElement->QueryIntAttribute("coordY", &y);

	this->initPol.push_back(make_pair(id, vec3((double)x, (double)y, 0.0)));

	for(int i = 1; i < numOfVertex; ++i)
	{
		vertexElement = vertexElement->NextSiblingElement("vertice");
		if(!vertexElement)
		{
			cerr << "Could not find element: \"vertice\"" << endl;
			throw cfg.ErrorID();
		}

		vertexElement->QueryIntAttribute("id", &id);
		vertexElement->QueryIntAttribute("coordX", &x);
		vertexElement->QueryIntAttribute("coordY", &y);

		this->initPol.push_back(make_pair(id, vec3((double) x, (double)y, 0.0)));
	}

	for(int i = 0; i < numOfVertex; ++i)
		cout << this->initPol[i].first << " -> " << this->initPol[i].second << endl;

	this->render = new Renderer();
	// Input *inpt = new Input();

}

void Core::run()
{
	this->render->run();
}