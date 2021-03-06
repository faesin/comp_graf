#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <iostream>
#include <sstream>
#include <cstring>

//#include <random>
//#include <sys/time.h>
//#include <chrono>

#include <GL/glut.h>
#include "vec3.h"

#include <math.h>
#define _USE_MATH_DEFINES

using namespace std;

typedef unsigned int uint;
typedef enum {
	STAY = 0,
	FOWARD = 1,
	BACKWARD = 2,
	TURN_R = 3,
	TURN_L = 4,
	TURNFWRD_R = 5,
	TURNFWRD_L = 6,
	TURNBACK_R = 7,
	TURNBACK_L = 8,
	SEEK = 9
} instr_t;

typedef enum dir {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
} dir_t;

class Object
{
public:
	Object(const char* nid, double R = 0, double G = 0, double B = 0) : r(R), g(G), b(B) {id[0] = '\0'; strcat(id, nid);};
	virtual ~Object() {};
	virtual void draw() = 0;
	virtual double getX() const = 0;
	virtual double getY() const = 0;

	void setColor(double r, double g, double b) { this->r = r; this->g = g; this->b = b; };
	const char* getID() const { return this->id; };
	
	double getR() const { return r; };
	double getG() const { return g; };
	double getB() const { return b; };
protected:
	char id[256];
	double r, g, b;
};

class Rectangle: public Object
{
	double x, y, width, height;
public:
	Rectangle(const char* id, int x, int y, int w, int h, double R, double G, double B):
		Object(id, R, G, B), x(x), y(y), width(w), height(h) {};

	~Rectangle(){};

	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getWidth() const {return width;};
	double getHeight() const {return height;};
};

class Circle: public Object
{
	double x, y, radius;
public:

	Circle(const char *id, int x, int y, int rad, double r, double g, double b):
		Object(id, r, g, b), x(x), y(y), radius(rad) {};

	~Circle(){};

	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getRadius() const {return radius;};
};

class Car: public Object
{
	double x, y, hitboxRad, yaw, carSpeed;

public:
	Car(const char *id, int x, int y, int rad, double yaw = 0, double spdC = 1,
		 double r = 0, double g = 1, double b = 0);

	~Car(){};

	void draw();

	double getX() const { return x; };
	double getY() const { return y; };
	double getYaw() const { return yaw; };
	double getHitboxRadius() const { return hitboxRad; };

	void setX(int x){ this->x = x; };
	void setY(int y){ this->y = y; };

	double getTurnSpeed() const { return carSpeed; };
	double getSpeed() const { return carSpeed; };

	//Preliminary snattacat; might remove
	void moveUp(){ this->y += this->carSpeed; };
	void moveDown(){ this->y -= this->carSpeed; };
	void moveSides(dir_t direc){ if(direc == LEFT) this->x -= this->carSpeed; else this->x += this->carSpeed; };

	void moveFoward();
	void moveBackward();
	void pivot(double dyaw);

	vec3 getNextPosition(int direc);
};

// ostream& operator<<(std::ostream& out, const Car& c);
long long getCurrentTimeMS();

#endif