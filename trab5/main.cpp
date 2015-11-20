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
Rectangle *g_posto;

random_device rd;
mt19937 eng;
uniform_int_distribution<> distr(0, 360);

int g_windowSizeX = 0, g_windowSizeY = 0;
double g_bltSpeed = 0.0, g_chpSpeed = 0.0, g_enemySpeed = 0.0, g_enemySeek = 0.0;
double g_initFuelTime;

int g_winCond = 0;

int once = 1;

short int key_press[256];
char g_winMsg[10] = "YOU WON";
char g_loseMsg[10] = "YOU LOSE";

int openFile(int argc, char **argv);
int checkPlayerColision(vec3 pos);
void makePlayerColision(GLdouble timeDiff);
void makeBulletColision(GLdouble timeDiff);
void makeIAStep(GLdouble timeDiff);

void renderBitmapString(int x, int y, void *font, char *string);
void drawFuel();

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

	if(g_winCond == 1)
		renderBitmapString(g_windowSizeX/2 - 10, g_windowSizeY/2, GLUT_BITMAP_9_BY_15, g_winMsg);

	if(g_winCond == -1)
		renderBitmapString(g_windowSizeX/2 - 10, g_windowSizeY/2, GLUT_BITMAP_9_BY_15, g_loseMsg);

	drawFuel();

	glutSwapBuffers();
}

void reshapeCallback(int w, int h)
{
	glutReshapeWindow(g_windowSizeX, g_windowSizeY);
}

void keyboardCallback(unsigned char key, int x, int y)
{
	key_press[(int)key] = 1;
}

void dumbKeyboardCallback(unsigned char key, int x, int y)
{
	if((int)key == 27)
		key_press[(int)key] = 1;
}

