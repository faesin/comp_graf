#ifndef _BOTIA_H_
#define _BOTIA_H_

#include <iostream>
#include <sstream>
#include <cstring>
#include <time.h> 
#include <math.h>

#include <random>
#include <sys/time.h>
#include <chrono>

#define _USE_MATH_DEFINES

using namespace std;

typedef enum {
	FORWARD = 0,
	TURNFWRD_R,
	TURNFWRD_L,
	BACKWARD,
	TURNBACK_R,
 	TURNBACK_L,
	SHOOT
} instr_t;

class IA
{
        instr_t current_instr;

        bool enableInt = false;

        double fillShoot, fillChange;
        double shootsPerMilli, changeMove;

		static random_device rd;
		static mt19937 eng;

public:

        IA(double seekTimer);
        ~IA(){};

        void setInstr(int instr){ current_instr = static_cast<instr_t>(instr); };
        int getInstr() const { if(enableInt) return current_instr; else return -1; };

        void switchInt(){ enableInt = !enableInt; };
        void doStep(double);

};

#endif
