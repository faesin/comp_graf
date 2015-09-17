#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "vec3.h"

using namespace std;

typedef unsigned int uint;

class Object
{
public:
	Object(const char* nid, double R, double G, double B) : r(R), g(G), b(B) {id[0] = '\0'; strcat(id, nid);};
	virtual void draw() = 0;
	virtual void print() = 0;
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
public:
	int x, y;
	int width, height;
	double strokeW;
	double strR, strG, strB;

	Rectangle(const char* id, int x, int y, int w, int h, double R, double G, double B, double sw, double sr, double sg, double sb):
	Object(id, R, G, B), x(x), y(y), width(w), height(h), strokeW(sw), strR(sr), strG(sg), strB(sb) {};

	void draw(){};
	void print()
	{ cout << "id: " << id << "\n\tx: " << x << "\n\ty: " << y << "\n\twidth: " << width << "\n\theight: " << height
			<< "\n\tstroke_width: " << strokeW << "\n\t\tstrokeR: " << strR << "\n\t\tstrokeG: " << strG
			<< "\n\t\tstrokeB: " << strB << "\n\tR: " << r << "\n\tG: " << g << "\n\tB: " << b << endl;};

	int getX() {return x;};
	int getY() {return y;};
};

class Circle: public Object
{
public:
	int x, y, radius;

	Circle(const char *id, int x, int y, int rad, double r, double g, double b):
	Object(id, r, g, b), x(x), y(y), radius(rad) {};

	void draw(){};
	void print()
	{ cout << "id: " << id << "\n\tx: " << x << "\n\ty: " << y << "\n\tradius: " << radius 
			<< "\n\tR: " << r << "\n\tG: " << g << "\n\tB: " << b << endl;};

	int getX() {return x;};
	int getY() {return y;};
};

#endif