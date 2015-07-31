/**
 * \file commands.c
 * \brief Check and Process Commands.
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

#include "commands.h"
#include "process.h"
#include "main.h"


#if 0
#include <stdio.h>
#define DEBUG(s)	s
#else
#define DEBUG(s)
#endif

/**
 * \brief Command dispatch table element type
 */
typedef struct {
    const char* name;  //!< Command string
    token_t* (*func)(token_t**, int);  //!< Command function
    const char* args;  /*!< Expected arguments encoded as a string containing the letters c for register name (character), s for string, d for number and + for repeat last character as needed. */
    const char* doc;  //!< Description of command
} cmd_t;


#define MK_CMD(x) static token_t* cmd_ ## x (token_t**, int)
//Functions definitions
MK_CMD(prompt);
#ifdef INCL_REG
MK_CMD(get);
MK_CMD(set);
#endif
#ifdef INCL_MATH
MK_CMD(add);
MK_CMD(sub);
MK_CMD(mul);
#endif
MK_CMD(hex);
MK_CMD(echo);
MK_CMD(help);

//The dispatch table
#define CMD(func, params, help) {#func, cmd_ ## func, params, help}
static cmd_t dsp_table[] ={
    CMD(prompt, "s", "Select the prompt for input"),
#ifdef INCL_REG
    CMD(set, "cs", "Set register to string"),
    CMD(get, "c", "Display register"),
#endif
#ifdef INCL_MATH
    CMD(add, "dd", "Add two numbers"),
    CMD(sub, "dd", "Subtract two numbers"),
    CMD(mul, "dd", "Multiply two numbers"),
#endif
    CMD(hex, "", "Toggle output base"),
    CMD(echo, "s+", "Display parameter"),
    CMD(help, "", "Display this help")
};
#define CMDS	(sizeof(dsp_table) / sizeof(cmd_t))


/**
 * \brief Change command prompt
 * \param args  Array of arguments in tokens
 * \param nArgs  Number of arguments
 * \returns token (must be freed)
 */
static token_t* cmd_prompt(token_t *args[], int nArgs) {
	strncpy(prompt, tokenGetText(args[0]), 20);
	token_t* r = tokenAlloc("cmd_prompt");
	r->t = EMPTY;
    return r;
}

#ifdef INCL_REG
/**
 * \brief Set the contents of a register
 * \param args  Array of 'REG' token and value token
 * \param nArgs  Number of arguments, must be two
 * \returns EMPTY token (must be freed)
 */
static token_t* cmd_set(token_t *args[], int nArgs)
{
	setReg(args[0]->v.c, args[1]);
	token_t* r = tokenAlloc("cmd_set");
	r->t = EMPTY;
    return r;
}

/**
 * \brief Get the contents of a register
 * \param args  Array of single argument, a 'REG' token
 * \param nArgs  Number of arguments, must be one
 * \returns register contents in token (must be freed)
 */
static token_t* cmd_get(token_t *args[], int nArgs)
{
	token_t* r = getReg(args[0]->v.c);
    return tokenDup(r, "cmd_get");
}
#endif

#ifdef INCL_MATH
/**
 * \brief Add two numbers
 * \param args  Array of 'NUM' tokens
 * \param nArgs  Number of arguments, must be two
 * \returns 'NUM' token containing result (must be freed)
 */
static token_t* cmd_add(token_t *args[], int nArgs)
{
	token_t* r = tokenAlloc("cmd_add");
	r->t = NUM;
	r->v.d = args[0]->v.d + args[1]->v.d;
    return r;
}

/**
 * \brief Subtract second number from the first
 * \param args  Array of 'NUM' tokens
 * \param nArgs  Number of arguments, must be two
 * \returns 'NUM' token containing result (must be freed)
 */
static token_t* cmd_sub(token_t *args[], int nArgs)
{
	token_t* r = tokenAlloc("cmd_sub");
	r->t = NUM;
	r->v.d = args[0]->v.d - args[1]->v.d;
    return r;
}

/**
 * \brief Multiply two numbers
 * \param args  Array of 'NUM' tokens
 * \param nArgs  Number of arguments, must be two
 * \returns 'NUM' token containing result (must be freed)
 */
static token_t* cmd_mul(token_t *args[], int nArgs)
{
	token_t* r = tokenAlloc("cmd_mul");
	r->t = NUM;
	r->v.d = args[0]->v.d * args[1]->v.d;
    return r;
}
#endif

/**
 * \brief Toggle number display format between decimal and hexadecimal
 * \param args  Array of arguments in tokens, ignored
 * \param nArgs  Number of arguments, ignored
 * \returns 'EMPTY' token (must be freed)
 */
