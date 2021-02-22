#pragma once

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace entidy::spaceinvaders
{
using namespace std;

struct Pixel
{
    char glyph;
    short br, bg, bb;
    short fr, fg, fb;

    void print()
    {
        cout << "\033[38;2;" << fr << ';' << fg << ';' << fb << ";48;2;" << br << ';' << bg << ';' << bb << 'm' << glyph;
    }
};

class Console
{
    public:

	void Update()
	{
		struct winsize size;
        while(true)
        {
		    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

            cout << "\033c";
            cout << size.ws_col << " " << size.ws_row << endl;
            usleep(1000);
        }

        cout << "\033c";
        Pixel p;
        p.glyph = 'a';
        p.fr = 222;
        p.fg = 14;
        p.fb = 120;
        p.br = 0;
        p.bg = 0;
        p.bb = 0;
        p.print();
        cout << "\033[0m";
        cout << endl;

        // int i = 0;
        // bool dir = true;
        // while(true)
        // {
        //     printf("\033c");
        //     printf("\n");

        //     for(int n = 0; n < i; n++)
        //         printf(" ");

        //     i = dir ? i + 1 : i - 1;
        //     if(i >= 100 || i <= 0)
        //         dir = !dir;

        //     cout << "\033[38;2;" << 44 << ';' << 44 << ';' << 255 << "m" << "TEST";
        //     //printf("\033[38;2;244;22;232mTEST");

        //     printf("\x1B[31mTexting\033[0m\t\t");
        //     printf("\033[3;43;30mTexting\033[0m\t\t");
        //     printf("\n");
        //     usleep(33000);
        // }

        //     // printf("\x1B[32mTexting\033[0m\t\t");
        //     // printf("\x1B[33mTexting\033[0m\t\t");
        //     // printf("\x1B[34mTexting\033[0m\t\t");
        //     // printf("\x1B[35mTexting\033[0m\n");
            
        //     // printf("\x1B[36mTexting\033[0m\t\t");
        //     // printf("\x1B[36mTexting\033[0m\t\t");
        //     // printf("\x1B[36mTexting\033[0m\t\t");
        //     // printf("\x1B[37mTexting\033[0m\t\t");
        //     // printf("\x1B[93mTexting\033[0m\n");
            
        //     // printf("\033[3;42;30mTexting\033[0m\t\t");
        //     // printf("\033[3;43;30mTexting\033[0m\t\t");
        //     // printf("\033[3;44;30mTexting\033[0m\t\t");
        //     // printf("\033[3;104;30mTexting\033[0m\t\t");
        //     // printf("\033[3;100;30mTexting\033[0m\n");

        //     // printf("\033[3;47;35mTexting\033[0m\t\t");
        //     // printf("\033[2;47;35mTexting\033[0m\t\t");
        //     // printf("\033[1;47;35mTexting\033[0m\t\t");
        //     // printf("\t\t");
        //     // printf("\n");
	}

};

} // namespace entidy::spaceinvaders