#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <iostream>
#include <sstream>
#include <cstring>
#include <random>

#include <GL/glut.h>
#include "vec3.h"

#include <math.h>
#define _USE_MATH_DEFINES

using namespace std;

typedef unsigned int uint;

class Object
{
public:
	Object(const char* nid, double R = 0, double G = 0, double B = 0) : r(R), g(G), b(B) {id[0] = '\0'; strcat(id, nid);};
	virtual ~Object() {};
	virtual void draw() = 0;
	virtual int getX() = 0;
	virtual int getY() = 0;

	void setColor(double r, double g, double b) {this->r = r; this->g = g; this->b = b;};
	const char* getID() {return this->id;};
	
	double getR() {return r;};
	double getG() {return g;};
	double getB() {return b;};
protected:
	char id[256];
	double r, g, b;
};

class Rectangle: public Object
{
	int x, y;
	int width, height;
	double strokeW;
	double strR, strG, strB;
public:

	Rectangle(const char* id, int x, int y, int w, int h, double R, double G, double B, double sw, double sr, double sg, double sb):
		Object(id, R, G, B), x(x), y(y), width(w), height(h), strokeW(sw), strR(sr), strG(sg), strB(sb) {};

	~Rectangle(){};

	void draw();

	int getX() {return x;};
	int getY() {return y;};
	int getWidth() {return width;};
	int getHeight() {return height;};
};

class Circle: public Object
{
	int x, y, radius;
public:

	Circle(const char *id, int x, int y, int rad, double r, double g, double b):
		Object(id, r, g, b), x(x), y(y), radius(rad) {};

	~Circle(){};

	void draw();

	int getX() {return x;};
	int getY() {return y;};
	int getRad() {return radius;};
};

class Bullet: public Object
{
	int x, y, hitboxRad;
	double dir, speed;

	char ownerID[256];

public:

	Bullet(const char *id, int x, int y, int rad, double d, double spd, const char* ownID):
		Object(id), x(x), y(y), hitboxRad(rad), dir(d), speed(spd) {ownerID[0] = '\0'; strcat(ownerID, ownID);};

	~Bullet(){};

	int getX(){ return x; };
	int getY(){ return y; };
	int getHitboxRad(){ return hitboxRad; };
	char* getOwner(){ return ownerID; };
	
	void draw();
	void updatePosition(double timeDiff);
};

class Chopper: public Object
{
	int x, y, hitboxRad, gunx, guny;
	double yaw, gunAngle, hlxSpeed, hlxAngle, chpSpeed, bltSpeed;

	vec3 gunVector;

	double mouseSens;
	int mouseLastX;

	short int state, drawHitbox;
	
	int	bodyWidth,
		bodyHeight,
		gunWidth,
		gunHeight,
		tailWidth,
		tailHeight,
		rotorWidth,
		rotorHeight,
		helixWidth,
		helixHeigth;

public:
	Chopper(const char *id, int x, int y, int rad, int st, double spdC = 1, double spdB = 1.5,
			double yaw = 0, double hSpeed = 10, double r = 0, double g = 1, double b = 0);

	~Chopper(){};

	void draw();

	int getX(){ return x; };
	int getY(){ return y; };
	int getHitboxRad(){ return hitboxRad; };
	void setX(int x){ this->x = x; };
	void setY(int y){ this->y = y; };

	double getTurnSpeed(){ return chpSpeed; };
	void drawHbx(){ drawHitbox = !drawHitbox; };
	void setLimits(int x, int y);

	void changeState();
	void incRot();
	void decRot();
	
	void moveFoward();
	void moveBackward();

	vec3 getNextPosition(int direc);

	void pivot(double dyaw);
	void moveGun(int x, int y);
	Bullet* shoot();
};

#endif