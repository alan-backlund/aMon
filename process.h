/**
 * \file process.h
 * \brief Evaluate command strings.
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

#if !defined(_PROCESS_H)
#define _PROCESS_H

#include "lexer.h"

#define MAX_ARGS		16
#define NUM_REGS		8
#define MAX_CMD_LEN		41

extern bool monExit;
extern char prompt[];

extern token_t* eval(char *input);

extern void setReg(char reg, token_t* t);
extern token_t* getReg(char reg);

extern char* formatNum(int n);

#endif
