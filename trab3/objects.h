#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "vec3.h"

#include <math.h>
#define _USE_MATH_DEFINES

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
		glColor3d(this->getR(), this->getG(), this->getB());

		glPushMatrix();
			glTranslated(x, y, 0);

			glBegin(GL_POLYGON);
				glVertex3d(-width/2, height/2, 0.0);
				glVertex3d(width/2, height/2, 0.0);
				glVertex3d(width/2, -height/2, 0.0);
				glVertex3d(-width/2, -height/2, 0.0);
			glEnd();

			glColor3d(this->strR, this->strG, this->strB);
			glLineWidth(this->strokeW);
			glBegin(GL_LINE_LOOP);
				glVertex3d(-width/2, height/2, 0.0);
				glVertex3d(width/2, height/2, 0.0);
				glVertex3d(width/2, -height/2, 0.0);
				glVertex3d(-width/2, -height/2, 0.0);
			glEnd();

			glPointSize(this->strokeW);
			glBegin(GL_POINTS);
				glVertex3d(-width/2, height/2, 0.0);
				glVertex3d(width/2, height/2, 0.0);
				glVertex3d(width/2, -height/2, 0.0);
				glVertex3d(-width/2, -height/2, 0.0);
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
		glColor3d(this->getR(), this->getG(), this->getB());

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

class Triangle: public Object
{
	int xt, yt, xb, yb, width;

public:
	Triangle(const char *id, int x, int y, int x2, int y2, int w, double r, double g, double b):
	Object(id, r, g, b), xt(x), yt(y), xb(x2), yb(y2), width(w){};

	void draw()
	{
		// glColor3d(this->getR(), this->getG(), this->getB());

		// glPushMatrix();
		// 	glTranslated((double)this->xt, (double)this->yt, 0);

		// glPopMatrix();
	}

	int getXT() {return xt;};
	int getYT() {return yt;};

	int getXB() {return xb;};
	int getYB() {return yb;};

	int getWidth() {return width;};
};

class Chopper: public Object
{
	int x, y, hitboxRad, gunx, guny;
	double yaw, gunAngle, hlxSpeed, hlxAngle, chpSpeed, bltSpeed;

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

	Chopper(const char *id, int x, int y, int rad, double spdC = 1, double spdB = 1.5, double yaw = 0, double hSpeed = 10):
	Object(id), x(x), y(y), hitboxRad(rad), yaw(yaw), hlxSpeed(hSpeed), chpSpeed(spdC), bltSpeed(spdB)
	{
		bodyWidth = hitboxRad/1.25;
		bodyHeight = hitboxRad/1.25;
		gunWidth = hitboxRad/6;
		gunHeight = hitboxRad/2.5;
		tailWidth = hitboxRad/5.5;
		tailHeight = hitboxRad/1.65;
		rotorWidth = hitboxRad/6;
		rotorHeight = hitboxRad/3;
		helixWidth = hitboxRad/5.5;
		helixHeigth = hitboxRad*2;

		gunx = x;
		guny = y - bodyWidth/2;

		state = 0;
		drawHitbox = 0;
		hlxAngle = 0;
		gunAngle = 0;
	};

	void draw()
	{
		if(drawHitbox)
		{
			glPushMatrix();
				glTranslated(x, y, 0);
				Circle("hitbox", 0, 0, hitboxRad, 0.5, 0.5, 0.5).draw();
			glPopMatrix();
		}
			
		glPushMatrix();
			glTranslated(x, y, 0);
			glRotated(yaw, 0, 0, 1);

			Rectangle("chpBody", 0, 0, bodyWidth, bodyHeight, 0, 1, 0, 1, 0, 0, 0).draw();

			glPushMatrix();
				glTranslated(0, (bodyHeight + tailHeight)/2, 0);
				Rectangle("chpTail", 0, 0, tailWidth, tailHeight, 0, 1, 0, 1, 0, 0, 0).draw();

				glPushMatrix();
					glTranslated((tailWidth+rotorWidth)/2, tailHeight/2, 0);
					Rectangle("chpRotor1", 0, 0, rotorWidth, rotorHeight, 0, 1, 0, 1, 0, 0, 0).draw();
				glPopMatrix();

				glPushMatrix();
					glTranslated(-(tailWidth+rotorWidth)/2, tailHeight/2, 0);
					Rectangle("chpRotor2", 0, 0, rotorWidth, rotorHeight, 0, 1, 0, 1, 0, 0, 0).draw();
				glPopMatrix();
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, -bodyHeight/2, 0);
				glRotated(gunAngle, 0, 0, 1);
				glTranslated(0, -gunHeight/2, 0);

