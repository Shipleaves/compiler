// Austin Shipley
// Isaac Ehlers
// 3/26/2017
// lexer.h for COP3402 System Software with Euripides Montagne HW3

#include <stdio.h>

#ifndef LEXER_H
#define LEXER_H

extern char** lexemeTable;
extern int* tokenTable;
extern FILE* out;

extern void lexer(int);

#endif
