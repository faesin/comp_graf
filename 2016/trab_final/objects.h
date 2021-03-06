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
#include "botia.h"
#include "objloader.h"

#include <math.h>
#define _USE_MATH_DEFINES

using namespace std;

typedef unsigned int uint;
typedef enum dir {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
} dir_t;

class Object
{
public:
	Object(const char* nid, double R = 0, double G = 0, double B = 0, double S = 128) : r(R), g(G), b(B), shininess(S) {id[0] = '\0'; strcat(id, nid);};
	virtual ~Object() {};

	virtual void draw() = 0;
	virtual double getX() const = 0;
	virtual double getY() const = 0;
	virtual double getZ() const = 0;

	void setColor(double r, double g, double b) { this->r = r; this->g = g; this->b = b; };
	void setTexture(GLuint tex) {texture = tex;}
	const char* getID() const { return this->id; };
	
	double getR() const {return r;};
	double getG() const {return g;};
	double getB() const {return b;};
	double getShininess() const {return shininess;};
	GLuint getTexture() const {return texture;};

protected:
	char id[256];
	double r, g, b, shininess;
	GLuint texture;
};



class Line: public Object
{
	double x, y, z, width;
public:
	Line(const char* id, double x, double y, double z, double w, double R, double G, double B, double S = 0):
		Object(id, R, G, B, S), x(x), y(y), z(z), width(w) {};
	~Line(){};

	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getZ() const {return z;};
	double getWidth() const {return width;};
};



class Rectangle: public Object
{
	double x, y, z, width, height;
public:
	Rectangle(const char* id, double x, double y, double z, double w, double h, double R, double G, double B, double S = 100):
		Object(id, R, G, B, S), x(x), y(y), z(z), width(w), height(h) {};
	~Rectangle(){};

	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getZ() const {return z;};
	double getWidth() const {return width;};
	double getHeight() const {return height;};
};



class Circle: public Object
{
	double x, y, z, radius;
public:

	Circle(const char *id, double x, double y, double z, double rad, double R, double G, double B, double S = 100):
		Object(id, R, G, B, S), x(x), y(y), z(z), radius(rad) {};
	~Circle(){};

	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getZ() const {return z;};
	double getRadius() const {return radius;};

};


class Cilinder: public Object
{
	double x, y, z, radius, height;

public:
	Cilinder(const char* id, double x, double y, double z, double r, double h,
		double R, double G, double B, double S = 128):
		Object(id, R, G, B, S), x(x), y(y), z(z), radius(r), height(h) {};
		
	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getZ() const {return z;};
	double getHeight() const {return height;};
	double getRadius() const {return radius;};
};


//Its a parallelepiped, but the name is too big
class Cube: public Object
{
	double x, y, z, width, height, depth; 

public:
	Cube(const char* id, double x, double y, double z, double w, double h, double d,
		double R, double G, double B, double S = 128):
		Object(id, R, G, B, S), x(x), y(y), z(z), width(w), height(h), depth(d) {};
	~Cube(){};

	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getZ() const {return z;};
	double getWidth() const {return width;};
	double getHeight() const {return height;};
	double getDepth() const {return depth;};
};



class Bullet: public Object
{
	double x, y, z, hitboxRad, yaw, pitch, speed;
	char ownerID[256];

public:

	Bullet(const char *id, double x, double y, double z, double rad, double d, double p, double spd, const char* ownID):
		Object(id), x(x), y(y), z(z), hitboxRad(rad), yaw(d), pitch(p), speed(spd) {ownerID[0] = '\0'; strcat(ownerID, ownID);};
	~Bullet(){};

	double getX() const {return x;};
	double getY() const {return y;};
	double getZ() const {return z;};
	int getHitboxRadius() const {return hitboxRad;};
	char* getOwner() {return ownerID;};
	
	void draw();
	void updatePosition(double timeDiff);
};



class Car: public Object
{
	double x, y, z, hitboxRad, yaw, carSpeed,
			bodyHeight, bodyWidth, bodyDepth,
			suspHeight, suspWidth, suspDepth,
			wheelRadius, wheelWidth, wheelYaw,
			wheelTrackHeight, wheelTrackWidth, trackDelta,
			cannonHeight, cannonWidth, cannonDepth, cannonYaw, cannonPitch, cX, cY, cZ,
			bulletSpeed;

	double rearAxesX, rearAxesY, rotCX, rotCY, rotRadius;

	double mouseSens, mouseLastX, mouseLastY;

	IA *intel;

	Mesh* carMesh;

	GLuint bodyTex;
	GLuint cockpitTex;
	GLuint tireTex;
	GLuint capTex;

public:
	Car(const char *id, double x, double y, double z, double rad, double yaw = 0, double cSpd = 1, double bSpd = 0.5,
		 double r = 0, double g = 1, double b = 0);
	~Car(){ delete intel; };

	void setMesh(Mesh* m){ carMesh = m; };

	void draw();

	double getX() const {return x;};
	double getY() const {return y;};
	double getZ() const {return z;};
	double getYaw() const {return yaw;};
	double getHitboxRadius() const {return hitboxRad;};

	double getCX() const {return cX;};
	double getCY() const {return cY;};
	double getCZ() const {return cZ;};
	double getCannonYaw() const {return cannonYaw;};
	double getCannonPitch() const {return cannonPitch;};
	
	void setX(int x) {this->x = x;};
	void setY(int y) {this->y = y;};
	
	double getWheelYaw() { return wheelYaw; };
	void setWheelYaw(double yaw){ this->wheelYaw = yaw; };
	
	void setIA(IA* ia){ this->intel = ia; };
	void startIA(){ this->intel->switchInt(); };
	int getNextInstr(){ return this->intel->getInstr(); };

	double getTurnSpeed() const { return carSpeed; };
	double getSpeed() const { return carSpeed; };

	//Preliminary movestuff; might remove
	void moveUp(){ this->y += this->carSpeed; };
	void moveDown(){ this->y -= this->carSpeed; };
	void moveSides(dir_t direc){ if(direc == LEFT) this->x -= this->carSpeed; else this->x += this->carSpeed; };

	void moveFoward(GLdouble);
	void moveBackward(GLdouble);
	void moveWheels(double dyaw, GLdouble);
	void moveCannon(int, int);

	Bullet* shoot();

	vec3 getNextPosition(int direc, GLdouble);

	void think(double timeDiff) {this->intel->doStep(timeDiff);};

	void setBodyTex(GLuint tex) {bodyTex = tex;};
	void setCockpitTex(GLuint tex) {cockpitTex = tex;};
	void setTireTex(GLuint tex) {tireTex = tex;};
	void setCapTex(GLuint tex) {capTex = tex;};
		
};

// ostream& operator<<(std::ostream& out, const Car& c);
//long long getCurrentTimeMS();

#endif
