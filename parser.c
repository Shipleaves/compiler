#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

typedef struct
{
	int kind; 		// const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		// number
	int level; 		// L level
	int addr; 		// M address
}symbol;

symbol symbolTable[5000];
char* token;
int instructionPointer = 0, symbolIndex = 0, sp = 0; flag = 0, level = 0;  regCount= 0;
FILE* out;

void parser(int flag);
void block();
void statement();
void condition();
void expression();
void term();
void codeGen();
void get();
int isRelation();
void error(int);

void parser(int directive)
{
	printf("\nBegin parsing\n\n");
	out = fopen("instructions.txt", "w");
	flag = directive;

	get();
	block();
	if(strcmp(token, "periodsym") != 0)
		error(9);

	return;
}

void block()
{
	if(strcmp(token, "constsym") == 0)
	{
		do
		{
			get();
			if(strcmp(token, "identsym") != 0)
				error(4);

			strcpy(symbolTable[symbolIndex].name, token);
			symbolTable[symbolIndex].kind = 1;

			get();
			if(strcmp(token, "eqsym") != 0)
				error(3);

			get();
			if(!isNumber())
				error(2);

			sp++;
			symbolTable[symbolIndex].value = stringToInt(token);
			symbolTable[symbolIndex].address = sp;

			get();
		}while(strcmp(token, "commasym") == 0);

		if(strcmp(token, "semicolonsym") != 0)
			error(5);

		get();
	}

	if(strcmp(token, "varsym") == 0)
	{
		do
		{
			get();
			if(strcmp(token, "identsym") != 0)
				error(4);

			get();
		}while(strcmp(token, "commasym") == 0);

		if(strcmp(token, "semicolonsym") != 0)
			error(5);

		localCount++;
	}

	// This is how we do code generation.
	fprintf("6 0 0 %d", localCount);

	while(strcmp(token, "procsym") == 0)
	{
		get();
		if(strcmp(token, "identsym") != 0)
			erorr(4);

		get();
		if(strcmp(token, "semicolonsym") != 0)
			error(5);

		get();
		block();
		if(strcmp(token, "semicolonsym") != 0)
			error(5);

		get();
	}
	// TODO: Error code 7 here? Statement expected.
	statement();
}

void statement()
{
	if(strcmp(token, "identsym") == 0)
	{
		get();
		if(strcmp(token, "beceomessym") != 0)
			if(strcmp(token, "eqsym") == 0)
				error(1);
			else
				error(13);

		get();
		expression();
	}
	else if(strcmp(token, "callsym") == 0)
	{
		get();
		if(strcmp(token, "identsym") != 0)
			error(14);

		if(isConstant() || isVariable())
				error(15);

		get();
	}
	else if(strcmp(token, "beginsym") == 0)
	{
		// TODO: Error code 7 here? Statement expected.
		get();
		statement();
		while(strcmp(token, "semicolonsym") == 0)
		{
			get();
			statement();
		}
		// TODO: Error code 10 here? Missing semicolon between statements.
	}

	if(strcmp(token, "endsym") != 0)
		error(8);

	get();

	return;
}

void condition()
{
	if(strcmp(token, "oddsym") != 0)
	{
		get();
		expression();
	}
	else
	{
		expression();
		if(!isRelation())
			error(20);

		get();
		expression();
	}
	return;
}

void expression()
{
	if(strcmp(token, "plussym") == 0 || strcmp(token, "minussym") == 0)
		get();

	term();
	while(strcmp(token, "plussym") == 0 || strcmp(token, "minussym") == 0)
	{
		get();
		term();
	}
	return;
}

void term()
{
	factor();
	while(strcmp(token, "multsym") == 0 || strcmp(token, "slashsym") ==0)
	{
		get();
		factor();
	}
	return;
}

void factor()
{
	if(strcmp(token, "identsym") == 0)
		get();
	else if(isNumber())
		get();
	else if(strcmp(token, "(") == 0)
	{
		get();
		expression();
		if(strcmp(token, ")") != 0)
			error(22);
		get();
	}
	printf("Error:");
}

void get()
{
	strcpy(token, lexemeTable[instructionPointer++]);
	return;
}

int isRelation()
{
	return 1;
}

void error(int errorCode)
{
	switch(errorCode)
	{
		case 1:
			printf("Error: found = when expecting :=\n");
			break;
		case 2:
			printf("Error: = must be followed by a number.\n");
			break;
		case 3:
			printf("Error: identifier must be followed by =.\n");
			break;
		case 4:
			printf("Error: const, var, procedure must be followed by identifier.\n");
			break;
		case 5:
			printf("Error: expected semicolon or comma.\n");
			break;
		case 6:
			// Incorrect symbol after procedure declaration
			// Procedures are not implemeneted yet
			break;
		case 7:
			// TODO: unused error code.
			printf("Error: expected statement.\n");
			break;
		case 8:
			printf("Error: incorrect symbol after statement part in block.\n");
		case 9:
			printf("Error: expected period.\n");
			break;
		case 10:
			// TODO: unused error code.
			printf("Error: semicolon between statements missing\n");
			break;
		case 11:
			// TODO: unused error code.
			printf("Error: undeclared identifier.\n");
			break;
		case 12:
			// TODO: unused error code.
			printf("Error: assignment to constant or procedure is not allowed.\n");
			break;
		case 13:
			printf("Error: expected assignment operator.\n");
			break;
		case 14:
			printf("Error: call must be followed by an identifier.\n");
			break;
		case 15:
			printf("Error: call of a constant or variable is meaningless\n");
			break;
		case 16:
			// TODO: unused error code.
			printf("Error: then expected.\n");
			break;
		case 17:
			// TODO: unused error code.
			printf("Error: expected semicolon or }.\n");
			break;
		case 18:
			// TODO: unused error code.
			printf("Error: expected do.\n");
			break;
		case 19:
			// TODO: what does this even mean?
			printf("Error: incorrect symbol following statement.\n");
			break;
		case 20:
			printf("Error: expected relational operator.\n");
			break;
		case 21:
			// TODO: unused error code.
			printf("Error: expression must not contain a procedure identifier.\n");
			break;
		case 22:
			printf("Error: expected close parenthesis.\n");
			break;
		case 23:
			// TODO: unused error code.
			printf("Error: the preceding factor cannot begin with this symbol.\n");
			break;
		case 24:
			// TODO: unused error code.
			printf("Error: an expression cannot begin with this symbol.\n");
			break;
		case 25:
			// TODO: unused error code.
			printf("This number is too larger.\n");
			break;
	}
}
