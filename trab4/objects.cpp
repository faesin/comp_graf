#include "objects.h"

/* =========================================================== */
/* ========================= GLOBALS ========================= */
/* =========================================================== */

long long getCurrentTimeMS();

static int g_limitX, g_limitY;

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


/* ========================================================== */
/* ========================= CIRCLE ========================= */
/* ========================================================== */

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


/* ========================================================== */
/* ========================= BULLET ========================= */
/* ========================================================== */


void Bullet::draw()
{
	glPushMatrix();
		glTranslated(x, y, 0);
		Circle("bulit", 0, 0, hitboxRad, 0.15, 0.15, 0.15).draw();
	glPopMatrix();
}

void Bullet::updatePosition(double timeDiff)
{
	x += sin(dir * M_PI / 180.0)*speed*timeDiff;
	y -= cos(dir * M_PI / 180.0)*speed*timeDiff;
}


/* ========================================================== */
/* =========================== IA =========================== */
/* ========================================================== */

random_device IA::rd;
mt19937 IA::eng;

IA::IA(double spm){

	IA::eng = mt19937(IA::rd());
	this->shootsPerMilli = spm;
	this->changeMove = spm*5;
	this->lastTime = getCurrentTimeMS();
	// cout << seekTimer << " " << instrTimer << endl;
	
	uniform_int_distribution<> distr(0, 8);
	this->current_instr = static_cast<instr_t>(distr(IA::eng));
}

void IA::doStep()
{
	long long now = getCurrentTimeMS();
	fillShoot += (double)(now - this->lastTime) * shootsPerMilli;

	if(fillShoot >= 1)
	{
		this->current_instr = SEEK;
	}else{
		fillChange += (now - this->lastTime) * changeMove;
		if(fillChange >= 1)
		{
			uniform_int_distribution<> distr(0, 8);
			this->current_instr = static_cast<instr_t>(distr(IA::eng));
			fillChange = 0;
		}
	}
	
	this->lastTime = now;
}

void IA::setSeekDone()
{
	this->fillShoot = 0;
	this->fillChange = 1;
}

/* =========================================================== */
/* ========================= CHOPPER ========================= */
/* =========================================================== */


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

	fuel = 150;
	fuelMax = 150;
}

void Chopper::setLimits(int x, int y)
{
	g_limitX = x;
	g_limitY = y;
}

void Chopper::draw()
{
	double uniX = 0, uniY = -1;
	if(drawHitbox)
	{
		glPushMatrix();
			glTranslated(x, y, 0);
			Circle("hitbox", 0, 0, hitboxRad, 0.5, 0.5, 0.5).draw();
		glPopMatrix();
	}
		
	glPushMatrix();
		glTranslated(x, y, 0);
		gunx = x;
		guny = y;

		glRotated(yaw, 0, 0, 1);
		
		uniX = sin(yaw*M_PI/180);
		uniY = cos(yaw*M_PI/180);

		gunx += uniX * bodyWidth/2;
		guny -= uniY * bodyHeight/2;

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

void Chopper::refuel(double delta)
{
	// cout << fuel + delta << endl;
	if(fuel + delta <= fuelMax)
		this->fuel += delta;
};

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

		if(x + hitboxRad + dx > g_limitX || x - hitboxRad + dx < 0)
			dx = 0;

		if(y + hitboxRad - dy > g_limitY || y - hitboxRad - dy < 0)
			dy = 0;

		x += dx; y -= dy;

		fuel -= 0.1;
	}
}

void Chopper::moveBackward()
{
	if(state)
	{
		int dx = round(sin(yaw * M_PI / 180.0)*chpSpeed);
		int dy = round(cos(yaw * M_PI / 180.0)*chpSpeed);

		if(x + hitboxRad - dx > g_limitX || x - hitboxRad - dx < 0)
			dx = 0;

		if(y + hitboxRad + dy > g_limitY || y - hitboxRad + dy < 0)
			dy = 0;

		x -= dx; y += dy;

		fuel -= 0.1;
	}
}

void Chopper::moveByIA()
{
	if(this->intel)
	{
		switch(this->intel->getInstr())
		{
			case STAY:
				break;
			case TURN_R:
				this->pivot(this->chpSpeed);
				break;
			case TURN_L:
				this->pivot(-this->chpSpeed);
				break;
			case FOWARD:
				this->moveFoward();
				break;
			case TURNFWRD_L:
				this->pivot(-this->chpSpeed);
				this->moveFoward();
			case TURNFWRD_R:
				this->pivot(this->chpSpeed);
				this->moveFoward();
				break;
			case BACKWARD:
				this->moveBackward();
				break;
			case TURNBACK_L:
				this->pivot(-this->chpSpeed);
				this->moveBackward();
			case TURNBACK_R:
				this->pivot(this->chpSpeed);
				this->moveBackward();
				break;
			default:
				break;
		}
	}
}

vec3 Chopper::getNextPosition(int direc)
{
	if(this->intel)
	{
		switch(this->intel->getInstr())
		{
			case STAY:
			case TURN_R:
			case TURN_L:
				direc = 0;
				break;
			case FOWARD:
			case TURNFWRD_L:
			case TURNFWRD_R:
				direc = 1;
				break;
			case BACKWARD:
			case TURNBACK_L:
			case TURNBACK_R:
				direc = -1;
				break;
			default:
				break;
		}
	}

	int dx = round(sin(yaw * M_PI / 180.0)*chpSpeed);
	int dy = round(cos(yaw * M_PI / 180.0)*chpSpeed);

	return vec3(x + (direc) * dx, y - (direc) * dy, 0);
}

void Chopper::pivot(double dyaw)
{
	if(state)
		yaw += dyaw;			

	if(yaw >= 360)
		yaw -= 360;

	if(yaw < 0)
		yaw += 360;

	fuel -= 0.1;
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
	if(state)
	{
		// gunx = this->x + round(sin(yaw * M_PI / 180.0)*(bodyWidth/2)*1.5);
		// guny = this->y - round(cos(yaw * M_PI / 180.0)*(bodyWidth/2)*1.5);

		return new Bullet("pewpew", gunx, guny, gunWidth/2, gunAngle + yaw, bltSpeed, this->getID());
	}

	return NULL;
}

ostream& operator<<(std::ostream& out, const Chopper& c)
{
	return out << c.getID() << " -> x:" << c.getX() << " y:" << c.getY() << " yaw:" << c.getYaw();
}

long long getCurrentTimeMS()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; //get current timestamp in milliseconds
	// std::cout << mslong << std::endl;
	return mslong;
}