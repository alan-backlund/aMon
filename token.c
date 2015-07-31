/**
 * \file token.c
 * \brief Allocate tokens from a static pool.
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
#include "main.h"
#include "print.h"

#if BIG
#include <stdio.h>
#endif

bool outputDecimal = true;

#define MAX_TOKENS	20
static token_t pool[MAX_TOKENS];
static bool inUse[MAX_TOKENS];
static char* owners[MAX_TOKENS];

/**
 * Allocate token.
 * \param owner  Name of allocator (for debugging).
 * \returns  Fresh token.
 */
token_t* tokenAlloc(char *owner)
{
	for (int i=0; i<MAX_TOKENS; i++) {
		if (!inUse[i]) {
			inUse[i] = true;
			owners[i] = owner;
			return &pool[i];
		}
	}
	transmitString("# Token pool empty #" EOL);
	return NULL;
}

/**
 * Allocate a duplicate of an existing token.
 * \param token  Existing token.
 * \param owner  Name of allocator (for debugging).
 * \returns  Copy of existing token.
 */
token_t* tokenDup(token_t* token, char *owner)
{
	token_t* newToken = tokenAlloc(owner);
	memcpy(newToken, token, sizeof(token_t));
	return newToken;
}

/**
 * Free token
 * \param t  Token to be freed.
 */
void tokenFree(token_t* t)
{
	for (int i=0; i<MAX_TOKENS; i++)
		if (t == &pool[i]) {
#if BIG
			if (!inUse[i])
				printf("# free token %d freed#\n", i);
#endif
			inUse[i] = false;
		}
}

static char outBuf[2];

/**
 * Get string describing token contents
 * \param token  The token to describe.
 * \returns String describing token contents
 */
char* tokenGetText(token_t* token)
{
	if (token->t == STR)
		return token->v.s;
	if (token->t == NUM)
		return formatNum(token->v.d, outputDecimal);
#ifdef INCL_REG
	if (token->t == REG) {
		outBuf[0] = token->v.c;
		outBuf[1] = 0;
	} else
#endif
		outBuf[0] = 0;
	return outBuf;
}


#if BIG
/**
 * Print detailed description of token
 * \param prefix  Prefix for description.
 * \param t  Token
 */
void tokenDebug(char* prefix, token_t* t)
{
	switch (t->t) {
	case ERR: printf("%s  ERR: %s\n", prefix, t->v.s); break;
	case STR: printf("%s  STR: \"%s\"\n", prefix, t->v.s); break;
	case NUM: printf("%s  NUM: %d\n", prefix, t->v.d); break;
#ifdef INCL_REG
	case REG: printf("%s  REG: %c\n", prefix, t->v.c); break;
	case GET: printf("%s  GET: %c\n", prefix, t->v.c); break;
#endif
	case EXE: printf("%s  EXE\n", prefix); break;
	case EMPTY: printf("%s  EMPTY\n", prefix); break;
	case END: printf("%s  END\n", prefix); break;
#ifdef INCL_EXIT
	case EXIT: printf("%s  EXIT\n", prefix); break;
#endif
	}
}

/**
 * Print report on token usage.
 */
void tokenReport()
{
	int cnt = 0;
	for (int i=0; i<MAX_TOKENS; i++) {
		if (inUse[i]) {
			printf("token %d in use by \"%s\"", i, owners[i]);
			tokenDebug("", &pool[i]);
			cnt++;
		}
	}
	printf("tokenReport: %d tokens in use.\n", cnt);
}
#endif
