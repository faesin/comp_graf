#include "objects.h"

/* =========================================================== */
/* ========================= GLOBALS ========================= */
/* =========================================================== */

// long long getCurrentTimeMS()
// {
// 	struct timeval tp;
// 	gettimeofday(&tp, NULL);
// 	long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; //get current timestamp in milliseconds
// 	// std::cout << mslong << std::endl;
// 	return mslong;
// }

// ostream& operator<<(std::ostream& out, const Car& c)
// {
// 	return out << c.getID() << " -> x:" << c.getX() << " y:" << c.getY() << " yaw:" << c.getYaw();
// }


/* ============================================================ */
/* =========================== LINE =========================== */
/* ============================================================ */

void Line::draw()
{
	glColor3d(this->getR(), this->getG(), this->getB());

	glPushMatrix();
		glTranslated(x, y, 0);
		
		glLineWidth(1);
		glBegin(GL_LINES);
			glVertex3d(-width/2.0, 0.0, 0.0);
			glVertex3d(width/2.0, 0.0, 0.0);
		glEnd();
	glPopMatrix();
}


/* ============================================================= */
/* ========================= RECTANGLE ========================= */
/* ============================================================= */

void Rectangle::draw()
{
	glColor3d(this->getR(), this->getG(), this->getB());

	glPushMatrix();
		glTranslated(x, y, 0);

		glBegin(GL_POLYGON);
			glVertex3d(-width/2.0, height/2.0, 0.0);
			glVertex3d(width/2.0, height/2.0, 0.0);
			glVertex3d(width/2.0, -height/2.0, 0.0);
			glVertex3d(-width/2.0, -height/2.0, 0.0);
		glEnd();

		// glColor3d(this->strR, this->strG, this->strB);
		// glLineWidth(this->strokeW);
		// glBegin(GL_LINE_LOOP);
		// 	glVertex3d(-width/2, height/2, 0.0);
		// 	glVertex3d(width/2, height/2, 0.0);
		// 	glVertex3d(width/2, -height/2, 0.0);
		// 	glVertex3d(-width/2, -height/2, 0.0);
		// glEnd();

		// glPointSize(this->strokeW);
		// glBegin(GL_POINTS);
		// 	glVertex3d(-width/2, height/2, 0.0);
		// 	glVertex3d(width/2, height/2, 0.0);
		// 	glVertex3d(width/2, -height/2, 0.0);
		// 	glVertex3d(-width/2, -height/2, 0.0);
		// glEnd();
	glPopMatrix();
}


/* ========================================================== */
/* ========================= CIRCLE ========================= */
/* ========================================================== */

void Circle::draw()
{
	/*GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat mat_color[] = {(float)this->getR(), (float)this->getG(), (float)this->getB(), 1.0};
	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[] = {this->getShininess()};*/

	//glColor3d(this->getR(), this->getG(), this->getB());
	glPushMatrix();
		/*
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);*/
		
		glColor3d(this->getR(), this->getG(), this->getB());

		glTranslated(x, y, 0);

		static const double circle_points = 100;
		static const float angle = 2.0f * 3.1416f / circle_points;
		glLineWidth(1);
		glPointSize(1);
		glBegin(GL_POLYGON);
			for(double angle1 = 0.0, i = 0; i < circle_points; ++i)
			{
				glVertex3d(radius*cos(angle1), radius * sin(angle1), 0.0);
				angle1 += angle;
			}
		glEnd();

	glPopMatrix();
}

/* ========================================================== */
/* ========================== CUBE ========================== */
/* ========================================================== */

void Cube::draw()
{
	glPushMatrix();
		GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
		GLfloat mat_color[] = {(float)this->getR(), (float)this->getG(), (float)this->getB(), 1.0};
		GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat mat_shininess[] = {(float)this->getShininess()};

		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

		glScaled(width, height, depth);
		glTranslated(x, y, z);
		glutSolidCube(1);
	glPopMatrix();
}


/* ========================================================== */
/* ========================= BULLET ========================= */
/* ========================================================== */

void Bullet::draw()
{
	glPushMatrix();
		glTranslatef(x, y, 0);
		Circle("bulit", 0, 0, hitboxRad, 1, 0.15, 0.15, 75.0).draw();
	glPopMatrix();
}

