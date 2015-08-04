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
 * \section command_sec Commands
 *
 * Ten commands are included in the monitor: <br/>
 *     prompt(s) - Select the prompt for input <br/>
 *       set(cs) - Set register to string <br/>
 *        get(c) - Display register <br/>
 *       add(dd) - Add two numbers <br/>
 *       sub(dd) - Subtract two numbers <br/>
 *       mul(dd) - Multiply two numbers <br/>
 *         hex() - Toggle output base <br/>
 *      echo(s+) - Display parameter <br/>
 *        help() - Display this help <br/>
 *        exit() - Exit monitor <br/>
 * brackets are optional, parameters must be seperated by comma's or space's.
 * 's' parameters are strings. Quotes are required if they contains spaces.
 * 'c' parameters are register names, by default a to h.
 * 'd' parameters are numbers. hexadecimal numbers must have a leading '0x'.
 * '+' after a parameter indicates that it can be repeated. echo takes one of more strings as parameters.
 * commands may return a value which the monitor will print (get, add, sub, mul and
 * echo do this), and/or the may print their output directly (hex and echo do this).
 * A parameter can be replaced by a command in a string by preceding it with a '!'.
 * Example: To set register a to the sum of 3 and 5 enter the command <b>set a !"add 3 5"</b>.
 *
 * \section addCmd_sec Adding commands
 *
 * To add a command named 'newcmd'.
 * In the commands.c module:
 * Add a <b>MK_CMD(newcmd);</b> with the other function definitions.
 * Add a <b>CMD(newcmd, "parameter codes", "Command description")</b> entry to the dsp_table.
 * In the commands.c module or, preferably, a new module:
 * Add a function <b>token_t* cmd_newcmd(token_t *args[], int nArgs)</b> to implement the new command.
 *
 * \section regiter_sec Registers
 *
 * Registers are places where a string or a number can be3 kept for later use.
 * There are two commands for accessing registers: set and get. 'set' takes the name
 * of a register and a value (string or number) and copies the value into the
 * register. 'get' returns the value of a given register which the monitor displays.
 * The value of a register can be inserted into a command by prefixing the
 * name of the register with a $ sign.
 * \note <b>get a</b> is equivalent to <b>echo $a</b>.
 * \note To increment register a enter <b>set a !"add $a 1"</b>.
 *
 * \section install_sec Installation
 *
 * All the c, h & re2c files here except main.h and main.c are part of
 * the monitor. main.c and main.h are included to allow building a "test
 * version" that runs in a unix environment. 
 * 
 * \subsection build_sec Building
 * Two non-standard tools are required:<br/>
 * re2c - "A tool for writing very fast and very flexible scanners." is available at 'http://re2c.org/'.<br/>
 * unifdef - "A utility selectively processes conditional C preprocessor #if and #ifdef directives." is available at 'http://dotat.at/prog/unifdef/'.<br/>
 * Once the preceding tools are installed just make.<br/>
 * There are four flags defined in the makefile, they are included to allow shrinking the monitor when flash or ram are in short supply:<br/>
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
