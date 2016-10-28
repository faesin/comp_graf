#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <map>

#include <GL/glut.h>
#include "tinyxml2.h"
#include "objects.h"

using namespace std;
using namespace tinyxml2;

typedef unsigned int uint;

int g_windowWidth = 0, g_windowHeight = 0;
pair<int,int> g_orthoX, g_orthoY;
GLfloat g_windowBG_R = 1.0, g_windowBG_G = 1.0, g_windowBG_B = 1.0;
char g_windowTitle[] = "trabalhocg";

vector<Circle*> g_arena;
Object* g_start;

vector<Object*> g_objects;
Car* g_player;

vector<Bullet*> g_bullets;

float g_carSpeed = 0, g_bulletSpeed = 0;

int keyStatus[256];

int openFile(int argc, char **argv);

bool checkPlayerCollision(vec3 nextPos);
void makeBulletColision(GLdouble timeDiff);

void keyDownCallback(unsigned char key, int x, int y)
{
	keyStatus[(int)key] = 1;

	if(keyStatus[27] == 1)
		exit(0);

	glutPostRedisplay();
}

void keyUpCallback(unsigned char key, int x, int y)
{
	keyStatus[(int)key] = 0;
	glutPostRedisplay();
}

void idleCallback()
{
	static GLdouble previousTime = 0;

	GLdouble currentTime, timeDiff;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	timeDiff = currentTime - previousTime;
	previousTime = currentTime;

	if(keyStatus[(int)'w'] == 1)
	{
		// vec3 aux(g_player->getX(), g_player->getY() + g_player->getSpeed(), 0);
		if(!checkPlayerCollision(g_player->getNextPosition(1, timeDiff)))
			g_player->moveFoward(timeDiff);
	} 

	if(keyStatus[(int)'s'] == 1)
	{	
		// vec3 aux(g_player->getX(), g_player->getY() - g_player->getSpeed(), 0);
		if(!checkPlayerCollision(g_player->getNextPosition(-1, timeDiff)))
			g_player->moveBackward(timeDiff);
	}

	if(keyStatus[(int)'a'] == 1)
	{
		// vec3 aux(g_player->getX() - g_player->getSpeed(), g_player->getY(), 0);
		// if(!checkPlayerCollision(aux))
		// 	g_player->moveSides(LEFT);
		g_player->moveWheels(g_player->getSpeed(), timeDiff);
	} 

	if(keyStatus[(int)'d'] == 1)
	{
		// vec3 aux(g_player->getX() + g_player->getSpeed(), g_player->getY(), 0);
		// if(!checkPlayerCollision(aux))
		// 	g_player->moveSides(RIGHT);
		g_player->moveWheels(-g_player->getSpeed(), timeDiff);
	}

	for(uint i = 0; i < g_bullets.size(); ++i)
		g_bullets[i]->updatePosition(timeDiff);

	makeBulletColision(timeDiff);

	glutPostRedisplay();
}

void displayCallback(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for(uint i = 0; i < g_arena.size(); ++i)
		g_arena[i]->draw();

	g_start->draw();

	for(uint i = 0; i < g_objects.size(); ++i)
		g_objects[i]->draw();

	g_player->draw();

	for(uint i = 0; i < g_bullets.size(); ++i)
		g_bullets[i]->draw();

	glutSwapBuffers();
}

void reshapeCallback(int w, int h)
{
	glutReshapeWindow(g_windowWidth, g_windowHeight);
}

void mouseCallback(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		Bullet *aux = g_player->shoot();
		if(aux != NULL)
			g_bullets.push_back(aux);
	}
	glutPostRedisplay();
}

