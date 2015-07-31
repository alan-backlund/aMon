/**
 * \file token.h
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

#if !defined(_TOKEN_POOL)
#define _TOKEN_POOL

#include <stdbool.h>

#define MAX_STRING 32

/**
 * \brief Type of token
 */
typedef enum {
	ERR=0,  /**< Error token, no value. */
	STR,   /**< String token, value is string. */
	NUM,   /**< Number token, value is number. */
#ifdef INCL_REG
	REG,   /**< Register token, value is register name. */
	GET,   /**< Get token, value is register name. */
#endif
	EXE,   /**< Execute token, no value. */
	EMPTY,   /**< Empty token, no value. */
	END,   /**< End of input token, no value. */
#ifdef INCL_EXIT
	EXIT  /**< Exit token, no value. */
#endif
} tokenType_t;

/**
 * \brief Token type
 */
typedef struct {
	tokenType_t t;  //!< type of token
	union {
		char s[MAX_STRING];  //!< String value
#ifdef INCL_REG
		char c;  //!< Register (name) value
#endif
		int  d;  //!< Numeric value
	} v;  //!< token value
} token_t;

extern bool outputDecimal;

extern token_t* tokenAlloc(char *owner);
extern token_t* tokenDup(token_t* token, char *owner);
extern void tokenFree(token_t* t);
extern char* tokenGetText(token_t* token);

extern void tokenDebug(char* prefix, token_t* t);
extern void tokenReport();

#endif
