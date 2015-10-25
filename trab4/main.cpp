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

int winID;

vector<Object*> g_objects;
vector<Bullet*> g_bullets;
vector<Chopper*> g_choppers;

Chopper *g_player;

random_device rd;
mt19937 eng;
uniform_int_distribution<> distr(0, 360);

int g_windowSizeX = 0, g_windowSizeY = 0;
double g_bltSpeed = 0.0, g_chpSpeed = 0.0, g_enemySpeed = 0.0, g_enemySeek = 0.0;

int g_winCond = 0;

int once = 1;

short int key_press[256];

int openFile(int argc, char **argv);
int checkPlayerColision(vec3 pos);
void makeBulletColision();
void makeIAStep();

void displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for(uint i = 0; i < g_objects.size(); ++i)
		g_objects[i]->draw();

	for(uint i = 0; i < g_bullets.size(); ++i)
		g_bullets[i]->draw();

	g_player->draw();

	for(uint i = 0; i < g_choppers.size(); ++i)
		g_choppers[i]->draw();

	glutSwapBuffers();
}

void reshapeCallback(int w, int h)
{
	glutReshapeWindow(g_windowSizeX, g_windowSizeY);
}

void keyboardCallback(unsigned char key, int x, int y)
{
	key_press[(int)key] = 1;
	// cout << "Key " << (int)key << " is pressed" << endl;
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
		g_player->changeState();
	}

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		Bullet *aux = g_player->shoot();
		if(aux != NULL)
			g_bullets.push_back(aux);
	}

	glutPostRedisplay();
}

void mousePassiveCallback(int x, int y)
{
	g_player->moveGun(x, y);
	glutPostRedisplay();
}

void idleCallback()
{
	if(key_press[(int)'a'])
		g_player->pivot(-g_player->getTurnSpeed());

	if(key_press[(int)'d'])
		g_player->pivot(g_player->getTurnSpeed());

	if(key_press[(int)'w'])
	{
		if(!checkPlayerColision(g_player->getNextPosition(1)))
			g_player->moveFoward();
	}

	if(key_press[(int)'s'])
	{
		if(!checkPlayerColision(g_player->getNextPosition(-1)))
			g_player->moveBackward();
	}

	if(key_press[(int)'-'])
		g_player->decRot();

	if(key_press[(int)'+'])
		g_player->incRot();

	if(key_press[(int)'1'])
		g_player->drawHbx();

	if(key_press[(int)'2'] && once)
	{
		Chopper* aux = g_player;
		g_player = g_choppers.back();
		g_choppers.pop_back();
		g_choppers.push_back(aux);

		once = 0;
	}

	if(key_press[(int)'3'])
	{
		makeIAStep();
	}

	if(key_press[27])
	{
		for(uint i = 0; i < g_objects.size(); ++i)
			delete g_objects[i];
		for(uint i = 0; i < g_choppers.size(); ++i)
			delete g_choppers[i];
		for(uint i = 0; i < g_bullets.size(); ++i)
			delete g_bullets[i];

		g_objects.clear();
		g_choppers.clear();
		g_bullets.clear();

		delete g_player;

		exit(0);
	}

	static GLdouble previousTime = 0;

	GLdouble currentTime, timeDiference;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	timeDiference = currentTime - previousTime;
	previousTime = currentTime;

	for(uint i = 0; i < g_bullets.size(); ++i)
		g_bullets[i]->updatePosition(timeDiference);

	makeBulletColision();
	// makeIAStep();

	// cout << g_bullets.size() << endl;

	if(g_winCond == -1)
	{
		cout << "YOU LOSE" << endl;
		exit(0);
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	for(uint i = 0; i < 256; ++i)
		key_press[i] = 0;

	int err = openFile(argc, argv);
	
	if(err != 0)
		return err;

	for(uint i = 0; i < g_objects.size(); ++i)
	{
		if(strstr(g_objects[i]->getID(), "Arena") != NULL)
		{
			Rectangle *aux = dynamic_cast<Rectangle*>(g_objects[i]);
			g_windowSizeX = aux->getWidth();
			g_windowSizeY = aux->getHeight();
			break;
		}
	}

	if(g_windowSizeX == 0 || g_windowSizeY == 0)
		return -1;
	else
		if(g_player != NULL)
			g_player->setLimits(g_windowSizeX, g_windowSizeY);

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
	glutMotionFunc(mousePassiveCallback);
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

	XMLElement *enemyChopperInfoElement = cfg.FirstChildElement()->FirstChildElement("helicopteroInimigo");
	if(!enemyChopperInfoElement)
	{
		cerr << "Could not find element: \"helicopteroInimigo\"" << endl;
		return cfg.ErrorID();
	}

	enemyChopperInfoElement->QueryDoubleAttribute("freqTiro", &g_enemySeek);
	enemyChopperInfoElement->QueryDoubleAttribute("velHelicoptero", &g_enemySpeed);
	
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

			g_objects.push_back(new Rectangle(id, x+width/2, y+height/2, width, height, r, g, b, strW, sr, sg, sb));
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
				g_player = new Chopper(id, x, y, radius, 0, g_chpSpeed, g_bltSpeed, 0, 10, 0, 1, 0);
			}else{
				if(strstr(color, "red") != NULL)
				{
					g_choppers.push_back(new Chopper(id, x, y, radius, 1, g_enemySpeed, g_bltSpeed, distr(eng), 10, 1, 0, 0));
					g_choppers.back()->drawHbx();
					g_choppers.back()->setIntel(new IA(g_enemySeek));
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

					g_objects.push_back(new Circle(id, x, y, radius, r, g, b));
				}
			}			
		}

		objectElement = objectElement->NextSiblingElement();
	}while(objectElement != NULL);

	return cfg.ErrorID();
}

