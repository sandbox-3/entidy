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
    char bgcolor;
    char fgcolor;
};

class Console
{
    public:

	void Update()
	{
		struct winsize size;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

        cout << size.ws_col << endl;
        cout << size.ws_row << endl;

        int i = 0;
        bool dir = true;
        while(true)
        {
            printf("\033c");
            printf("\n");

            for(int n = 0; n < i; n++)
                printf(" ");

            i = dir ? i + 1 : i - 1;
            if(i >= 100 || i <= 0)
                dir = !dir;

            printf("\x1B[31mTexting\033[0m\t\t");
            printf("\033[3;42;30mTexting\033[0m\t\t");
            printf("\n");
            usleep(33000);
        }

            // printf("\x1B[32mTexting\033[0m\t\t");
            // printf("\x1B[33mTexting\033[0m\t\t");
            // printf("\x1B[34mTexting\033[0m\t\t");
            // printf("\x1B[35mTexting\033[0m\n");
            
            // printf("\x1B[36mTexting\033[0m\t\t");
            // printf("\x1B[36mTexting\033[0m\t\t");
            // printf("\x1B[36mTexting\033[0m\t\t");
            // printf("\x1B[37mTexting\033[0m\t\t");
            // printf("\x1B[93mTexting\033[0m\n");
            
            // printf("\033[3;42;30mTexting\033[0m\t\t");
            // printf("\033[3;43;30mTexting\033[0m\t\t");
            // printf("\033[3;44;30mTexting\033[0m\t\t");
            // printf("\033[3;104;30mTexting\033[0m\t\t");
            // printf("\033[3;100;30mTexting\033[0m\n");

            // printf("\033[3;47;35mTexting\033[0m\t\t");
            // printf("\033[2;47;35mTexting\033[0m\t\t");
            // printf("\033[1;47;35mTexting\033[0m\t\t");
            // printf("\t\t");
            // printf("\n");
	}


};

} // namespace entidy::spaceinvaders