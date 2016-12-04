#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstring>
#include <map>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "tinyxml2.h"
#include "objects.h"
#include "imageloader.h"

using namespace std;
using namespace tinyxml2;

typedef unsigned int uint;

int g_windowWidth = 0, g_windowHeight = 0;
pair<int,int> g_orthoX, g_orthoY;
GLfloat g_windowBG_R = 1.0, g_windowBG_G = 1.0, g_windowBG_B = 1.0;
char g_windowTitle[] = "trabalhocg";

vector<Circle*> g_arena;
Rectangle* g_start;

vector<Object*> g_objects;
vector<Car*> g_cars;
Car* g_player;

vector<Bullet*> g_bullets;

float	g_carSpeed = 0, g_bulletSpeed = 0,
		g_enCarSpeed = 0, g_enBulletSpeed = 0,
		g_enShootFreq = 0;

bool inStart = true;
int laps = 0;

int keyStatus[256];
int g_winCond = 0;

short int cameraState = 1;
bool dayTime = true;

bool gameStart = false, initialized = false;

double elapsedTime = 0; //In millisec

char g_winMsg[10] = "GANHOU!"; 
char g_loseMsg[10] = "PERDEU!"; 

GLuint textureTrack;
GLuint textureGrass;
GLuint textureFinishLine;

int openFile(int argc, char **argv);

bool checkPlayerCollision(vec3 nextPos);
bool checkBotCollision(uint index, vec3 nextPos);
void makeBulletColision(GLdouble timeDiff);
void checkLaps();
void renderBitmapString(int x, int y, void *font, char *string);
GLuint loadTextureRAW(const char* filename);

void keyDownCallback(unsigned char key, int x, int y)
{
	keyStatus[(int)key] = 1;

	if(!gameStart)
		gameStart = !gameStart;

	if(keyStatus[27] == 1)
		exit(0);

	glutPostRedisplay();
}

void keyUpCallback(unsigned char key, int x, int y)
{
	keyStatus[(int)key] = 0;
	glutPostRedisplay();
}

void dumbIdleCallback()
{
}

void idleCallback()
{
	if(gameStart && !initialized)
	{
		for(uint i = 0; i < g_cars.size(); ++i)
			g_cars[i]->startIA();

		initialized = true;
	}

	static GLdouble previousTime = 0;

	GLdouble currentTime, timeDiff;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	timeDiff = currentTime - previousTime;
	previousTime = currentTime;

	if(gameStart)
		elapsedTime += timeDiff;

	if(g_winCond == -1)
		glutIdleFunc(dumbIdleCallback);

	if(laps >= 1)
	{
		g_winCond = 1;
		glutIdleFunc(dumbIdleCallback);
	}	

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

	if(keyStatus[(int)'1'] == 1)
		cameraState = 1;

	if(keyStatus[(int)'2'] == 1)
		cameraState = 2;

	if(keyStatus[(int)'3'] == 1)
		cameraState = 3;

	if(keyStatus[(int)'n'] == 1)
		dayTime = !dayTime;

	for(uint i = 0; i < g_bullets.size(); ++i)
		g_bullets[i]->updatePosition(timeDiff);

	for(uint i = 0; i < g_cars.size(); ++i)
	{
		g_cars[i]->think(timeDiff);
		int nInstr = g_cars[i]->getNextInstr();
		//cout << nInstr << endl;
		switch(nInstr)
		{
			case SHOOT:
			{
				Bullet *aux = g_cars[i]->shoot();
				g_bullets.push_back(aux);
				break;
			}
			case FORWARD:
			{
				if(!checkBotCollision(i, g_cars[i]->getNextPosition(1, timeDiff)))
				{
					g_cars[i]->moveFoward(timeDiff);
					//TODO:: If it collides?
				}
				break;
			}
			case TURNFWRD_R:
			{
				g_cars[i]->moveWheels(-g_cars[i]->getSpeed(), timeDiff);
				//if(g_cars[i]->getWheelYaw() < -2)
				//	g_cars[i]->setWheelYaw(-2);
				if(g_cars[i]->getWheelYaw() < 5)
					g_cars[i]->setWheelYaw(5);


				if(!checkBotCollision(i, g_cars[i]->getNextPosition(1, timeDiff)))
					g_cars[i]->moveFoward(timeDiff);

				break;
			}
			case TURNFWRD_L:
			{
				g_cars[i]->moveWheels(g_cars[i]->getSpeed(), timeDiff);
				if(g_cars[i]->getWheelYaw() > 9)
					g_cars[i]->setWheelYaw(9);

				if(!checkBotCollision(i, g_cars[i]->getNextPosition(1, timeDiff)))
					g_cars[i]->moveFoward(timeDiff);

				break;
			}	
		}
	}

	makeBulletColision(timeDiff);
	checkLaps();

	glutPostRedisplay();
}

