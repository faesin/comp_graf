#include "botia.h"

random_device IA::rd;
mt19937 IA::eng;

IA::IA(double spm) :
	shootsPerMilli(spm), changeMove(spm*7)
{
	IA::eng = mt19937(IA::rd());
	uniform_int_distribution<> distr(0, 2);
	current_instr = static_cast<instr_t>(distr(IA::eng));

	//cout << current_instr << endl;
}

void IA::doStep(double timeDiff)
{
	if(enableInt)
	{
		fillShoot += timeDiff * shootsPerMilli;

	 	if(fillShoot >= 1)
		{
			current_instr = SHOOT;
			fillShoot = 0;
			fillChange = 1;
		}else{
			fillChange += timeDiff * changeMove;
			if(fillChange >= 1)
			{
				uniform_int_distribution<> distr(0, 2);
				current_instr = static_cast<instr_t>(distr(IA::eng));
				fillChange = 0;
			}
		}
	}
}