				Rectangle("gun", 0, 0, gunWidth, gunHeight, 1, 1, 0, 1, 0, 0, 0).draw();
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			glTranslated(x, y, 0);

			if(state)
			{
				if(hlxAngle > 90)
					hlxAngle -= 90;
				else
					if(hlxAngle < -90)
						hlxAngle += 90;

				hlxAngle += hlxSpeed;
			}
			
			glRotated(hlxAngle, 0, 0, 1);

			Rectangle("helix1", 0, 0, helixWidth, helixHeigth, 0, 0, 1, 1, 0, 0, 0).draw();
			Rectangle("helix2", 0, 0, helixHeigth, helixWidth, 0, 0, 1, 1, 0, 0, 0).draw();

			Circle("top", 0, 0, helixWidth/2, 0.5, 0.5, 0.5).draw();
		glPopMatrix();

	};

	int getX(){return x;};
	int getY(){return y;};
	double getTurnSpeed(){return chpSpeed;};

	void changeState()
	{
		state = !state;

		if(state){
			hitboxRad *= 1.5;
			bodyWidth *= 1.5;
			bodyHeight *= 1.5;
			gunWidth *= 1.5;
			gunHeight *= 1.5;
			tailWidth *= 1.5;
			tailHeight *= 1.5;
			rotorWidth *= 1.5;
			rotorHeight *= 1.5;
			helixWidth *= 1.5;
			helixHeigth *= 1.5;
			
		}else{
			hitboxRad /= 1.5;
			bodyWidth /= 1.5;
			bodyHeight /= 1.5;
			gunWidth /= 1.5;
			gunHeight /= 1.5;
			tailWidth /= 1.5;
			tailHeight /= 1.5;
			rotorWidth /= 1.5;
			rotorHeight /= 1.5;
			helixWidth /= 1.5;
			helixHeigth /= 1.5;
		} 
	};
	void drawHbx(){drawHitbox = !drawHitbox;};

	void incRot()
	{
		if(hlxSpeed >= 45)
			hlxSpeed = 45.0;
		else
			hlxSpeed += 2;
	};
	
	void decRot()
	{
		if(hlxSpeed < -45)
			hlxSpeed = -45;
		else
			hlxSpeed -= 2;
	};

	void moveFoward()
	{
		if(state)
		{
			int dx = round(sin(yaw * M_PI / 180.0)*chpSpeed);
			int dy = round(cos(yaw * M_PI / 180.0)*chpSpeed);

			x += dx; y -= dy;

			gunx = this->x + sin(yaw * M_PI / 180.0)*((bodyWidth/2)*1.5);
			guny = this->y - cos(yaw * M_PI / 180.0)*((bodyWidth/2)*1.5);

			cout << "gun x = " << gunx << " gun y = " << guny << endl;
		}
	};

	void moveBackward()
	{
		if(state)
		{
			int dx = round(sin(yaw * M_PI / 180.0)*chpSpeed);
			int dy = round(cos(yaw * M_PI / 180.0)*chpSpeed);

			x -= dx; y += dy;

			gunx = this->x + sin(yaw * M_PI / 180.0)*((bodyWidth/2)*1.5);
			guny = this->y - cos(yaw * M_PI / 180.0)*((bodyWidth/2)*1.5);

			cout << "gun x = " << gunx << " gun y = " << guny << endl;
		}
	}

	void pivot(double dyaw)
	{
		if(state)
		{
			yaw += dyaw;			
		}

		if(yaw >= 360)
			yaw -= 360;

		if(yaw < 0)
			yaw += 360;
	}

	void moveGun(int x, int y)
	{
		double dangle;

		if(state)
		{
			if(yaw >= 0 && yaw < 90)
			{
				dangle = atan2(abs(y - guny), abs(x - gunx)) * 180.0/M_PI;
				if(abs(dangle) < 45)
					gunAngle = dangle;
			
				cout << gunAngle << " grads" << endl;

			}else if(yaw >= 90 && yaw < 180)
			{
				dangle = yaw - atan2(guny - y , x - gunx) * 180.0/M_PI;
				
				if(abs(dangle) < 45)
					gunAngle = dangle;
			
				cout << gunAngle << " grads" << endl;

			}else if(yaw >= 180 && yaw < 270)
			{
				dangle = yaw - atan2(guny - y , x - gunx) * 180.0/M_PI;
				
				if(abs(dangle) < 45)
					gunAngle = dangle;
			
				cout << gunAngle << " grads" << endl;
			}else if(yaw >= 270 && yaw < 360) 
			{
				dangle = yaw - atan2(guny - y , x - gunx) * 180.0/M_PI;
				
				if(abs(dangle) < 45)
					gunAngle = dangle;
			
				cout << gunAngle << " grads" << endl;
			}
		}

	};
};

#endif