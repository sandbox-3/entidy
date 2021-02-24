#pragma once

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sstream>

namespace entidy::spaceinvaders
{
using namespace std;

class ConsoleException : virtual public std::exception
{
protected:
	std::string error_message;

public:
	explicit ConsoleException(const std::string& msg)
		: error_message(msg)
	{ }
	virtual ~ConsoleException() throw() { }
	virtual const char* what() const throw()
	{
		return error_message.c_str();
	}
};

struct Color
{
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
};

struct Pixel
{
	char Glyph = ' ';
	Color Background;
	Color Foreground;


	string Str() const
	{
        stringstream ss;
		ss << "\033[";
        ss << "38;2;";
        ss << to_string(Foreground.R) << ';';
        ss << to_string(Foreground.G) << ';';
        ss << to_string(Foreground.B) << ";";
        ss << "48;2;";
        ss << to_string(Background.R) << ';';
        ss << to_string(Background.G) << ';';
        ss << to_string(Background.B) << 'm';
        ss << Glyph;
        return ss.str();
    }
};

class ConsoleImpl;
using Console = shared_ptr<ConsoleImpl>;
class ConsoleImpl
{
protected:
	struct winsize size;
	vector<shared_ptr<Pixel>> buffer;

	Pixel* Get(ushort x, ushort y)
	{
		ushort index = y * size.ws_col + x;
		if(!buffer[index])
			buffer[index] = make_shared<Pixel>();
        return buffer[index].get();
	}

public:
    ConsoleImpl()
    {
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		buffer.resize(size.ws_row * size.ws_col);
		cout << "\033c" << flush;
    }

	void Render()
	{
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		buffer.resize(size.ws_row * size.ws_col);

		for(ushort y = 0; y < size.ws_row; y++)
		{
			for(ushort x = 0; x < size.ws_col; x++)
			{
                cout << "\033[" << y+1 << ";" << x+1 << "f";
				cout << Get(x, y)->Str();
			}
		}
        cout << "\033[1B" << '\r';
        cout << "\033[2K";
		cout << "\033[0m";
		cout << std::flush;
	}

	size_t Cols() const
	{
		return size.ws_col;
	}

	size_t Rows() const
	{
		return size.ws_row;
	}

	Pixel* At(ushort x, ushort y)
	{
		if(x >= size.ws_col || x < 0 || y >= size.ws_row || y < 0)
			return Get(0,0);
		return Get(x, y);
	}
};

} // namespace entidy::spaceinvaders