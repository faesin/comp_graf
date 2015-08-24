#ifndef _CORE_H_
#define _CORE_H_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <exception>

#include <GL/glut.h>
#include "tinyxml2.h"
#include "vec3.h"
#include "renderer.h"

typedef unsigned int uint;
typedef pair<int, vec3> vertex;
typedef vector<vertex> poligon;

using namespace std;
using namespace tinyxml2;

class Core
{

public:
	Core(int*, char**);
	void run();

	const vector<vertex> getInitPoligon() { return initPol; };

private:
	static int initialized;
	vector<vertex> initPol;
	vector<poligon> poligons;

	Renderer *render;
	//Input *input;
	
	
};


#endif