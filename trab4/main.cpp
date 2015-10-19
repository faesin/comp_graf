#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <map>
#include <signal.h>
#include <random>

#include <GL/glut.h>
#include "tinyxml2.h"
#include "vec3.h"
#include "objects.h"

using namespace std;
using namespace tinyxml2;

int winID;

vector<Object*> objects;
Chopper *player;
int g_windowSizeX = 0, g_windowSizeY = 0;
double g_bltSpeed = 0.0, g_chpSpeed = 0.0;

short int key_press[256];

random_device rd;
mt19937 eng(rd());
uniform_int_distribution<> distr(0, 360);

int openFile(int argc, char **argv);
void sigCallback(int signum);

void displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for(uint i = 0; i < objects.size(); ++i)
	{
		objects[i]->draw();
	}

	glutSwapBuffers();
}

void reshapeCallback(int w, int h)
{
	glutReshapeWindow(g_windowSizeX, g_windowSizeY);
}

void keyboardCallback(unsigned char key, int x, int y)
{
	key_press[(int)key] = 1;
	// cout << "Key " << key << " is pressed" << endl;
}

void keyUpCallback(unsigned char key, int x, int y)
{
	key_press[(int)key] = 0;
	// cout << "Key " << key << " is up" << endl;
}

void mouseCallback(int button, int state, int x, int y)
{
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		player->changeState();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// objects.push_back(player->shoot());
	}

	glutPostRedisplay();
}

void mousePassiveCallback(int x, int y)
{
	player->moveGun(x, y);
	glutPostRedisplay();
}

void idleCallback()
{
	if(key_press[(int)'a'])
		player->pivot(-player->getTurnSpeed());

	if(key_press[(int)'d'])
		player->pivot(player->getTurnSpeed());

	if(key_press[(int)'w'])
		player->moveFoward();

	if(key_press[(int)'s'])
		player->moveBackward();

	if(key_press[(int)'-'])
		player->decRot();

	if(key_press[(int)'+'])
		player->incRot();

	if(key_press[(int)'1'])
		player->drawHbx();


	static GLdouble previousTime = 0;

	GLdouble currentTime, timeDiference;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	timeDiference = currentTime - previousTime;
	previousTime = currentTime;

	for(uint i = 0; i < objects.size(); ++i)
	{
		Bullet *baux = dynamic_cast<Bullet*>(objects[i]);
		if(baux)
		{
			baux->updatePosition(timeDiference);
		}
	}

	glutPostRedisplay();
}

// void sigCallback(int signum)
// {
// 	glutIdleFunc(NULL);

// 	objects.clear();
// 	glutDestroyWindow(winID);
// 	exit(signum);
// }

int main(int argc, char **argv)
{
	// signal(SIGINT, sigCallback);

	for(uint i = 0; i < 256; ++i)
		key_press[i] = 0;

	int err = openFile(argc, argv);
	
	if(err != 0)
		return err;

	for(uint i = 0; i < objects.size(); ++i)
	{
		int found = 0;
		if(found == 2)
			break;

		if(strstr(objects[i]->getID(), "Arena") != NULL)
		{
			Rectangle *aux = dynamic_cast<Rectangle*>(objects[i]);
			g_windowSizeX = aux->getWidth();
			g_windowSizeY = aux->getHeight();
			found++;
			continue;
		}

		if(strstr(objects[i]->getID(), "Jogador") != NULL)
		{
			Chopper *paux = dynamic_cast<Chopper*>(objects[i]);
			player = paux;
			found++;
			continue;
		}
	}

	if(g_windowSizeX == 0 || g_windowSizeY == 0)
		return -1;
	else
		if(player != NULL)
		{
			//TODO::
		}

	char param[] = "swag";
	char *fakeargv[] = { param, NULL };
	int fakeargc = 1;

	glutInit(&fakeargc, fakeargv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(g_windowSizeX, g_windowSizeY);
	glutInitWindowPosition(100, 100);
	winID = glutCreateWindow("Trabalho 2");

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, g_windowSizeX, g_windowSizeY, 0.0, -1.0, 1.0);
	
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutMouseFunc(mouseCallback);
	glutPassiveMotionFunc(mousePassiveCallback);
	glutKeyboardFunc(keyboardCallback);
	glutKeyboardUpFunc(keyUpCallback);

	glutIdleFunc(idleCallback);

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

	XMLElement *chopperInfoElement = cfg.FirstChildElement()->FirstChildElement("helicoptero");
	if(!chopperInfoElement)
	{
		cerr << "Could not find element: \"helicoptero\"" << endl;
		return cfg.ErrorID();
	}

	chopperInfoElement->QueryDoubleAttribute("velTiro", &g_bltSpeed);
	chopperInfoElement->QueryDoubleAttribute("velHelicoptero", &g_chpSpeed);
	
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

			newObj = new Rectangle(id, x+width/2, y+height/2, width, height, r, g, b, strW, sr, sg, sb);
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

			if(strstr(color, "green") != NULL)
			{
				newObj = new Chopper(id, x, y, radius, 0, g_chpSpeed, g_bltSpeed, 0, 10, 0, 1, 0);
				objects.push_back(newObj);
			}else{
				if(strstr(color, "red") != NULL)
				{
					newObj = new Chopper(id, x, y, radius, 1, g_chpSpeed, g_bltSpeed, distr(eng), 10, 1, 0, 0);
					objects.push_back(newObj);
				}else{
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

					newObj = new Circle(id, x, y, radius, r, g, b);
					objects.push_back(newObj);
				}
			}			
		}

		objectElement = objectElement->NextSiblingElement();
	}while(objectElement != NULL);

	return cfg.ErrorID();
}