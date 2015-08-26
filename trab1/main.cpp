#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <map>

#include <GL/glut.h>
#include "tinyxml2.h"
#include "vec3.h"

using namespace std;
using namespace tinyxml2;

typedef unsigned int uint;
typedef pair<int, vec3> vertex;
typedef vector<vertex> poligon;

poligon g_poligon;
bool g_createPoligon = true;

int g_windowSizeX = 500, g_windowSizeY = 500;
int g_moveVertexID = -1;

int openFile(int argc, char **argv);

void displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(g_createPoligon)
		return;

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	for(uint i = 0; i < g_poligon.size(); ++i)
		glVertex3f(g_poligon[i].second.x, g_poligon[i].second.y, g_poligon[i].second.z);
	glEnd();

	glPointSize(3.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for(uint i = 0; i < g_poligon.size(); ++i)
		glVertex3f(g_poligon[i].second.x, g_poligon[i].second.y, g_poligon[i].second.z);
	glEnd();

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	for(uint i = 0; i < g_poligon.size(); ++i)
		glVertex3f(g_poligon[i].second.x, g_poligon[i].second.y, g_poligon[i].second.z);
	glEnd();

	glFlush();
}

void reshapeCallback(int w, int h)
{

}

void mouseCallback(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if(g_createPoligon)
		{
			cout << "x: " << x << " y: " << y << endl;
			for(uint i = 0; i < g_poligon.size(); ++i)
			{
				g_poligon[i].second.x += x;
				g_poligon[i].second.y += (g_windowSizeY - y);
			}

			g_createPoligon = false;
		}else{

			double dist = 10.0;
			y = (g_windowSizeY - y);

			for(uint i = 0; i < g_poligon.size(); ++i)
			{
				double newDist = sqrt(pow(g_poligon[i].second.x - x, 2.0) + pow(g_poligon[i].second.y - y, 2));
				if(newDist < dist)
				{
					g_moveVertexID = i;
					dist = newDist;
				}
			}
		}
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_moveVertexID = -1;

	glutPostRedisplay();
}

void mouseMoveCallback(int x, int y)
{
	if(g_moveVertexID >= 0)
	{
		g_poligon[g_moveVertexID].second.x = x;
		g_poligon[g_moveVertexID].second.y = (g_windowSizeY - y);
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{

	int err = openFile(argc, argv);
	
	if(err != 0)
		return err;

	char param[] = "swag";
	char *fakeargv[] = { param, NULL };
	int fakeargc = 1;

	glutInit(&fakeargc, fakeargv);

	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(g_windowSizeX, g_windowSizeY);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Trabalho 1");

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, 500.0, 0.0, 500.0, -1.0, 1.0);
	
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMoveCallback);

	glutMainLoop();

	return 0;
}

int openFile(int argc, char **argv)
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
		cerr << "\t=> Does the file exists, or is it well writen?" << endl;
		return cfg.ErrorID();
	}

	XMLElement *poligonElement = cfg.FirstChildElement()->FirstChildElement("poligono");
	int numOfVertex;
	if(!poligonElement)
	{
		cerr << "Could not find element: \"poligono\"" << endl;
		return cfg.ErrorID();
	}

	poligonElement->QueryIntAttribute("numeroDeVertices", &numOfVertex);


	XMLElement *vertexElement = poligonElement->FirstChildElement("vertice");
	if(!vertexElement)
	{
		cerr << "Could not find element: \"vertice\"" << endl;
		return cfg.ErrorID();
	}

	int id, x, y;
	vertexElement->QueryIntAttribute("id", &id);
	vertexElement->QueryIntAttribute("coordX", &x);
	vertexElement->QueryIntAttribute("coordY", &y);

	g_poligon.push_back(make_pair(id, vec3((double)x, (double)y, 0.0)));

	for(int i = 1; i < numOfVertex; ++i)
	{
		vertexElement = vertexElement->NextSiblingElement("vertice");
		if(!vertexElement)
		{
			cerr << "Could not find element: \"vertice\"" << endl;
			return cfg.ErrorID();
		}

		vertexElement->QueryIntAttribute("id", &id);
		vertexElement->QueryIntAttribute("coordX", &x);
		vertexElement->QueryIntAttribute("coordY", &y);

		g_poligon.push_back(make_pair(id, vec3((double) x, (double)y, 0.0)));
	}

	for(int i = 0; i < numOfVertex; ++i)
		cout << g_poligon[i].first << " -> " << g_poligon[i].second << endl;

	return cfg.ErrorID();
}
