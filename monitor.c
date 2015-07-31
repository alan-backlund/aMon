/**
 * \file monitor.c
 * \brief Collect character input and form command strings.
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

#include <stdbool.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "process.h"
#include "main.h"

#ifdef BIG
#define BS	0x7F	//!< erase last character, delete on Unix's
#define LE  0x0A	//!< line end
#define UP	0x15	//!< control u
#define DN	0x04	//!< control d
#else
#define BS	0x08	//!< erase last character, backspace on MCU's
#define LE  0x0D	//!< line end
#define UP	0x15	//!< control u
#define DN	0x04	//!< control d
#endif

static char buf[4][MAX_STRING];
static unsigned bufIdx[4] = { 0, 0, 0, 0 };
static unsigned curBuf = 0;
static unsigned histBuf = 0;
char prompt[20] = { '>', 0 };
bool monExit;
 
/**
 * \brief Buffer index increment
 * \param bn  
 */
void bufIdxInc(int bn)
{
	if ((bufIdx[bn] + 1) < MAX_STRING-2)
		bufIdx[bn]++;
}

/**
 * \brief Compose the current input line. Supports backspacing and copmmand history.
 * @param c latest input character
 */
void readLine(char c)
{
	if (c == LE) {  // Line End
		transmitString(EOL);
		buf[curBuf][bufIdx[curBuf]++] = 0;  // mark end of string
		buf[curBuf][bufIdx[curBuf]] = 0;  // eval looks past end, so mark it again
		token_t* rslt = eval(buf[curBuf]);
		if (rslt != NULL) {
			// Print result of eval (maybe)
			if ((rslt->t != EMPTY) && (rslt->t != ERR)) {
				char *s = tokenGetText(rslt);
				transmitString(s);
				transmitString(EOL);
			}
			tokenFree(rslt);
		}
		// If not done print a new prompt
		if (!monExit) {
			transmit(prompt, strlen(prompt));
			transmit(" ", 1);
		}
		curBuf = (curBuf + 1) & 3;
		histBuf = curBuf;
		bufIdx[curBuf] = 0;
	} else if (c == BS) {  // Backspace
		if (bufIdx[curBuf] > 0) {
			transmit("\x08 \x08", 3);
			bufIdx[curBuf]--;
		}
	} else if (c == UP) {
		histBuf = (histBuf - 1) & 3;
		if (histBuf != curBuf) {
			// Erase current input from console
			for (int i=0; i<bufIdx[curBuf]; i++)
				transmit("\x08 \x08", 3);
			// copy history into current
			strcpy(buf[curBuf], buf[histBuf]);
			bufIdx[curBuf] = bufIdx[histBuf]-1;
			// Display new input on console
			for (int i=0; i<bufIdx[curBuf]; i++)
				transmit(&buf[curBuf][i], 1);
		} else
			histBuf = (histBuf + 1) & 3;
	} else if (c == DN) {
		histBuf = (histBuf + 1) & 3;
		if (histBuf != curBuf) {
			// Erase current input
			for (int i=0; i<bufIdx[curBuf]; i++)
				transmit("\x08 \x08", 3);
			strcpy(buf[curBuf], buf[histBuf]);
			bufIdx[curBuf] = bufIdx[histBuf]-1;
			for (int i=0; i<bufIdx[curBuf]; i++)
				transmit(&buf[curBuf][i], 1);
		} else {
			for (int i=0; i<strlen(buf[curBuf]); i++)
				transmit("\x08 \x08", 3);
			bufIdx[curBuf] = 0;
		}
	} else {
		buf[curBuf][bufIdx[curBuf]] = c;
		bufIdxInc(curBuf);
		transmit(&c, 1);
	}
}

/**
 * \brief Process input character.
 * Converts up and down arrow keys into UP and DN characters.
 * \param c  The input character.
 */
void processChar(char c)
{
	static int state = 0;

	if (state == 0) {
		if (c == 0x1B) {
			state = 1;
		} else {
			readLine(c);
		}
	} else if (state == 1) {
		if (c == 0x5B) {
			state = 2;
		} else {
			readLine(0x1B);
			state = 0;
		}
	} else /* state == 2 */ {
		if (c == 0x41) {
			readLine(UP);
		} else if (c == 0x42) {
			readLine(DN);
		} else {
			readLine(0x1B);
			readLine(0x5B);
			readLine(c);
		}
		state = 0;
	}
}
