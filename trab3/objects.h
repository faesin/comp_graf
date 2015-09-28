#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "vec3.h"

using namespace std;

typedef unsigned int uint;

class Object
{
public:
	Object(const char* nid, double R = 0, double G = 0, double B = 0) : r(R), g(G), b(B) {id[0] = '\0'; strcat(id, nid);};
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

	void draw()
	{
		glPushMatrix();
			glTranslated((double)this->x, (double)this->y, 0.0);
			glBegin(GL_POLYGON);
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(this->width, 0.0, 0.0);
				glVertex3d(this->width, this->height, 0.0);
				glVertex3d(0.0, this->height, 0.0);
			glEnd();

			glColor3d(this->strR, this->strG, this->strB);
			glLineWidth((float)this->strokeW);
			glBegin(GL_LINE_LOOP);
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(this->width, 0.0, 0.0);
				glVertex3d(this->width, this->height, 0.0);
				glVertex3d(0.0, this->height, 0.0);
			glEnd();

			glPointSize((float)this->strokeW);
			glBegin(GL_POINTS);
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(this->width, 0.0, 0.0);
				glVertex3d(this->width, this->height, 0.0);
				glVertex3d(0.0, this->height, 0.0);
			glEnd();
		glPopMatrix();
	};

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

	void draw()
	{
		glPushMatrix();
			glTranslated((double)this->x, (double)this->y, 0);

			static const double circle_points = 100;
			static const float angle = 2.0f * 3.1416f / circle_points;
			glLineWidth(1);
			glPointSize(1);
			glBegin(GL_POLYGON);
				for(double angle1 = 0.0, i = 0; i < circle_points; ++i)
				{
					glVertex3d(this->radius*cos(angle1), this->radius * sin(angle1), 0);
					angle1 += angle;
				}
			glEnd();

		glPopMatrix();
	};

	int getX() {return x;};
	int getY() {return y;};
	int getRad() {return radius;};
};

class Chopper: public Object
{
	int x, y;
	double yaw, spdHelix, spdChopper, spdBullet;
	
	static const int	bodyWidth = 20,
						bodyHeight = 35,
						tailWidth = 5,
						tailHeight = 22,
						rotorWidth = 3,
						rotorHeight = 5;

public:

	Chopper(const char *id, int x, int y, double spdC, double spdB, double yaw = 0):
	Object(id), x(x), y(y), yaw(yaw), spdChopper(spdC), spdBullet(spdB){};

	void draw()
	{
		Rectangle body("", x, y, bodyWidth, bodyHeight, 0, 1, 0, 2, 0, 0, 0);
		body.draw();

	};

	int getX(){return x;};
	int getY(){return y;};
};

#endif