void mouseMoveCallback(int x, int y)
{
	g_player->moveCannon(x, y);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	for(int i = 0; i < 256; ++i)
		keyStatus[i] = 0;

	int err = openFile(argc, argv);
	
	if(err != 0)
		return err;

	char param[] = "";
	char *fakeargv[] = { param, NULL };
	int fakeargc = 1;

	glutInit(&fakeargc, fakeargv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(g_windowWidth, g_windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(g_windowTitle);

	glClearColor(g_windowBG_R, g_windowBG_G, g_windowBG_B, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(g_orthoX.first, g_orthoX.second, g_orthoY.first, g_orthoY.second, -1.0, 1.0);
	
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutMouseFunc(mouseCallback);
	glutPassiveMotionFunc(mouseMoveCallback);
	glutKeyboardFunc(keyDownCallback);
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

	XMLDocument cfg;
	if(cfg.LoadFile(cfgPath) != XMLError(0))
	{
		cerr << "Could not load file" << endl;
		cerr << "\t=> Does the file exists, or is it well writen?" << endl;
		return cfg.ErrorID();
	}

	XMLElement *arenaFileElement = cfg.FirstChildElement()->FirstChildElement("arquivoDaArena");
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

	// cout << finalPath << endl;

	XMLElement *carInfoElement = cfg.FirstChildElement()->FirstChildElement("carro");
	if(!carInfoElement)
	{
		cout << "Could not find attribute: \"carro\". Exiting..." << endl;
		return -1;
	}
	carInfoElement->QueryFloatAttribute("velTiro", &g_bulletSpeed);
	carInfoElement->QueryFloatAttribute("velCarro", &g_carSpeed);


	XMLDocument cfgArena;
	if(cfgArena.LoadFile(finalPath) != XMLError(0))
	{
		cerr << "Could not load arena config file" << endl;
		cerr << "\t=> Does the file exists, or is it well writen?" << endl;
		return cfgArena.ErrorID();
	}

	XMLElement *objectElement = cfgArena.FirstChildElement("svg")->FirstChildElement();

	do{
		if(strstr(objectElement->Name(), "rect") != NULL)
		{
			int x, y, width, height;
			const char *color, *id;
			objectElement->QueryIntAttribute("x", &x);
			objectElement->QueryIntAttribute("y", &y);
			objectElement->QueryIntAttribute("width", &width);
			objectElement->QueryIntAttribute("height", &height);
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

			if(strstr(color, "gray") != NULL)
			{
				r = 0.5; g = 0.5; b = 0.5;
			}

			if(strstr(id, "LargadaChegada") != NULL)
				g_start = new Rectangle(id, x+width/2, y+height/2, width, height, r, g, b);
			else
				g_objects.push_back(new Rectangle(id, x+width/2, y+height/2, width, height, r, g, b));
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

			if((strstr(id, "Pista") != NULL) && (g_windowWidth < radius*2))
			{
				g_windowWidth = g_windowHeight = radius*2;

				g_orthoX = make_pair(x-radius, x+radius);
				g_orthoY = make_pair(y-radius, y+radius);
			}
			
			if(strstr(color, "green") != NULL)
			{
				g_player = new Car(id, x, y, radius, 0, g_carSpeed, g_bulletSpeed, 0, 1, 0);
			}else{

				if(strstr(color, "red") != NULL)
				{
					// g_objects.push_back(new Car(id, x, y, radius, 0, g_carSpeed, 1, 0, 0));
					g_objects.push_back(new Circle(id, x, y, radius, 1, 0, 0));
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

					if(strstr(id, "Pista") != NULL)
						g_arena.push_back(new Circle(id, x, y, radius, r, g, b));
					else
						g_objects.push_back(new Circle(id, x, y, radius, r, g, b));
				}				
			}
		}

		objectElement = objectElement->NextSiblingElement();
	}while(objectElement != NULL);

	return cfg.ErrorID();
}

bool checkPlayerCollision(vec3 nextPos)
{
	double maxRadius, minRadius;

	maxRadius = g_arena[0]->getRadius();
	if(g_arena[0]->getRadius() > g_arena[1]->getRadius())
	{
		maxRadius = g_arena[0]->getRadius();
		minRadius = g_arena[1]->getRadius();
	}else{
		minRadius = g_arena[0]->getRadius();
		maxRadius = g_arena[1]->getRadius();
	}

	double dx = g_arena[0]->getX() - nextPos.x;
	double dy = g_arena[0]->getY() - nextPos.y;

	double dist = sqrt(dx * dx + dy * dy);
	if((dist <= minRadius + g_player->getHitboxRadius()) || (dist >= maxRadius - g_player->getHitboxRadius()))
		return true;

	for(uint i = 0; i < g_objects.size(); ++i)
	{
		// Car *aux = dynamic_cast<Car*>(g_objects[i]);
		Circle *aux = dynamic_cast<Circle*>(g_objects[i]);

		if(aux != NULL)
		{
			dx = aux->getX() - nextPos.x;
			dy = aux->getY() - nextPos.y;

			dist = sqrt(dx * dx + dy * dy);
			if(dist <= aux->getRadius() + g_player->getHitboxRadius())
				return true;
		}
	}

	return false;
}

void makeBulletColision(GLdouble timeDiff)
{
	vector<Bullet*>::iterator bIt;
	
	for(bIt = g_bullets.begin(); bIt != g_bullets.end();)
	{
		bool bErased = false;
		// if(strstr((*bIt)->getOwner(), "Jogador") != NULL)
		// {
		// 	vector<Chopper*>::iterator cIt;
		// 	for(cIt = g_choppers.begin(); cIt != g_choppers.end(); ++cIt)
		// 	{
		// 		int dx = (*cIt)->getX() - (*bIt)->getX();
		// 		int dy = (*cIt)->getY() - (*bIt)->getY();

		// 		double dist = sqrt(dx * dx + dy * dy);

		// 		if(dist <= (*cIt)->getHitboxRad() + (*bIt)->getHitboxRad())
		// 		{
		// 			Bullet *baux = *bIt;
		// 			g_bullets.erase(bIt);
		// 			delete baux;

		// 			Chopper *caux = *cIt;
		// 			g_choppers.erase(cIt);
		// 			delete caux;

		// 			bErased = true;
		// 			break;
		// 		}
		// 	}
		// }else{
		// 	int dx = g_player->getX() - (*bIt)->getX();
		// 	int dy = g_player->getY() - (*bIt)->getY();

		// 	double dist = sqrt(dx * dx + dy * dy);

		// 	if(dist <= g_player->getHitboxRad() + (*bIt)->getHitboxRad())
		// 	{
		// 		if(g_player->getState())
		// 			g_winCond = -1;
		// 	}
		// }

		// if(bErased)
		// 	continue;
		
		// cout << g_orthoY.first << " " << g_orthoY.second << endl;

		if(((*bIt)->getX() < g_orthoX.first || (*bIt)->getX() > g_orthoX.second)
			|| ((*bIt)->getY() < g_orthoY.first || (*bIt)->getY() > g_orthoY.second))
		{
			Bullet *aux = *bIt;
			// cout << "delete bullet from " << aux->getOwner() << endl;
			g_bullets.erase(bIt);
			delete aux;
			bErased = true;
		}

		if(!bErased)
			++bIt;
	}
}