void Bullet::updatePosition(GLdouble timeDiff)
{
	x -= sin(dir * M_PI / 180.0)*speed*timeDiff;
	y += cos(dir * M_PI / 180.0)*speed*timeDiff;
}

/* =========================================================== */
/* =========================== Car =========================== */
/* =========================================================== */


Car::Car(const char *id, int x, int y, int rad, double yaw, double cSpd, double bSpd, double r, double g, double b) :
	Object(id, r, g, b), x(x), y(y), hitboxRad(rad), yaw(yaw), carSpeed(cSpd), bulletSpeed(bSpd)
{
	wheelYaw = 0;

	bodyHeight = rad * 2;
	bodyWidth = rad * 0.25 * 2;
	suspHeight = rad * 0.1;
	suspWidth = rad * 0.25;
	wheelHeight = rad * 0.40;
	wheelWidth = rad * 0.30;

	wheelTrackHeight = wheelHeight * 0.4;
	// wheelTrackWidth = wheelWidth * 0.7;

	cannonHeight = rad * 0.3;
	cannonWidth = rad * 0.1;

	mouseSens = 0.5;

	cX = x - sin(yaw*M_PI/180) * bodyHeight/2;
	cY = y + cos(yaw*M_PI/180) * bodyHeight/2;

	trackDelta = 0;
	intel = NULL;
}

void setMeshes(char path)
{
	//TODO::this
}


void Car::draw()
{
	glPushMatrix();
		glTranslated(x, y, 0);
		glRotated(yaw, 0, 0, 1);
		//Circle("hitboxCar", 0, 0, hitboxRad, 0.5, 0.5, 0.5).draw();
		Cube("bodyCar", 0, 0, 1, bodyWidth, bodyHeight, bodyDepth, getR(), getG(), getB(), 128).draw();
		
		/*
		Rectangle("bodyCar", 0, 0, bodyWidth, bodyHeight, getR(), getG(), getB()).draw();

		glPushMatrix();
			glTranslated(bodyWidth/2 + suspWidth/2, bodyHeight/2 - wheelHeight/2, 0);
			Rectangle("suspFrontRight", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(suspWidth/2 + wheelWidth/2, 0, 0);
			glRotated(wheelYaw, 0, 0, 1);
			Rectangle("wheelFrontRight", 0, 0, wheelWidth, wheelHeight, getR()*0.3, getG()*0.3, getB()*0.3).draw();

			glPushMatrix();
				glTranslated(0, trackDelta, 0);
				Line("wheelTrackMiddle", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, -wheelHeight/2 + trackDelta, 0);
				Line("wheelTrackLast", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			glTranslated(-bodyWidth/2 - suspWidth/2, bodyHeight/2 - wheelHeight/2, 0);
			Rectangle("suspFrontLeft", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(-suspWidth/2 - wheelWidth/2, 0, 0);
			glRotated(wheelYaw, 0, 0, 1);
			Rectangle("wheelFrontLeft", 0, 0, wheelWidth, wheelHeight, getR()*0.3, getG()*0.3, getB()*0.3).draw();

			glPushMatrix();
				glTranslated(0, trackDelta, 0);
				Line("wheelTrackMiddle", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, -wheelHeight/2 + trackDelta, 0);
				Line("wheelTrackLast", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			glTranslated(bodyWidth/2 + suspWidth/2, - bodyHeight/2 + wheelHeight/2, 0);
			Rectangle("suspBackRight", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(suspWidth/2 + wheelWidth/2, 0, 0);
			Rectangle("wheelBackRight", 0, 0, wheelWidth, wheelHeight, getR()*0.3, getG()*0.3, getB()*0.3).draw();

			glPushMatrix();
				glTranslated(0, trackDelta, 0);
				Line("wheelTrackMiddle", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, -wheelHeight/2 + trackDelta, 0);
				Line("wheelTrackLast", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			glTranslated(-bodyWidth/2 - suspWidth/2, -bodyHeight/2 + wheelHeight/2, 0);
			Rectangle("suspBackLeft", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(-suspWidth/2 - wheelWidth/2, 0, 0);
			Rectangle("wheelBackLeft", 0, 0, wheelWidth, wheelHeight, getR()*0.3, getG()*0.3, getB()*0.3).draw();

			glPushMatrix();
				glTranslated(0, trackDelta, 0);
				Line("wheelTrackMiddle", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, -wheelHeight/2 + trackDelta, 0);
				Line("wheelTrackLast", 0, 0, wheelWidth, 0, 0, 0).draw();
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			glTranslated(0, bodyHeight/2, 0);
			glRotated(-cannonYaw, 0, 0, 1);
			glTranslated(0, cannonHeight/2, 0);
			Rectangle("cannon", 0, 0, cannonWidth, cannonHeight, getR()*0.1, getG()*0.1, getB()*0.1).draw();
		glPopMatrix();*/
	glPopMatrix();
}


