/**
 * \file print.c
 * \brief Number formatting that doesn't blow the budget
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

#include "print.h"

#define INT_DIGITS 11		/* enough for 32 bit integer */

static char buf[INT_DIGITS + 2];

/**
 * \brief Format Decimal Number (base 10)
 * \param i Number to format.
 * \param width Minimum width of result (pad with leading blanks).
 * \result string containing formatted number, must be used before
 * next 'formatNum' call.
 */
char* formatDecimal(int i, unsigned width)
{
	char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
	*p = '\000';

	if (i >= 0) {
		do {
			*--p = '0' + (i % 10);
			i /= 10;
		} while (i != 0);
	} else {			/* i < 0 */
		do {
			*--p = '0' - (i % 10);
			i /= 10;
		} while (i != 0);
		*--p = '-';
	}
	for (int j=(buf+INT_DIGITS+1)-p; j<width; j++)
		*--p = ' ';
	return p;
}

/**
 * Format Hexadecimal Number (base 16)
 * \param i Number to format.
 * \param prefix Set true to add '0x' prefix to result.
 * \param width minimum width of result (pad with '0's after prefix)
 * \result string containing formatted number, must be used before
 * next 'formatNum' call.
 */
char* formatHex(unsigned i, bool prefix, unsigned width)
{
	char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
	*p = '\000';
	do {
		if ((i % 16) < 10)
			*--p = '0' + (i % 16);
		else
			*--p = 'A' + (i % 16) - 10;
		i /= 16;
	} while (i != 0);
	for (int j=(buf+INT_DIGITS+1)-p; j<width; j++)
		*--p = '0';
	if (prefix) {
		*--p = 'x';
		*--p = '0';
	}
	return p;
}

/**
 * Format Number
 * \param i number to format.
 * \param outputDecimal true for decimal otherwise hexadecimal.
 * \result string containing formatted number, must be used before
 * next 'formatNum' call.
 * \note Number will not be padded and hex numbers will be prefixed
 * with '0x'.
 */
char* formatNum(int i, bool outputDecimal)
{
	if (outputDecimal)
		return formatDecimal(i, 0);
	return formatHex((unsigned)i, true, 0);
}

