#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <map>

#include <GL/glut.h>
#include "tinyxml2.h"
#include "vec3.h"
#include "objects.h"

using namespace std;
using namespace tinyxml2;

vector<Object*> objects;
int g_windowSizeX = 0, g_windowSizeY = 0;

int openFile(int argc, char **argv);

void displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for(uint i = 0; i < objects.size(); ++i)
	{
		glColor3d(objects[i]->getR(), objects[i]->getG(), objects[i]->getB());
		objects[i]->draw();
	}

	glutSwapBuffers();
}

void reshapeCallback(int w, int h)
{
	glutReshapeWindow(g_windowSizeX, g_windowSizeY);
}

void mouseCallback(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for(uint i = 0; i < objects.size(); ++i)
		{
			Circle *circle = dynamic_cast<Circle*>(objects[i]);
			if(circle != NULL)
			{
				if(sqrt(pow(circle->getX() - x, 2) + pow(circle->getY() - y, 2)) <= (double)circle->getRad())
					cout << "id=" << objects[i]->getID() << endl;
			}

			Rectangle *rect = dynamic_cast<Rectangle*>(objects[i]);
			if(rect != NULL)
			{
				if(strstr(objects[i]->getID(), "Arena") != NULL)
					continue;

				if((rect->getX() < x && x < rect->getX() + rect->getWidth()) && (rect->getY() < y && y < rect->getY() + rect->getHeight()))
					cout << "id=" << objects[i]->getID() << endl;
			}
		}
	}

	glutPostRedisplay();
}

void mouseMoveCallback(int x, int y)
{
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	int err = openFile(argc, argv);
	
	if(err != 0)
		return err;

	for(uint i = 0; i < objects.size(); ++i)
	{
		if(strstr(objects[i]->getID(), "Arena") != NULL)
		{
			Rectangle *aux = dynamic_cast<Rectangle*>(objects[i]);
			g_windowSizeX = aux->getWidth();
			g_windowSizeY = aux->getHeight();
			break;
		}
	}

	if(g_windowSizeX == 0 || g_windowSizeY == 0)
		return -1;

	char param[] = "swag";
	char *fakeargv[] = { param, NULL };
	int fakeargc = 1;

	glutInit(&fakeargc, fakeargv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(g_windowSizeX, g_windowSizeY);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Trabalho 2");

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, g_windowSizeX, g_windowSizeY, 0.0, -1.0, 1.0);
	
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

	XMLElement *inFilesElement = cfg.FirstChildElement()->FirstChildElement("arquivosDeEntrada");
	if(!inFilesElement)
	{
		cerr << "Could not find element: \"arquivosDeEntrada\"" << endl;
		return cfg.ErrorID();
	}

	XMLElement *arenaFileElement = inFilesElement->FirstChildElement("arquivoDaArena");
	if(!arenaFileElement)
	{
		cerr << "Could not find element: \"arquivoDaArena\"" << endl;
		return cfg.ErrorID();
	}

	const char *arenaPath;
	arenaPath = arenaFileElement->Attribute("caminho");
	
	if(!arenaPath)
	{
		cout << "Could not find arena file path. Exiting..." << endl;
		return -1;
	}

	//Considering that "~/" is at the begining of the path
	if(strstr(arenaPath, "~/") != NULL)
	{
		char *temp = getenv("HOME");
		strcat(temp, arenaPath+1);
		arenaPath = temp;
	}

	char finalPath[1024];
	finalPath[0] = '\0';

	strcat(finalPath, arenaPath);

	strcat(finalPath, arenaFileElement->Attribute("nome"));
	strcat(finalPath, ".");
	strcat(finalPath, arenaFileElement->Attribute("tipo"));
	
	XMLDocument cfgArena;
	if(cfgArena.LoadFile(finalPath) != XMLError(0))
	{
		cerr << "Could not load arena config file" << endl;
		cerr << "\t=> Does the file exists, or is it well writen?" << endl;
		return cfgArena.ErrorID();
	}

	XMLElement *svgElement = cfgArena.FirstChildElement("svg");
	XMLElement *objectElement = svgElement->FirstChildElement();

	do 
	{
		Object *newObj;
		if(strstr(objectElement->Name(), "rect") != NULL)
		{
			int x, y, width, height, strW;
			const char *color, *strColor, *id;
			objectElement->QueryIntAttribute("x", &x);
			objectElement->QueryIntAttribute("y", &y);
			objectElement->QueryIntAttribute("width", &width);
			objectElement->QueryIntAttribute("height", &height);
			color = objectElement->Attribute("fill");
			objectElement->QueryIntAttribute("stroke-width", &strW);
			strColor = objectElement->Attribute("stroke");
			id = objectElement->Attribute("id");

			double r = 0, g = 0, b = 0;
			double sr = 0, sg = 0, sb = 0;
			if(strstr(color, "red") != NULL)
				r = 1;

			if(strstr(color, "green") != NULL)
				g = 1;

			if(strstr(color, "blue") != NULL)
				b = 1;

			if(strstr(color, "white") != NULL)
			{
				r = 1; g = 1; b = 1;
			}

			if(strstr(color, "gray") != NULL)
			{
				r = 0.5; g = 0.5; b = 0.5;
			}

			if(strstr(strColor, "red") != NULL)
				sr = 1;

			if(strstr(strColor, "green") != NULL)
				sg = 1;

			if(strstr(strColor, "blue") != NULL)
				sb = 1;

			if(strstr(strColor, "white") != NULL)
			{
				sr = 1; sg = 1; sb = 1;
			}

			if(strstr(strColor, "grey") != NULL)
			{
				sr = 0.5; sg = 0.5; sb = 0.5;
			}

			newObj = new Rectangle(id, x, y, width, height, r, g, b, strW, sr, sg, sb);
			objects.push_back(newObj);
		}

		if(strstr(objectElement->Name(), "circle") != NULL)
		{
			int x, y, radius;
			const char *color, *id;
			objectElement->QueryIntAttribute("cx", &x);
			objectElement->QueryIntAttribute("cy", &y);
			objectElement->QueryIntAttribute("r", &radius);
			color = objectElement->Attribute("fill");
			id = objectElement->Attribute("id");

			double r = 0, g = 0, b = 0;
			if(strstr(color, "red") != NULL)
				r = 1;

			if(strstr(color, "green") != NULL)
				g = 1;

			if(strstr(color, "blue") != NULL)
				b = 1;

			if(strstr(color, "white") != NULL)
			{
				r = 1; g = 1; b = 1;
			}

			if(strstr(color, "grey") != NULL)
			{
				r = 0.5; g = 0.5; b = 0.5;
			}

			newObj = new Circle(id, x,y,radius,r,g,b);
			objects.push_back(newObj);
		}

		objectElement = objectElement->NextSiblingElement();
	}while(objectElement != NULL);

	return cfg.ErrorID();
}