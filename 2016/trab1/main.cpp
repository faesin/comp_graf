#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <map>

#include <GL/glut.h>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

typedef unsigned int uint;

int g_windowWidth = 100, g_windowHeight = 100;
GLfloat g_windowBG_R = 1.0, g_windowBG_G = 1.0, g_windowBG_B = 1.0;
char* g_windowTitle;

bool g_drawSquare = false;
GLfloat g_squareSize; 
GLfloat g_square_R = 0.0, g_square_G = 0.0, g_square_B = 0.0;
GLfloat g_square_X, g_square_Y;

bool g_moveSquare = false;
float deltaX, deltaY;

int openFile(int argc, char **argv);

void displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(!g_drawSquare)
	{
		glFlush();
		return;
	}

	glPushMatrix();
		glTranslated(g_square_X, g_square_Y, 0.0);
		glColor3f(g_square_R, g_square_G, g_square_B);
		glBegin(GL_POLYGON);
			glVertex3f(g_squareSize/2, g_squareSize/2, 0.0);
			glVertex3f(g_squareSize/2, -g_squareSize/2, 0.0);
			glVertex3f(-g_squareSize/2, -g_squareSize/2, 0.0);
			glVertex3f(-g_squareSize/2, g_squareSize/2, 0.0);
		glEnd();
	glPopMatrix();

	glFlush();
}

void reshapeCallback(int w, int h)
{

}

void mouseCallback(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if(!g_drawSquare)
		{
			g_square_X = x;
			g_square_Y = y;
			g_drawSquare = true;
		}else{
			if(((x >= g_square_X - g_squareSize/2) && (x <= g_square_X + g_squareSize/2))
				&& ((y >= g_square_Y - g_squareSize/2) && (y <= g_square_Y + g_squareSize/2)))
			{
				deltaX = x - g_square_X;
				deltaY = y - g_square_Y;
				g_moveSquare = true;
			}
		}
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_moveSquare = false;

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		if(g_drawSquare)
			if(((x >= g_square_X - g_squareSize/2) && (x <= g_square_X + g_squareSize/2))
				&& ((y >= g_square_Y - g_squareSize/2) && (y <= g_square_Y + g_squareSize/2)))
			{
				g_drawSquare = false;
			}


	glutPostRedisplay();
}

void mouseMoveCallback(int x, int y)
{
	if(g_moveSquare)
	{
		g_square_X = x - deltaX;
		g_square_Y = y - deltaY;
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{

	int err = openFile(argc, argv);
	
	if(err != 0)
		return err;

	char param[] = "";
	char *fakeargv[] = { param, NULL };
	int fakeargc = 1;

	glutInit(&fakeargc, fakeargv);

	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(g_windowWidth, g_windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(g_windowTitle);

	glClearColor(g_windowBG_R, g_windowBG_G, g_windowBG_B, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//inverted camera at y
	glOrtho(0.0, g_windowWidth, g_windowHeight, 0.0, -1.0, 1.0);
	
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

	XMLElement *windowElement = cfg.FirstChildElement("aplicacao")->FirstChildElement("janela");
	if(!windowElement)
	{
		cerr << "Could not find element: \"janela\"" << endl;
		return cfg.ErrorID();
	}

	windowElement->FirstChildElement("largura")->QueryIntText(&g_windowWidth);
	windowElement->FirstChildElement("altura")->QueryIntText(&g_windowHeight);
	
	// cout << g_windowWidth << g_windowHeight << endl;

	const char* windowTitle = windowElement->FirstChildElement("titulo")->GetText();
	g_windowTitle = new char[strlen(windowTitle)+1]; //TODO:: DELETE THIS OR LEAK 4 DAYZ 
	strcpy(g_windowTitle, windowTitle);
	//cout << windowTitle << strlen(windowTitle) << "\t" << g_windowTitle << strlen(g_windowTitle) << endl;

	XMLElement *bgElement = windowElement->FirstChildElement("fundo");
	if(!bgElement)
	{
		cerr << "Could not find element: \"fundo\"" << endl;
		return cfg.ErrorID();
	}
	bgElement->QueryFloatAttribute("corR", &g_windowBG_R);
	bgElement->QueryFloatAttribute("corG", &g_windowBG_G);
	bgElement->QueryFloatAttribute("corB", &g_windowBG_B);

	// cout << g_windowBG_R << "\t" << g_windowBG_G << "\t" << g_windowBG_B << "\t" << endl;

	XMLElement *squareElement = cfg.FirstChildElement()->FirstChildElement("quadrado");
	if(!squareElement)
	{
		cerr << "Could not find element: \"quadrado\"" << endl;
		return cfg.ErrorID();
	}
	squareElement->QueryFloatAttribute("tamanho", &g_squareSize);
	squareElement->QueryFloatAttribute("corR", &g_square_R);
	squareElement->QueryFloatAttribute("corG", &g_square_G);
	squareElement->QueryFloatAttribute("corB", &g_square_B);

	return cfg.ErrorID();
}
