/**
 * \file main.c
 * \brief Monitor input and output
 * \author    Alan Backlund
 * \version   1.0
 * \date      2015 Jul 29
 * \copyright 2015 Alan Backlund
 * \section License The MIT License (MIT)
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*! \mainpage aMon - another Monitor
 * \author    Alan Backlund
 * \version   1.0
 * \date      2015 Jul 29
 * \copyright 2015 Alan Backlund
 * \section License The MIT License (MIT)
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * \section intro_sec Introduction
 *
 * This program is an example of an implementation of aMon. aMon is
 * intended primarily test tool for devices containing a small (<64k
 * flash) MPU.
 *
 * \section install_sec Installation
 *
 * All the c, h & re2c files here except main.h and main.c are part of
 * the monitor. main.c and main.h are included to allow building a "test
 * version" that runs in a unix environment. 
 * 
 * \subsection build_sec Building
 * Just make.<BR/>
 * There are four flags defined in the makefile:<br/>
 * <b>INCL_REG</b> Include support for registers <br/>
 * <b>INCL_EXIT</b> Include "exit" command.<br/>
 * <b>INCL_MATH</b> Include math commands (add, sub & mul).<br/>
 * <b>BIG</b> Build for hosted environment, i.e. lots of space.<br/>
 */
 
 #include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "monitor.h"
#include "main.h"


#if 0
#define DEBUG(s)	s
#else
#define DEBUG(s)
#endif

static struct termios old_tio, new_tio;
 
/**
 * \brief Print a string on the console.
 * \param pData  A pointer to the string.
 * \param size  The length of the string.
 */
void transmit(char *pData, unsigned size)
{
	for (int i=0; i<size; i++)
		putchar(pData[i]);
}

/**
 * \brief Collect input characters and pass them to monitor
 */
int main()
{
	char c;

	/* get the terminal settings for stdin */
	tcgetattr(STDIN_FILENO,&old_tio);

	/* we want to keep the old setting to restore them a the end */
	new_tio = old_tio;

	/* disable canonical mode (buffered i/o) and local echo */
	new_tio.c_lflag &= (~ICANON & ~ECHO);

	/* set the new settings immediately */
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

	transmit(prompt, strlen(prompt));
	transmit(" ", 1);
	monExit = false;
	do {
		 c = getchar();
		 if (c == EOF)
			break;
		 processChar(c);
	} while (!monExit);
	transmit(EOL, 1);
	
	/* restore the former settings */
	tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

	return 0;
}
