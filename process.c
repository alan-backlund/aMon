/**
 * \file process.c
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

#include <stdbool.h>
#include <string.h>

#include "process.h"
#include "lexer.h"
#include "token.h"
#include "commands.h"
#include "main.h"

#if 0
#include <stdio.h>
#define DEBUG(s)	s
#else
#define DEBUG(s)
#endif

#ifdef INCL_REG
static token_t* regs[NUM_REGS];
static token_t emptyReg = { EMPTY, { "" } };

/**
 * Set register
 * \param reg  Register name ('a' to 'h')
 * \param t    Token to be copied into register
 */
void setReg(char reg, token_t* t)
{
	int regNum = reg - 'a';
	if ((regNum >= 0) && (regNum < NUM_REGS)) {
		if (regs[regNum] != NULL)
			tokenFree(regs[regNum]);
		regs[regNum] = tokenDup(t, "setReg");
	}
}

/**
 * Get register
 * \param reg  Register name ('a' to 'h')
 * \returns token *DO NOT FREE*
 */
token_t* getReg(char reg)
{
	int regNum = reg - 'a';
	if ((regNum < 0) || (regNum >= NUM_REGS)) {
		transmitString("# Register '");
		transmit(&reg, 1);
		transmitString("' out of range #" EOL);
		return &emptyReg;
	}
	if (regs[regNum] == NULL) {
		transmitString("# Register '");
		transmit(&reg, 1);
		transmitString("' undefined #" EOL);
		return &emptyReg;
	}
	return regs[regNum];
}
#endif

/**
 * Evaluate Command
 * \param input  String containing command
 * \return token Result of evaluation (STR, NUM, EMPTY) *MUST BE FREED*
 */
token_t* eval(char *input)
{
	int numTokens;
	token_t* tokens[MAX_ARGS];
	token_t* result;
	int exeCount = 0;

	DEBUG(printf("eval \"%s\" begin\n", input);)
	lexerStart(input);
	token_t* token;
	numTokens = 0;
	do {
		token = lexer();
		DEBUG(tokenDebug("lexed", token);)
#ifdef INCL_EXIT
		if (token->t == EXIT) {
			for (int i=0; i<numTokens; i++)
				tokenFree(tokens[i]);
			monExit = true;
			result = NULL;
			break;
		} else
#endif
		if (token->t == END) {
			tokenFree(token);
			// process command
			result = command(&tokens[0], numTokens);
			// free tokens
			for (int i=0; i<numTokens; i++) {
				DEBUG(tokenDebug("free", tokens[i]);)
				tokenFree(tokens[i]);
			}
			break;
#ifdef INCL_REG
		} else if (token->t == GET) {
			token_t* newToken = getReg(token->v.c);
			if (newToken != NULL) {
				tokenFree(token);
				token = tokenDup(newToken, "eval");
				DEBUG(tokenDebug("  got", token);)
			} else
				token->t = REG;
#endif
		} else if (token->t == EXE) {
			DEBUG(tokenDebug("  exe", token);)
			exeCount = 2;
			tokenFree(token);
		}
		if (exeCount == 2) {
			// Do not save EXE token
			exeCount--;
		} else if (exeCount == 1) {
			exeCount--;
			// If the next token is a string
			if (token->t == STR) {
				token_t* old = token;
				token = eval(token->v.s);
				if (token->t != EMPTY)
					tokens[numTokens++] = token;
				else
					tokenFree(token);
				tokenFree(old);
			} else {
				if (token->t != EMPTY)
					tokens[numTokens++] = token;
				else
					tokenFree(token);
			}
		} else {
			if (token->t != EMPTY)
				tokens[numTokens++] = token;
			else
				tokenFree(token);
		}
	} while (true);
	lexerClose();
	DEBUG(tokenDebug("eval end", token);)
	return result;
}
