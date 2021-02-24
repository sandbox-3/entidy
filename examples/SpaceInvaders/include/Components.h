#pragma once
#include <memory>

namespace entidy::spaceinvaders
{
using namespace std;

struct Vec2f
{
	double x;
	double y;

	Vec2f() { }
    
	Vec2f(double _x, double _y)
		: x(_x)
		, y(_y)
	{ }
};

struct Vec2s
{
	short x;
	short y;

    Vec2s(){};

	Vec2s(short _x, short _y)
		: x(_x)
		, y(_y)
	{ }
};

struct BGFXRipple
{
	Vec2f center;
	double radius;
	double intensity;

	BGFXRipple() { }

	BGFXRipple(const Vec2f& _center, double _radius, double _intensity)
		: center(_center)
		, radius(_radius)
		, intensity(_intensity)
	{ }
};

enum InputCommand
{
    A,
    D,
    SPACE,
    P,
    Q
};

enum BoundaryAction
{
    BOUNCE,
    WARP,
    DISAPPEAR
};

} // namespace entidy::spaceinvaders