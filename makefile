# Makefile for aMon
#
# 2015 Jul 29 Alan Backlund
#
# The MIT License (MIT)
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# INCL_REG and INCL_EXIT flags must be undef'd (-U) to rm registers/exit
FLAGS := -DINCL_REG -DINCL_EXIT

CFLAGS := -std=c99 -O0 -g3 -Wall -c -fmessage-length=0 -DBIG -DINCL_MATH $(FLAGS)

aMon: main.o lexer.o token.o process.o commands.o monitor.o print.o
	gcc -o aMon main.o lexer.o token.o process.o commands.o monitor.o print.o

main.o: main.c main.h process.h
	gcc $(CFLAGS) -o main.o main.c

lexer.o: lexer.re2c lexer.h token.h
	unifdef $(FLAGS) -x1 -t -o lexer.tre2c lexer.re2c
	re2c -is -c -o lexer.c lexer.tre2c
	gcc $(CFLAGS) -o lexer.o lexer.c

process.o: process.c lexer.h process.h token.h
	gcc $(CFLAGS) -o process.o process.c

commands.o: commands.c process.h token.h
	gcc $(CFLAGS) -o commands.o commands.c

token.o: token.c token.h
	gcc $(CFLAGS) -o token.o token.c

monitor.o: monitor.c monitor.h
	gcc $(CFLAGS) -o monitor.o monitor.c

print.o: print.c print.h
	gcc $(CFLAGS) -o print.o print.c

.PHONY: clean
clean:
	rm aMon *.o lexer.c lexer.tre2c output*

.PHONY: test
test:
	./aMon < testfiles/test1 > testfiles/output1
	diff testfiles/expect1 testfiles/output1
	./aMon < testfiles/test2 > testfiles/output2
	diff testfiles/expect2 testfiles/output2
	./aMon < testfiles/test3 > testfiles/output3
	diff testfiles/expect3 testfiles/output3

.PHONY: doc
doc:
	doxygen doxygen.conf
