#pragma once
#include <sstream>
#include <string>

#include "Components.h"
#include "Helper.h"

namespace entidy::spaceinvaders
{
using namespace std;
using namespace entidy;

class SpriteFactory
{
public:
	static string Enemy0(int frame)
	{
		if(frame == 0)
		{
			stringstream ss;
			ss << "/MMM\\";
			ss << "\\~=~/";
			ss << " ||| ";
			return ss.str();
		}
		else
		{
			stringstream ss;
			ss << "/MMM\\";
			ss << "\\=~=/";
			ss << " /|\\ ";
			return ss.str();
		}
	}

	static string Enemy1(int frame)
	{
		if(frame == 0)
		{
			stringstream ss;
			ss << "/d-b\\";
			ss << "|o^o|";
			ss << " |=| ";
			return ss.str();
		}
		else
		{
			stringstream ss;
			ss << "/b-d\\";
			ss << "|ovo|";
			ss << " /=\\ ";
			return ss.str();
		}
	}

	static string Enemy2(int frame)
	{
		if(frame == 0)
		{
			stringstream ss;
			ss << "d0-0b";
			ss << "^/ \\^";
			ss << " [=] ";
			return ss.str();
		}
		else
		{
			stringstream ss;
			ss << "d0-0b";
			ss << "^| |^";
			ss << " ]=[ ";
			return ss.str();
		}
	}

	static Sprite Enemy(int id)
	{
		Sprite enemy;
		enemy.cols = 5;
		enemy.rows = 3;
		enemy.speed = 0.25;
		enemy.frame = 0;

		Sprite::Frame frame;
		frame.bgcolor.R = -1;
		frame.bgcolor.G = -1;
		frame.bgcolor.B = -1;

		frame.fgcolor.R = 255;
		frame.fgcolor.G = 255;
		frame.fgcolor.B = 255;

		switch(id)
		{
		case 0:
			frame.glyphs = Enemy0(0);
			enemy.frames.push_back(frame);

			frame.glyphs = Enemy0(1);
			enemy.frames.push_back(frame);
			break;
		case 1:
			frame.glyphs = Enemy1(0);
			enemy.frames.push_back(frame);

			frame.glyphs = Enemy1(1);
			enemy.frames.push_back(frame);
			break;
		case 2:
			frame.glyphs = Enemy2(0);
			enemy.frames.push_back(frame);

			frame.glyphs = Enemy2(1);
			enemy.frames.push_back(frame);
			break;
		
		}

		return enemy;
	}

	static Sprite Player()
	{
		Sprite player;
		player.cols = 9;
		player.rows = 1;
		player.speed = 1;
		player.frame = 0;

		Sprite::Frame frame;
		frame.bgcolor.R = -1;
		frame.bgcolor.G = -1;
		frame.bgcolor.B = -1;

		frame.fgcolor.R = 255;
		frame.fgcolor.G = 0;
		frame.fgcolor.B = 0;

		frame.glyphs = "()-(0)-()";
		player.frames.push_back(frame);

		return player;
	}

	static Sprite Bullet()
	{
		Sprite bullet;
		bullet.cols = 1;
		bullet.rows = 1;
		bullet.speed = 1;
		bullet.frame = 0;

		for(size_t i = 0; i < 10; i++)
		{
			Sprite::Frame frame;
			frame.bgcolor.R = -1;
			frame.bgcolor.G = -1;
			frame.bgcolor.B = -1;

			frame.fgcolor.R = Helper::RandInt(0, 255);
			frame.fgcolor.G = Helper::RandInt(0, 255);
			frame.fgcolor.B = Helper::RandInt(0, 255);

			frame.glyphs = "^";

			bullet.frames.push_back(frame);
		}
		return bullet;
	}
};

} // namespace entidy::spaceinvaders