void Car::moveFoward(GLdouble timeDiff)
{
	//From https://www.ri.cmu.edu/pub_files/2009/2/Automatic_Steering_Methods_for_Autonomous_Automobile_Path_Tracking.pdf
	double	dxF = sin(wheelYaw * M_PI / 180.0)*carSpeed*timeDiff,
			dyF = cos(wheelYaw * M_PI / 180.0)*carSpeed*timeDiff;

	yaw += atan2(dxF, (bodyHeight/2 + dyF)) * 180.0/M_PI;
	// cout << yaw << endl;

	double	dx = sin(yaw * M_PI / 180.0)*carSpeed*timeDiff,
			dy = cos(yaw * M_PI / 180.0)*carSpeed*timeDiff;

	x -= dx; y += dy;

	cX = x - sin(yaw*M_PI/180) * bodyHeight/2;
	cY = y + cos(yaw*M_PI/180) * bodyHeight/2;

	trackDelta += 0.2;
	if(trackDelta >= wheelHeight * 0.50)
		trackDelta = 0;
}

void Car::moveBackward(GLdouble timeDiff)
{
	double	dxF = -sin(wheelYaw * M_PI / 180.0)*carSpeed*timeDiff,
			dyF = -cos(wheelYaw * M_PI / 180.0)*carSpeed*timeDiff;

	yaw += atan2(dxF, (bodyHeight/2 + dyF)) * 180.0/M_PI;

	double	dx = sin(yaw * M_PI / 180.0)*carSpeed*timeDiff,
			dy = cos(yaw * M_PI / 180.0)*carSpeed*timeDiff;

	x += dx; y -= dy;

	cX = x - sin(yaw*M_PI/180) * bodyHeight/2;
	cY = y + cos(yaw*M_PI/180) * bodyHeight/2;

	trackDelta -= 0.2;
	if(trackDelta < 0)
		trackDelta = wheelHeight * 0.50;
}


vec3 Car::getNextPosition(int direc, GLdouble timeDiff)
{
	double	dxF = sin(wheelYaw * M_PI / 180.0)*carSpeed*timeDiff,
			dyF = cos(wheelYaw * M_PI / 180.0)*carSpeed*timeDiff;

	double fakeyaw = yaw + (atan2(dxF, (bodyHeight/2 + dyF)) * 180.0/M_PI);
	// cout << yaw << endl;

	double	dx = sin(fakeyaw * M_PI / 180.0)*carSpeed*timeDiff,
			dy = cos(fakeyaw * M_PI / 180.0)*carSpeed*timeDiff;

	return vec3(x - (direc) * dx, y + (direc) * dy, 0);
}

void Car::moveWheels(double dyaw, GLdouble timeDiff)
{
	// yaw += dyaw * timeDiff;
	wheelYaw += dyaw * timeDiff;

	if(wheelYaw > 45)
		wheelYaw = 45;
	
	if(wheelYaw < -45)
		wheelYaw = -45;
	
	// cout << wheelYaw << endl;
}

void Car::moveCannon(int x, int y)
{
	int mouseMovedX = x - mouseLastX;

	cannonYaw += mouseMovedX*mouseSens;
	if(cannonYaw > 45)
		cannonYaw = 45;

	if(cannonYaw < -45)
		cannonYaw = -45;

	mouseLastX = x;
}

Bullet* Car::shoot()
{
	return new Bullet("pewpew", cX, cY, cannonWidth, yaw - cannonYaw , bulletSpeed, this->getID());
}

