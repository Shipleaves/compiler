#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

char* token;
int idx = 0, flag= 0, regCount= 0, ;
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
	out = fopen("log.txt", "w");
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

			get();
			if(strcmp(token, "eqsym") != 0)
				error(13);

			get();
			if(!isNumber())
				error(2);

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
	}

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
		get();
		statement();
		while(strcmp(token, "semicolonsym"))
		{
			get();
			statement();
		}
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
	strcpy(token, lexemeTable[idx++]);
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
			printf("Error: found = when expecting :=");
			break;
		case 2:
			printf("Error: = must be followed by a number.");
			break;
		case 4:
			printf("Error: const, var, procedure must be followed by identifier.");
			break;
		case 5:
			printf("Error: expected semicolon or comma.");
			break;
		case 8:
			printf("Error: incorrect symbol after statement part in block.");
		case 9:
			printf("Error: expected period.");
			break;
		case 13:
			printf("Error: expected assignment operator.");
			break;
		case 14:
			printf("Error: call must be followed by an identifier.");
			break;
		case 15:
			printf("Error: call of a constant or variable is meaningless");
			break;
		case 20:
			printf("Error: expected relation operator.");
			break;
		case 22:
			printf("Error: expected close parenthesis.");
			break;
	}
}
