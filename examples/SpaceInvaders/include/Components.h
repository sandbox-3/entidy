#pragma once
#include <memory>
#include <vector>

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

struct Sprite
{
    public:
    struct Frame
    {
        struct Color
        {
            short R;
            short G;
            short B;
        };

        string glyphs;
        Color bgcolor;
        Color fgcolor;
    };

    vector<Frame> frames;

    uint8_t cols;
    uint8_t rows;
    
    float speed;
    float frame;
};

} // namespace entidy::spaceinvaders