void displayCallback(void)
{
	glClearColor(g_windowBG_R, g_windowBG_G, g_windowBG_B, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(g_orthoX.first, g_orthoX.second, g_orthoY.first, g_orthoY.second, -1.0, 1.0);

	if(dayTime)
	{
		GLfloat sun_position[] = {(float)(g_orthoX.second - g_orthoX.first)/2, (float)(g_orthoY.second - g_orthoY.first)/2, 1, 0};
		glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
		glEnable(GL_LIGHT0);
	}

	/*
	switch(cameraState)
	{
		case 1:
			gluLookAt(g_player->getX(), g_player->getY(), g_player->getZ())
			break;
		case 2:
			break;
		case 3:
			break;
	}*/


	for(uint i = 0; i < g_arena.size(); ++i)
		g_arena[i]->draw();
	/*
	g_start->draw();

	for(uint i = 0; i < g_objects.size(); ++i)
		g_objects[i]->draw();

	for(uint i = 0; i < g_cars.size(); ++i)
		g_cars[i]->draw();

	g_player->draw();

	for(uint i = 0; i < g_bullets.size(); ++i)
		g_bullets[i]->draw();

	char buffer[256];
	sprintf(buffer, "%d", (int)elapsedTime/1000);
	renderBitmapString(g_orthoX.second - 25, g_orthoY.second - 15, GLUT_BITMAP_9_BY_15, buffer);
	
	switch(g_winCond)
	{
		case 1:
			renderBitmapString((g_orthoX.second - g_orthoX.first)/2, (g_orthoY.second - g_orthoY.first)/2, GLUT_BITMAP_9_BY_15, g_winMsg);
			break;
		case -1:
			renderBitmapString((g_orthoX.second - g_orthoX.first)/2, (g_orthoY.second - g_orthoY.first)/2, GLUT_BITMAP_9_BY_15, g_loseMsg);
			break;
	}
	*/

	glutSwapBuffers();
}

void reshapeCallback(int w, int h)
{
	glutReshapeWindow(g_windowWidth, g_windowHeight);
}

void mouseCallback(int button, int state, int x, int y)
{
	//cout << "x: " << x << "\ty:" << y << endl;
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

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(g_windowWidth, g_windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(g_windowTitle);

	glClearColor(g_windowBG_R, g_windowBG_G, g_windowBG_B, 1.0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(g_orthoX.first, g_orthoX.second, g_orthoY.first, g_orthoY.second, -1.0, 1.0);
	
	textureTrack = loadTextureRAW("asphalt.bmp");
	if(textureTrack == 0)
	{
		cerr << "Could not bind texture \"asphalt.bmp\"; Exiting... " << endl;
		return 1;
	}
	textureGrass = loadTextureRAW("grass.bmp");

	if(textureGrass == 0)
	{
		cerr << "Could not bind texture \"grass.bmp\"; Exiting... " << endl;
		return 1;
	}
	g_arena[0]->setTexture(textureTrack);
	g_arena[1]->setTexture(textureGrass);

	textureFinishLine = loadTextureRAW("checker.bmp");


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
		cerr << "Could not find arena file path. Exiting..." << endl;
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
		cerr << "Could not find attribute: \"carro\". Exiting..." << endl;
		return -1;
	}
	carInfoElement->QueryFloatAttribute("velTiro", &g_bulletSpeed);
	carInfoElement->QueryFloatAttribute("velCarro", &g_carSpeed);

	XMLElement *enemyInfoElement = cfg.FirstChildElement()->FirstChildElement("carroInimigo");
	if(!carInfoElement)
	{
		cerr << "Could not find attribute: \"carroInimigo\". Exiting..." << endl;
		return -1;
	}
	enemyInfoElement->QueryFloatAttribute("velTiro", &g_enBulletSpeed);
	enemyInfoElement->QueryFloatAttribute("velCarro", &g_enCarSpeed);
	enemyInfoElement->QueryFloatAttribute("freqTiro", &g_enShootFreq);

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
				g_start = new Rectangle(id, x+width/2, y+height/2, 0, width, height, r, g, b);
			else
				g_objects.push_back(new Rectangle(id, x+width/2, y+height/2, 0, width, height, r, g, b));
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
				g_player = new Car(id, x, y, 0.5, radius, 0, g_carSpeed, g_bulletSpeed, 0, 1, 0);
			}else{

				if(strstr(color, "red") != NULL)
				{
					double	dx = g_arena[0]->getX() - x,
							dy = g_arena[0]->getY() - y,
							yaw = 0;

					yaw = atan2(dx,dy) * 180.0/M_PI;
					//cout << yaw << endl;
					Car* enemy = new Car(id, x, y, 0.5, radius, 270 - yaw, g_enCarSpeed, g_enBulletSpeed, 1, 0, 0);
					enemy->setIA(new IA(g_enShootFreq));
					enemy->setWheelYaw(7);
					
					g_cars.push_back(enemy);
					// g_objects.push_back(new Circle(id, x, y, radius, 1, 0, 0));
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
						g_arena.push_back(new Circle(id, x, y, 0, radius, r, g, b));
					else
						g_objects.push_back(new Circle(id, x, y, 0, radius, r, g, b));
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

	for(uint i = 0; i < g_cars.size(); ++i)
	{
		// Car *aux = dynamic_cast<Car*>(g_cars[i]);
		// Circle *aux = dynamic_cast<Circle*>(g_objects[i]);

		if(g_cars[i] != NULL)
		{
			dx = g_cars[i]->getX() - nextPos.x;
			dy = g_cars[i]->getY() - nextPos.y;

			dist = sqrt(dx * dx + dy * dy);
			if(dist <= g_cars[i]->getHitboxRadius() + g_player->getHitboxRadius())
				return true;
		}
	}

	return false;
}

bool checkBotCollision(uint index, vec3 nextPos)
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
	if((dist <= minRadius + g_cars[index]->getHitboxRadius()) || (dist >= maxRadius - g_cars[index]->getHitboxRadius()))
		return true;


	dx = g_player->getX() - nextPos.x;
	dy = g_player->getY() - nextPos.y;

	dist = sqrt(dx * dx + dy * dy);
	if(dist <= g_cars[index]->getHitboxRadius() + g_player->getHitboxRadius())
			return true;

	for(uint i = 0; i < g_cars.size(); ++i)
	{
		// Car *aux = dynamic_cast<Car*>(g_cars[i]);
		// Circle *aux = dynamic_cast<Circle*>(g_objects[i]);
		if(i == index)
			continue;

		if(g_cars[i] != NULL)
		{
			dx = g_cars[i]->getX() - nextPos.x;
			dy = g_cars[i]->getY() - nextPos.y;

			dist = sqrt(dx * dx + dy * dy);
			if(dist <= g_cars[i]->getHitboxRadius() + g_cars[index]->getHitboxRadius())
				return true;
		}
	}

	return false;
}

void checkLaps()
{
	bool lastStatus = inStart;
	inStart = (g_player->getX() > g_start->getX() - g_start->getWidth()/2)
		&& (g_player->getX() < g_start->getX() + g_start->getWidth()/2)
		&& (g_player->getY() >= g_start->getY() - g_start->getHeight()/2)
		&& (g_player->getY() <= g_start->getY() + g_start->getHeight()/2);

	//If he was inside and got out on the bottom, minus a lap
	if(lastStatus && !inStart)
	{
		double dy = g_start->getY() - g_player->getY();
		//cout << "From inside dy: " << dy << " distance from start center: " << g_player->getHitboxRadius() + g_start->getHeight()/2 << endl;
		if(dy > 0 && dy >= g_start->getHeight()/2)
		{
			//cout << "got out thru bottom" << endl; 
			laps--;
			//cout << "Laps = " << laps << endl;
		}
	}

	if(!lastStatus && inStart)
	{
		//if was outside and got in tru bottom, plus a lap
		double dy = g_start->getY() - g_player->getY();
		//cout << "From outside dy: " << dy << endl;
		if(dy > 0 && dy <= g_player->getHitboxRadius() + g_start->getHeight()/2)
		{
			//cout << "got in thru bottom" << endl; 
			laps++;
			//cout << "Laps = " << laps << endl;
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
			vector<Car*>::iterator cIt;
			for(cIt = g_cars.begin(); cIt != g_cars.end(); ++cIt)
			{
				double dx = (*cIt)->getX() - (*bIt)->getX();
				double dy = (*cIt)->getY() - (*bIt)->getY();

				double dist = sqrt(dx * dx + dy * dy);

				if(dist <= (*cIt)->getHitboxRadius() + (*bIt)->getHitboxRadius())
				{
					Bullet *baux = *bIt;
					g_bullets.erase(bIt);
					delete baux;

					Car *caux = *cIt;
					g_cars.erase(cIt);
					delete caux;

					bErased = true;
					break;
				}
			}
		}else{
			double dx = g_player->getX() - (*bIt)->getX();
			double dy = g_player->getY() - (*bIt)->getY();

			double dist = sqrt(dx * dx + dy * dy);

			if(dist <= g_player->getHitboxRadius() + (*bIt)->getHitboxRadius())
			{
				g_winCond = -1;
			}
		}

		if(bErased)
			continue;
		
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

void renderBitmapString(int x, int y, void *font, char *string)
{
	glColor3f(0, 0, 0);
	char *c;
	glRasterPos2f(x, y);
	for(c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
}

GLuint loadTextureRAW(const char* filename)
{
    GLuint texture;
    
    Image* image = loadBMP(filename);

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                             0,                            //0 for now
                             GL_RGB,                       //Format OpenGL uses for image
                             image->width, image->height,  //Width and height
                             0,                            //The border of the image
                             GL_RGB, //GL_RGB, because pixels are stored in RGB format
                             GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                                               //as unsigned numbers
                             image->pixels);               //The actual pixel data
    delete image;

    return texture;
}
