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

/* ============================================================= */
/* ========================= RECTANGLE ========================= */
/* ============================================================= */

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
	glColor3d(this->getR(), this->getG(), this->getB());

	glPushMatrix();
		glTranslated(x, y, 0);

		static const double circle_points = 100;
		static const float angle = 2.0f * 3.1416f / circle_points;
		glLineWidth(1);
		glPointSize(1);
		glBegin(GL_POLYGON);
			for(double angle1 = 0.0, i = 0; i < circle_points; ++i)
			{
				glVertex3d(radius*cos(angle1), radius * sin(angle1), 0);
				angle1 += angle;
			}
		glEnd();

	glPopMatrix();
}


/* ========================================================== */
/* =========================== IA =========================== */
/* ========================================================== */

// random_device IA::rd;
// mt19937 IA::eng;

// IA::IA(double spm){

// 	IA::eng = mt19937(IA::rd());
// 	this->shootsPerMilli = spm;
// 	this->changeMove = spm*5;
// 	this->lastTime = getCurrentTimeMS();
// 	// cout << seekTimer << " " << instrTimer << endl;
	
// 	uniform_int_distribution<> distr(0, 8);
// 	this->current_instr = static_cast<instr_t>(distr(IA::eng));
// }

// void IA::doStep()
// {
// 	long long now = getCurrentTimeMS();
// 	fillShoot += (double)(now - this->lastTime) * shootsPerMilli;

// 	if(fillShoot >= 1)
// 	{
// 		this->current_instr = SEEK;
// 	}else{
// 		fillChange += (now - this->lastTime) * changeMove;
// 		if(fillChange >= 1)
// 		{
// 			uniform_int_distribution<> distr(0, 8);
// 			this->current_instr = static_cast<instr_t>(distr(IA::eng));
// 			fillChange = 0;
// 		}
// 	}
	
// 	this->lastTime = now;
// }

// void IA::setSeekDone()
// {
// 	this->fillShoot = 0;
// 	this->fillChange = 1;
// }

/* =========================================================== */
/* =========================== Car =========================== */
/* =========================================================== */


Car::Car(const char *id, int x, int y, int rad, double yaw, double spd, double r, double g, double b) :
	Object(id, r, g, b), x(x), y(y), hitboxRad(rad), yaw(yaw), carSpeed(spd)
{
	wheelYaw = 0;

	bodyHeight = rad * 2;
	bodyWidth = rad * 0.25 * 2;
	suspHeight = rad * 0.1;
	suspWidth = rad * 0.25;
	wheelHeight = rad * 0.33;
	wheelWidth = rad * 0.25;
}

void Car::draw()
{
	glPushMatrix();
		glTranslated(x, y, 0);
		glRotated(yaw, 0, 0, 1);
		Circle("hitboxCar", 0, 0, hitboxRad, 0.5, 0.5, 0.5).draw();
		Rectangle("bodyCar", 0, 0, bodyWidth, bodyHeight, getR(), getG(), getB()).draw();

		glPushMatrix();
			glTranslated(bodyWidth/2 + suspWidth/2, bodyHeight/2 - wheelHeight/2, 0);
			Rectangle("suspFrontRight", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(suspWidth/2 + wheelWidth/2, 0, 0);
			glRotated(wheelYaw, 0, 0, 1);
			Rectangle("wheelFrontRight", 0, 0, wheelWidth, wheelHeight, getR(), getG(), getB()).draw();
		glPopMatrix();

		glPushMatrix();
			glTranslated(-bodyWidth/2 - suspWidth/2, bodyHeight/2 - wheelHeight/2, 0);
			Rectangle("suspFrontLeft", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(-suspWidth/2 - wheelWidth/2, 0, 0);
			glRotated(wheelYaw, 0, 0, 1);
			Rectangle("wheelFrontLeft", 0, 0, wheelWidth, wheelHeight, getR(), getG(), getB()).draw();
		glPopMatrix();

		glPushMatrix();
			glTranslated(bodyWidth/2 + suspWidth/2, - bodyHeight/2 + wheelHeight/2, 0);
			Rectangle("suspBackRight", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(suspWidth/2 + wheelWidth/2, 0, 0);
			Rectangle("wheelBackRight", 0, 0, wheelWidth, wheelHeight, getR(), getG(), getB()).draw();
		glPopMatrix();

		glPushMatrix();
			glTranslated(-bodyWidth/2 - suspWidth/2, -bodyHeight/2 + wheelHeight/2, 0);
			Rectangle("suspBackLeft", 0, 0, suspWidth, suspHeight, getR()*0.5, getG()*0.5, getB()*0.5).draw();
	
			glTranslated(-suspWidth/2 - wheelWidth/2, 0, 0);
			Rectangle("wheelBackLeft", 0, 0, wheelWidth, wheelHeight, getR(), getG(), getB()).draw();
		glPopMatrix();
	glPopMatrix();
}


void Car::moveFoward(GLdouble timeDiff)
{
	int dx = round(sin(yaw * M_PI / 180.0)*carSpeed*timeDiff);
	int dy = round(cos(yaw * M_PI / 180.0)*carSpeed*timeDiff);

	x += dx; y += dy;
}

void Car::moveBackward(GLdouble timeDiff)
{
	int dx = round(sin(yaw * M_PI / 180.0)*carSpeed*timeDiff);
	int dy = round(cos(yaw * M_PI / 180.0)*carSpeed*timeDiff);

	x -= dx; y -= dy;
}


vec3 Car::getNextPosition(int direc, GLdouble timeDiff)
{
	int dx = round(sin(yaw * M_PI / 180.0)*carSpeed);
	int dy = round(cos(yaw * M_PI / 180.0)*carSpeed);

	return vec3(x + (direc) * dx, y - (direc) * dy, 0);
}

void Car::moveWheels(double dyaw, GLdouble timeDiff)
{
	// yaw += dyaw * timeDiff;	
	wheelYaw += dyaw * timeDiff;	

	// if(yaw >= 360)
	// 	yaw -= 360;

	// if(yaw < 0)
	// 	yaw += 360;
}