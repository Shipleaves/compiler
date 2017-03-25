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
void printError(int);

void parser(int directive)
{
	printf("\nBegin parsing\n\n");
	out = fopen("log.txt", "w");
	flag = directive;

	get();
	block();
	if(strcmp(token, "periodsym") != 0)
		printf("Error: expected periodsym at EOF.");

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
				printf("Error: expected constant identifier.");

			get();
			if(strcmp(token, "eqsym") != 0)
				printf("Error: expected assignment operator.");

			get();
			if(!isNumber())
				printf("Error: expected number.");

			get();
		}while(strcmp(token, "commasym") == 0);

		if(strcmp(token, "semicolonsym") != 0)
			printf("Error: expected semicolon at end of constant declarations.");

		get();
	}

	if(strcmp(token, "intsym") == 0)
	{
		do
		{
			get();
			if(strcmp(token, "identsym") != 0)
				printf("Error: expected variable identifier.");

			get();
		}while(strcmp(token, "commasym") == 0);

		if(strcmp(token, "semicolonsym") != 0)
			printf("Error: expected semicolon at end of variable declarations.");
	}

	while(strcmp(token, "procsym") == 0)
	{
		get();
		if(strcmp(token, "identsym") != 0)
			printf("Error: expected procedure identifier.");

		get();
		if(strcmp(token, "semicolonsym") != 0)
			printf("Error: expected semicolon after procedure declaration.");

		get();
		block();
		if(strcmp(token, "semicolonsym") != 0)
			printf("Error: expected semicolon after procedure definition.");

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
			printf("Error: expected assignment operator.");

		get();
		expression();
	}
	else if(strcmp(token, "callsym") == 0)
	{
		get();
		if(strcmp(token, "identsym") != 0)
			printf("Error: expected procedure identifier.");

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
		printf("Error: expected end keyword.");

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
			printf("Error: expected relation operator.");

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
			printf("Error: expected close parentheses.");
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
