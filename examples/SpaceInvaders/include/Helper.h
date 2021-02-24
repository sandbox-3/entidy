#pragma once
#include <stdlib.h>
#define VIEWPORT_W 135
#define VIEWPORT_H 45

namespace entidy::spaceinvaders
{
using namespace std;
using namespace entidy;

class Helper
{
public:
	static int RandInt(int min, int max)
	{
		return rand() % (max - min) + min;
	}

	static double RandDouble(double min, double max)
	{
		return (double(rand()) / double(RAND_MAX)) * (max - min) + min;
	}

	static double RandBool(double prob)
	{
		return RandDouble(0,1) <= prob;
	}
};

} // namespace entidy::spaceinvaders