int checkPlayerColision(vec3 pos)
{
	for(uint i = 0; i < g_choppers.size(); ++i)
	{
		int dx = g_choppers[i]->getX() - pos.x;
		int dy = g_choppers[i]->getY() - pos.y;

		double dist = sqrt(dx * dx + dy * dy);

		if(dist <= g_choppers[i]->getHitboxRad() + g_player->getHitboxRad())
			return 1;
	}

	return 0;
}

void makeIAStep()
{
	for(vector<Chopper*>::iterator cIt = g_choppers.begin(); cIt != g_choppers.end(); ++cIt)
	{
		bool colide = false;
		(*cIt)->doStep();
		
		if((*cIt)->getCurrInstr() == SEEK)
		{
			vec3 pos((*cIt)->getX(), (*cIt)->getY(), 0);
			int quad = 0;
			if(pos.x < )
		}else{
			vec3 pos = (*cIt)->getNextPosition(0);

			if(g_player->getState())
			{
				int dx = pos.x - g_player->getX();
				int dy = pos.y - g_player->getY();

				double dist = sqrt(dx * dx + dy * dy);

				if(dist <= (*cIt)->getHitboxRad() + g_player->getHitboxRad())
				{
					colide = true;
				}
			}

			for(vector<Chopper*>::iterator aIt = g_choppers.begin(); aIt != g_choppers.end(); ++aIt)
			{
				if(cIt != aIt)
				{
					vec3 pos2 = (*aIt)->getNextPosition(0);

					int dx = pos.x - pos2.x;
					int dy = pos.y - pos2.y;

					double dist = sqrt(dx * dx + dy * dy);

					if(dist <= (*cIt)->getHitboxRad() + (*aIt)->getHitboxRad())
					{
						colide = true;
					}
				}
			}

			if(!colide)
			{
				(*cIt)->moveByIA();
			}			
		}

	}
}

void makeBulletColision()
{
	vector<Bullet*>::iterator bIt;
	
	for(bIt = g_bullets.begin(); bIt != g_bullets.end();)
	{
		bool bErased = false;
		if(strstr((*bIt)->getOwner(), "Jogador") != NULL)
		{
			vector<Chopper*>::iterator cIt;
			for(cIt = g_choppers.begin(); cIt != g_choppers.end(); ++cIt)
			{
				int dx = (*cIt)->getX() - (*bIt)->getX();
				int dy = (*cIt)->getY() - (*bIt)->getY();

				double dist = sqrt(dx * dx + dy * dy);

				if(dist <= (*cIt)->getHitboxRad() + (*bIt)->getHitboxRad())
				{
					Bullet *baux = *bIt;
					g_bullets.erase(bIt);
					delete baux;

					Chopper *caux = *cIt;
					g_choppers.erase(cIt);
					delete caux;

					bErased = true;
					break;
				}
			}
		}else{
			int dx = g_player->getX() - (*bIt)->getX();
			int dy = g_player->getY() - (*bIt)->getY();

			double dist = sqrt(dx * dx + dy * dy);

			if(dist <= g_player->getHitboxRad() + (*bIt)->getHitboxRad())
			{
				g_winCond = -1;
			}
		}

		if(bErased)
			continue;
		
		if(((*bIt)->getX() < 0 || (*bIt)->getX() > g_windowSizeX)
			|| ((*bIt)->getY() < 0 || (*bIt)->getY() > g_windowSizeY))
		{
			Bullet *aux = *bIt;
			g_bullets.erase(bIt);
			delete aux;
			bErased = true;
		}

		if(!bErased)
			++bIt;
	}
}