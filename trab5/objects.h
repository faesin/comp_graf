#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <iostream>
#include <sstream>
#include <cstring>
#include <random>
#include <sys/time.h>
#include <chrono>

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
	SEEK
} instr_t;

class Object
{
public:
	Object(const char* nid, double R = 0, double G = 0, double B = 0) : r(R), g(G), b(B) {id[0] = '\0'; strcat(id, nid);};
	virtual ~Object() {};
	virtual void draw() = 0;
	virtual GLfloat getX() const = 0;
	virtual GLfloat getY() const = 0;

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
	GLfloat x, y;
	int width, height;
	double strokeW;
	double strR, strG, strB;
public:

	Rectangle(const char* id, GLfloat x, GLfloat y, int w, int h, double R, double G, double B, double sw, double sr, double sg, double sb):
		Object(id, R, G, B), x(x), y(y), width(w), height(h), strokeW(sw), strR(sr), strG(sg), strB(sb) {};

	~Rectangle(){};

	void draw();

	GLfloat getX() const {return x;};
	GLfloat getY() const {return y;};
	int getWidth() const {return width;};
	int getHeight() {return height;};
};

class Circle: public Object
{
	GLfloat x, y, radius;
public:

	Circle(const char *id, GLfloat x, GLfloat y, int rad, double r, double g, double b):
		Object(id, r, g, b), x(x), y(y), radius(rad) {};

	~Circle(){};

	void draw();

	GLfloat getX() const {return x;};
	GLfloat getY() const {return y;};
	int getRad() const {return radius;};
};

class Bullet: public Object
{
	GLfloat x, y;
	int hitboxRad;
	double dir, speed;

	char ownerID[256];

public:

	Bullet(const char *id, GLfloat x, GLfloat y, int rad, double d, double spd, const char* ownID):
		Object(id), x(x), y(y), hitboxRad(rad), dir(d), speed(spd) {ownerID[0] = '\0'; strcat(ownerID, ownID);};

	~Bullet(){};

	GLfloat getX() const { return x; };
	GLfloat getY() const { return y; };
	int getHitboxRad() const { return hitboxRad; };
	char* getOwner() { return ownerID; };
	
	void draw();
	void updatePosition(double timeDiff);
};

class IA
{
	instr_t current_instr;

	bool enableInt = false;
	bool didSeek = false;

	long long lastTime;
	double fillShoot, fillChange;
	double shootsPerMilli, changeMove;

	static random_device rd;
	static mt19937 eng;

public:

	IA(double seekTimer);

	~IA(){};

	void setInstr(int instr){ current_instr = static_cast<instr_t>(instr); };
	void setSeekDone();
	int getInstr() const { return current_instr; };

	void switchInt(){ enableInt = !enableInt; };
	void doStep();

};

class Chopper: public Object
{
	GLfloat x, y, gunx, guny;
	int hitboxRad;
	double yaw, gunAngle, hlxSpeed, hlxAngle, chpSpeed, bltSpeed;

	IA *intel;

	double fuelTime, fuelTimeMax;

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
	Chopper(const char *id, GLfloat x, GLfloat y, int rad, int st, double spdC = 1, double spdB = 1.5,
			double yaw = 0, double hSpeed = 10, double fuel = 10, double r = 0, double g = 1, double b = 0);

	~Chopper(){ delete intel; };

	void draw();

	GLfloat getX() const { return x; };
	GLfloat getY() const { return y; };
	double getYaw() const { return yaw; };
	double getFuel() const { return fuelTime; };
	int getHitboxRad() const { return hitboxRad; };
	int getCurrInstr() const { return intel->getInstr(); };

	void setX(GLfloat x){ this->x = x; };
	void setY(GLfloat y){ this->y = y; };
	void setIntel(IA *intel){ this->intel = intel; };
	void refuel(double delta);
	void setSeekDone(){ this->intel->setSeekDone(); };

	void useFuel(double timeDiff);

	double getTurnSpeed() const { return chpSpeed; };
	void drawHbx(){ drawHitbox = !drawHitbox; };
	void setLimits(int x, int y);

	void doStep(){ this->intel->doStep(); };
	void changeState();
	short int getState(){ return this->state; };
	void incRot();
	void decRot();
	
	void moveFoward(GLdouble timeDiff);
	void moveBackward(GLdouble timeDiff);
	void moveByIA(GLdouble timeDiff);

	vec3 getNextPosition(int direc);

	void pivot(double dyaw, GLdouble timeDiff);
	void moveGun(int x, int y);
	Bullet* shoot();
};

ostream& operator<<(std::ostream& out, const Chopper& c);
long long getCurrentTimeMS();

#endif