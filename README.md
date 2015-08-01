# aMon
Another small system monitor.

This is a small command line program for minimal resource single board computers. It is intended to aid experimentation and testing of boards based of processors like Atmel's AVRs and ST's STM32s.

It features limited line editing (backspace), command history, registers, argument parsing and number formating.

Building requires:
re2c - "A tool for writing very fast and very flexible scanners." is available at 'http://re2c.org/'.
unifdef - "A utility selectively processes conditional C preprocessor #if and #ifdef directives." is available at 'http://dotat.at/prog/unifdef/'.
