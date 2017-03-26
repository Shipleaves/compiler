#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

typedef struct
{
	int kind; 		// const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int value; 		// number
	int level; 		// L level
	int address; 		// M address
}symbol;

symbol symbolTable[5000];
int rf[5000];
char* lexeme;
int token, programCounter = 0, symbolIndex = 0, sp = 0, level = 0,  regCount= 0;
FILE* out;

int nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6, slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11,
leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26,
callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33;

void parser(int);
void block();
void statement();
void condition();
int expression();
void term();
void factor();
void get();
int isRelation();
int number();
int isVariable();
int isConstant();
int findIdentifier();
int stringToInt(char*);
void print();
void error(int);

void parser(int directive)
{
	printf("\nBegin parsing\n\n");
	out = fopen("instructions.txt", "w");
	lexeme = malloc(20*sizeof(char));

	get();
	block();
	if(token != periodsym)
		error(9);

	if(directive)
		print();

	free(lexeme);
	return;
}

void block()
{
	// Count of local variables / constants
	int localCount = 0, i;
	sp += 4;

	if(token == constsym)
	{
		do
		{
			get();
			if(token != identsym)
				error(4);

			strcpy(symbolTable[symbolIndex].name, lexeme);
			symbolTable[symbolIndex].kind = 1;

			get();
			if(token != eqsym)
				error(3);

			get();
			if(!number())
				error(2);

			symbolTable[symbolIndex].value = stringToInt(lexeme);
			symbolTable[symbolIndex].address = sp;
			symbolIndex++;
			localCount++;
			sp++;

			get();
		}while(token == commasym);

		if(token != semicolonsym)
			error(5);

		get();
	}

	// Allocate space on the stack
	fprintf(out, "6 0 0 %d", sp);

	for(i = 0; i < localCount; i++)
	{
		// Put the value in a register
		fprintf(out, "1 %d 0 %d", i, symbolTable[symbolIndex-localCount+i].value);
		// Store the value on the stack
		fprintf(out, "3 %d 0 %d", i, symbolTable[symbolIndex-localCount+i].address);
	}
	localCount = 0;

	if(token == varsym)
	{
		do
		{
			get();
			if(token != identsym)
				error(4);

				strcpy(symbolTable[symbolIndex].name, lexeme);
				symbolTable[symbolIndex].kind = 2;
				symbolTable[symbolIndex].address = sp;
				localCount++;
				sp++;

			get();
		}while(token == commasym);

		if(token != semicolonsym)
			error(5);
	}

	// Allocate space on the stack
	fprintf(out, "6 0 0 %d", localCount);
	localCount = 0;

	while(token == procsym)
	{
		printf("Error: Procedures are note supported.\n");
		get();
		if(token != identsym)
			error(4);

		get();
		if(token != semicolonsym)
			error(5);

		get();
		block();
		if(token != semicolonsym)
			error(5);

		get();
	}
	// TODO: Error code 7 here? Statement expected.
	statement();
}

void statement()
{
	int symbolTableIndex;

	if(token == identsym)
	{
		symbolTableIndex = findIdentifier();
		if(symbolTableIndex < 0)
			error(11);

		get();
		if(token != becomessym)
			if(token == eqsym)
				error(1);
			else
				error(13);

		get();
		expression();
	}
	else if(token == callsym)
	{
		printf("Error: procedures are not supported.\n");
		get();
		if(token != identsym)
			error(14);

		if(isConstant() || isVariable())
				error(15);

		get();
	}
	else if(token == beginsym)
	{
		// TODO: Error code 7 here? Statement expected.
		get();
		statement();
		while(token == semicolonsym)
		{
			get();
			statement();
		}
		// TODO: Error code 10 here? Missing semicolon between statements.
		if(token != endsym)
			error(8);
		get();
	}
	else if(token == ifsym)
	{
		get();
		condition();
		if(token != thensym)
			error(16);

		get();
		statement();
	}
	else if(token == whilesym)
	{
		get();
		condition();
		if(token != dosym)
			error(18);

		get();
		statement();
	}

	return;
}

void condition()
{
	if(token != oddsym)
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

int expression()
{
	if(token == plussym || token == minussym)
		get();

	term();
	while(token == plussym || token == minussym)
	{
		get();
		term();
	}
	return;
}

void term()
{
	factor();
	while(token == multsym || token == slashsym)
	{
		get();
		factor();
	}
	return;
}

void factor()
{
	if(token == varsym || token == constsym)
		get();
	else if(number())
		get();
	else if(token == rparentsym)
	{
		get();
		expression();
		if(token != lparentsym)
			error(22);
		get();
	}
	printf("Error:");
}

void get()
{
	strcpy(lexeme, lexemeTable[programCounter]);
	token = tokenTable[programCounter];
	programCounter++;

	printf("%s \t %d\n", lexeme, token);
	return;
}

int isRelation()
{
	return 1;
}

// Poorly named because isNumber() is already used in lexer.c
int number()
{
	return 1;
}

int isConstant()
{
	return 1;
}

int isVariable()
{
	return 1;
}

int findIdentifier()
{
	return 1;
}

int stringToInt(char* number)
{
	return 12;
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
			printf("Error: then expected.\n");
			break;
		case 17:
			// TODO: unused error code.
			printf("Error: expected semicolon or }.\n");
			break;
		case 18:
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

void print()
{

}
