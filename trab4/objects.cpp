#include "objects.h"


/* ========================= RECTANGLE ========================= */

void Rectangle::draw()
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
}

/* ========================= CIRCLE ========================= */

void Circle::draw()
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
}

/* ========================= BULLET ========================= */

void Bullet::updatePosition(double timeDiff)
{
	x += round(sin(dir * M_PI / 180.0)*speed);
	y -= round(cos(dir * M_PI / 180.0)*speed);
}

/* ========================= CHOPPER ========================= */

Chopper::Chopper(const char *id, int x, int y, int rad, int st, double spdC, double spdB,
		double yaw, double hSpeed, double r, double g, double b) :
	Object(id, r, g, b), x(x), y(y), hitboxRad(rad), yaw(yaw), hlxSpeed(hSpeed), chpSpeed(spdC), bltSpeed(spdB)
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

	drawHitbox = 0;
	hlxAngle = 0;
	gunAngle = 0;

	state = 0;

	mouseSens = 0.5;
	mouseLastX = x;

	if(st)
		this->changeState();
}

void Chopper::draw()
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

		Rectangle("chpBody", 0, 0, bodyWidth, bodyHeight, 
				this->getR(), this->getG(), this->getB(), 1, 0, 0, 0).draw();

		glPushMatrix();
			glTranslated(0, (bodyHeight + tailHeight)/2, 0);
			Rectangle("chpTail", 0, 0, tailWidth, tailHeight, 
					this->getR(), this->getG(), this->getB(), 1, 0, 0, 0).draw();

			glPushMatrix();
				glTranslated((tailWidth+rotorWidth)/2, tailHeight/2, 0);
				Rectangle("chpRotor1", 0, 0, rotorWidth, rotorHeight, 
						this->getR(), this->getG(), this->getB(), 1, 0, 0, 0).draw();
			glPopMatrix();

			glPushMatrix();
				glTranslated(-(tailWidth+rotorWidth)/2, tailHeight/2, 0);
				Rectangle("chpRotor2", 0, 0, rotorWidth, rotorHeight, 
						this->getR(), this->getG(), this->getB(), 1, 0, 0, 0).draw();
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
}

void Chopper::changeState()
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
}

void Chopper::incRot()
{
	if(hlxSpeed >= 45)
		hlxSpeed = 45.0;
	else
		hlxSpeed += 2;
}

void Chopper::decRot()
{
	if(hlxSpeed < -45)
		hlxSpeed = -45;
	else
		hlxSpeed -= 2;
}

void Chopper::moveFoward()
{
	if(state)
	{
		int dx = round(sin(yaw * M_PI / 180.0)*chpSpeed);
		int dy = round(cos(yaw * M_PI / 180.0)*chpSpeed);

		x += dx; y -= dy;

		gunx = this->x + sin(yaw * M_PI / 180.0)*((bodyWidth/2)*1.5);
		guny = this->y - cos(yaw * M_PI / 180.0)*((bodyWidth/2)*1.5);
	}
}

void Chopper::moveBackward()
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

void Chopper::pivot(double dyaw)
{
	if(state)
		yaw += dyaw;			

	if(yaw >= 360)
		yaw -= 360;

	if(yaw < 0)
		yaw += 360;
}

void Chopper::moveGun(int x, int y)
{
	if(state)
	{
		int mouseMovedX = x - mouseLastX;

		gunAngle += mouseMovedX*mouseSens;
		if(gunAngle > 45)
			gunAngle = 45;

		if(gunAngle < -45)
			gunAngle = -45;

		mouseLastX = x;
	}
}

Bullet* Chopper::shoot()
{
	return new Bullet("pewpew", gunx, guny, gunWidth/2, yaw, bltSpeed);
}