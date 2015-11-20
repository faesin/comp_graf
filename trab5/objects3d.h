#ifndef _OBJECTS3D_H_
#define _OBJECTS3D_H_

#include <iostream>
#include <sstream>
#include <cstring>
#include <random>
#include <sys/time.h>
#include <chrono>

#include <GL/glut.h>
#include "vec3.h"

#include <math.h>
#include "objects.h"
#define _USE_MATH_DEFINES

using namespace std;

class Object3d
{
public:
	Object3d(const char* nid, double R = 0, double G = 0, double B = 0) : r(R), g(G), b(B) {id[0] = '\0'; strcat(id, nid);};
	virtual ~Object() {};
	virtual void draw() = 0;
	virtual GLfloat getX() const = 0;
	virtual GLfloat getY() const = 0;
	virtual GLfloat getZ() const = 0;

	void setColor(double r, double g, double b) { this->r = r; this->g = g; this->b = b; };
	const char* getID() const { return this->id; };
	
	double getR() const { return r; };
	double getG() const { return g; };
	double getB() const { return b; };
protected:
	char id[256];
	double r, g, b;
};

class Block: public Object3d
{
	GLfloat x, y, z;
	int widthX, widthY, height;
	double strokeW;
	double strR, strG, strB;
public:

	Block(const char* id, GLfloat x, GLfloat y, GLfloat z, int wx, int wy, int h, double R, double G, double B, double sw, double sr, double sg, double sb):
		Object(id, R, G, B), x(x), y(y), z(z), widthX(wx), widthY(wy), height(h), strokeW(sw), strR(sr), strG(sg), strB(sb) {};

	~Rectangle(){};

	void draw();

	GLfloat getX() const {return x;};
	GLfloat getY() const {return y;};
	GLfloat getZ() const {return z;};
	int getWidth() const {return width;};
	int getHeight() {return height;};
};

class Sphere: public Object3d
{
	GLfloat x, y, z, radius;
public:

	Circle(const char *id, GLfloat x, GLfloat y, GLfloat z, int rad, double r, double g, double b):
		Object(id, r, g, b), x(x), y(y), radius(rad) {};

	~Circle(){};

	void draw();

	GLfloat getX() const {return x;};
	GLfloat getY() const {return y;};
	int getRad() const {return radius;};
};


#endif