static token_t* cmd_hex(token_t *args[], int nArgs)
{
	outputDecimal = !outputDecimal;
	if (outputDecimal)
		transmitString("output decimal" EOL);
	else
		transmitString("output hexadecimal" EOL);
	token_t* r = tokenAlloc("cmd_hex");
	r->t = EMPTY;
    return r;
}

/**
 * \brief Print list of commands
 * \param args  Array of arguments in tokens, ignored
 * \param nArgs  Number of arguments, ignored
 * \returns token (must be freed)
 */
static token_t* cmd_help(token_t *args[], int nArgs)
{
    transmitString("Available Commands:" EOL);
    for (int i=0; i<CMDS; i++) {
        cmd_t cmd = dsp_table[i];
        for (int j=0; j<12-strlen(cmd.name)-strlen(cmd.args); j++)
			transmitString(" ");
        transmit((char*)cmd.name, strlen(cmd.name));
		transmitString("(");
        transmit((char*)cmd.args, strlen(cmd.args));
		transmitString(") - ");
        transmit((char*)cmd.doc, strlen(cmd.doc));
		transmitString(EOL);
    }
#ifdef INCL_EXIT
		transmitString("        exit() - Exit monitor" EOL);
#endif
	token_t* r = tokenAlloc("cmd_help");
	r->t = EMPTY;
    return r;
}

/**
 * \brief Echo arguments
 * \param args  Array of arguments in tokens
 * \param nArgs  Number of arguments
 * \returns last argument token (must be freed)
 */
static token_t* cmd_echo(token_t *args[], int nArgs)
{
	int i;
	for (i=0; i<nArgs-1; i++) {
		char *s = tokenGetText(args[i]);
		transmitString(s);
		transmitString(EOL);
	}
    return tokenDup(args[i], "cmd_echo");
}

/**
 * \brief Process command
 * \param tokens  Array of tokens, the first token is the command, the
 * remainder are the arguments
 * \param numTokens  Number of tokens
 * \returns The result as a token (must be freed)
 */
token_t* command(token_t* tokens[], int numTokens)
{
	token_t* r;
	int i;

#if 1
	DEBUG(for (int i=0; i<numTokens; i++)
			  tokenDebug("  cmd-tokens", tokens[i]);)
#endif

	if (numTokens == 0) {
		r = tokenAlloc("empty command");
		r->t = EMPTY;
		return r;
	}
	
	token_t* cmd = tokens[0];
	token_t** args = &tokens[1];
	numTokens--;
	
	DEBUG(tokenDebug("cmd", cmd);)
	DEBUG(for (int i=0; i<numTokens; i++)
			  tokenDebug("  arg", args[i]);)

	char *cmdName = tokenGetText(cmd);
	DEBUG(printf("command name: %s\n", cmdName);)
	for (i=0; i<CMDS; i++) {
		cmd_t cur = dsp_table[i];
		if (!strcmp(cmdName, cur.name)) {
			// Check arguments
			bool argsOK = true;
			int numChks = strlen(cur.args);
			// last check of '+' means any number of extra arguments
			if (cur.args[numChks-1] == '+') {
				// don't check the '+', req more args than checks
				numChks--;
				if (numTokens < numChks)
					argsOK = false;
			} else {
				// req num args match num of checks
				if (numTokens != numChks)
					argsOK = false;
			}
			if (argsOK) {
				for (int j=0; j<numChks; j++) {
					DEBUG(printf("arg %d type is %c\n", j, cur.args[j]);)
					if (cur.args[j] == 's') {
						if ((args[j]->t != STR)
#ifdef INCL_REG
						  && (args[j]->t != REG)
#endif
						  && (args[j]->t != NUM)) {
							DEBUG(printf("  arg %d not a string\n", j);)
							argsOK = false;
						}
					} else if (cur.args[j] == 'c') {
#ifdef INCL_REG
						if (args[j]->t != REG) {
#endif
							DEBUG(printf("  arg %d not a register\n", j);)
							argsOK = false;
#ifdef INCL_REG
						}
#endif
					} else if (cur.args[j] == 'd') {
						if (args[j]->t != NUM) {
							DEBUG(printf("  arg %d not a number\n", j);)
							argsOK = false;
						}
					}
				}
			}
			DEBUG(printf("numChks %d, numTokens %d\n", numChks, numTokens);)
			// Execute function?
			if (argsOK) {
				r = cur.func(args, numTokens);
			} else {
				r = tokenAlloc("command");
				r->t = ERR;
				strcpy(r->v.s, "Argument Error");
				transmitString("# Argument Error #" EOL);
			}
			break;
		}
	}
	if (i == CMDS) {
		r = tokenAlloc("command");
		r->t = ERR;
		strcpy(r->v.s, "Command Not Found");
		transmitString("# Command Not Found #" EOL);
	}

	return r;
}