void keyUpCallback(unsigned char key, int x, int y)
{
	key_press[(int)key] = 0;
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
	static GLdouble previousTime = 0;

	GLdouble currentTime, timeDifference;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	timeDifference = currentTime - previousTime;
	previousTime = currentTime;

	if(key_press[(int)'a'])
		g_player->pivot(-g_player->getTurnSpeed(), timeDifference);

	if(key_press[(int)'d'])
		g_player->pivot(g_player->getTurnSpeed(), timeDifference);

	if(key_press[(int)'w'])
	{
		if(!checkPlayerColision(g_player->getNextPosition(1)))
			g_player->moveFoward(timeDifference);
	}

	if(key_press[(int)'s'])
	{
		if(!checkPlayerColision(g_player->getNextPosition(-1)))
			g_player->moveBackward(timeDifference);
	}

	if(key_press[(int)'-'])
		g_player->decRot();

	if(key_press[(int)'+'])
		g_player->incRot();

	// if(key_press[(int)'1'])
	// 	g_player->drawHbx();

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

	bool maybeWin;

	for(vector<Object*>::iterator it = g_objects.begin(); it != g_objects.end(); ++it)
	{
		maybeWin = true;
		if(strstr((*it)->getID(), "ObjetoResgate") != NULL)
			maybeWin = false;
	}

	if(maybeWin)
		g_winCond = 1;

	if(g_winCond == 0)
	{
		g_player->useFuel(timeDifference/1000);

		for(uint i = 0; i < g_bullets.size(); ++i)
			g_bullets[i]->updatePosition(timeDifference);

		makeBulletColision(timeDifference);
		makePlayerColision(timeDifference);

		makeIAStep(timeDifference);
	}

	if(g_winCond == 1)
	{
		if(once)
		{
			cout << "ganhou" << endl;
			once = 0;
		}

		glutMotionFunc(NULL);
		glutPassiveMotionFunc(NULL);
		glutKeyboardFunc(dumbKeyboardCallback);

		for(uint i = 0; i < 256; ++i)
			key_press[i] = 0;
	}

	if(g_player->getFuel() <= 0)
		g_winCond = -1;

	if(g_winCond == -1)
	{
		if(once)
		{
			cout << "Perdeu" << endl;
			once = 0;
		}

		glutMotionFunc(NULL);
		glutPassiveMotionFunc(NULL);
		glutKeyboardFunc(dumbKeyboardCallback);

		for(uint i = 0; i < 256; ++i)
			key_press[i] = 0;
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

	for(uint i = 0; i < g_objects.size(); ++i)
	{
		if(strstr(g_objects[i]->getID(), "PostoAbastecimento") != NULL)
		{
			Rectangle *aux = dynamic_cast<Rectangle*>(g_objects[i]);
			g_posto = aux;
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
	glutInitWindowPosition(10, 10);
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
		cerr << "\t=> Does the file (" << cfgPath << ") exists, or is it well writen?" << endl;
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
	chopperInfoElement->QueryDoubleAttribute("tempoDeVoo", &g_initFuelTime);

	cout << g_initFuelTime << endl;

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
				g_player = new Chopper(id, x, y, radius, 0, g_chpSpeed, g_bltSpeed, 0, 10, g_initFuelTime, 0, 1, 0);
			}else{
				if(strstr(color, "red") != NULL)
				{
					g_choppers.push_back(new Chopper(id, x, y, radius, 1, g_enemySpeed, g_bltSpeed, distr(eng), 10, g_initFuelTime, 1, 0, 0));
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

void makePlayerColision(GLdouble timeDiff)
{
	for(vector<Object*>::iterator it = g_objects.begin(); it != g_objects.end();)
	{
		bool erased = false;
		if(strstr((*it)->getID(), "ObjetoResgate") != NULL)
		{
			Circle *rescueObj = dynamic_cast<Circle*>(*it);
			if(rescueObj)
			{
				int dx = rescueObj->getX() - g_player->getX();
				int dy = rescueObj->getY() - g_player->getY();

				double dist = sqrt(dx * dx + dy * dy);

				if(dist <= rescueObj->getRad() + g_player->getHitboxRad())
				{
					g_objects.erase(it);
					delete rescueObj;
					erased = true;
				}
			}
		}

		if(!erased)
			++it;
	}

	if(g_player->getState() == 0)
	{
		vec3 aux(g_posto->getX() - g_posto->getWidth()/2, g_posto->getY() - g_posto->getHeight()/2, 0);
		int width = g_posto->getWidth(),
			height = g_posto->getHeight();

		if((g_player->getX() >= aux.x && g_player->getX() <= aux.x + width)
			&& (g_player->getY() >= aux.y && g_player->getY() <= aux.y + height))
		{
			g_player->refuel(timeDiff*2/1000);
		}
	}
}

void makeIAStep(GLdouble timeDiff)
{
	for(vector<Chopper*>::iterator cIt = g_choppers.begin(); cIt != g_choppers.end(); ++cIt)
	{
		bool colide = false;
		(*cIt)->doStep();
		
		if((*cIt)->getCurrInstr() == SEEK)
		{
			//vec3 vet((*cIt)->getX(), (*cIt)->getY(), 0);
			// int quad = 0;
			// cout << vet << endl;
			// cout << atan2(vet.y, vet.x) << endl;

			// if(vet.x <= g_player->getX() && vet.y >= g_player->getY())
			// 	quad = 1;
			// if(vet.x <= g_player->getX() && vet.y < g_player->getY())
			// 	quad = 2;
			// if(vet.x > g_player->getX() && vet.y < g_player->getY())
			// 	quad = 3;
			// if(vet.x < g_player->getX() && vet.y >= g_player->getY())
			// 	quad = 4;

			// cout << quad << endl;

			Bullet *aux = (*cIt)->shoot();
			if(aux != NULL)
				g_bullets.push_back(aux);
			
			(*cIt)->setSeekDone();


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
				(*cIt)->moveByIA(timeDiff);
			}			
		}

	}
}

void makeBulletColision(GLdouble timeDiff)
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
				if(g_player->getState())
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

void renderBitmapString(int x, int y, void *font, char *string)
{
	glColor3f(0, 0, 0);
	char *c;
	glRasterPos2f(x, y);
	for(c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void drawFuel()
{
	int height = 20;
	int boxwidth = 100;
	GLdouble currWidth = g_player->getFuel()*100/g_initFuelTime;
	glColor3d(0.75, 0, 0.75);

	glPushMatrix();
		glTranslated(g_windowSizeX - 120, g_windowSizeY - 30, 0);

		glBegin(GL_POLYGON);
			glVertex3d(0, 0, 0.0);
			glVertex3d(currWidth, 0, 0.0);
			glVertex3d(currWidth, height, 0.0);
			glVertex3d(0, height, 0.0);
		glEnd();

		glColor3d(0, 0, 0);
		glLineWidth(1);
		glBegin(GL_LINE_LOOP);
			glVertex3d(0, 0, 0.0);
			glVertex3d(boxwidth, 0, 0.0);
			glVertex3d(boxwidth, height, 0.0);
			glVertex3d(0, height, 0.0);
		glEnd();

		glPointSize(1);
		glBegin(GL_POINTS);
			glVertex3d(0, 0, 0.0);
			glVertex3d(boxwidth, 0, 0.0);
			glVertex3d(boxwidth, height, 0.0);
			glVertex3d(0, height, 0.0);
		glEnd();

	glPopMatrix();
}