// Austin Shipley
// Isaac Ehlers
// 3/26/2017
// parser.c for COP3402 System Software with Euripides Montagne HW3

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
	int reg;
}symbol;

symbol symbolTable[5000];
char** code;
int rf[5000];
char* lexeme;
int halt, token, currInstruction = 0, programCounter = 0, symbolIndex = 0, sp = 0, level = 0,  regCount= 0;

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
void expression();
void term();
void factor();
void get();
int number();
int getKind();
int findIdentifier();
void error(int);
void print(int);

void parser(int directive)
{
	int i;
	lexeme = malloc(20*sizeof(char));
	code = malloc(5000*sizeof(char*));
	for(i = 0; i < 5000; i++)
		code[i] = malloc(20*sizeof(char));


	get();
	block();

	if(token != periodsym)
		error(9);

	// Halt execution
	sprintf(code[programCounter++], "11 0 0 3");

	// Print the generated source code
	if(directive)
		print(directive);

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

			symbolTable[symbolIndex].value = atoi(lexeme);
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
	sprintf(code[programCounter++], "6 0 0 %d", sp);

	for(i = 0; i < localCount; i++)
	{
		// Put the value in a register
		sprintf(code[programCounter++], "1 %d 0 %d", i, symbolTable[symbolIndex-localCount+i].value);
		// Store the value on the stack
		sprintf(code[programCounter++], "4 %d 0 %d", i, symbolTable[symbolIndex-localCount+i].address);
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
	sprintf(code[programCounter++], "6 0 0 %d", localCount);
	localCount = 0;

	while(token == procsym)
	{
		printf("Error: procedures are not supported.\n");
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
		// Find identifier's location in the symbolTable
		symbolTableIndex = findIdentifier();
		if(symbolTableIndex < 0)
			error(11);

		// If not variable
		if(symbolTable[symbolTableIndex].kind != 2)
			error(12);

		get();
		if(token != becomessym)
			if(token == eqsym)
				error(1);
			else
				error(13);

		get();
		expression();

		// Store the result of the expression, which is in register[regCount-1], on the stack
		sprintf(code[programCounter++], "4 %d 0 %d", regCount-1, symbolTable[symbolTableIndex].address);
	}
	else if(token == callsym)
	{
		printf("Error: procedures are not supported.\n");
		get();
		if(token != identsym)
			error(14);

		// If constant or variable, we can't call those
		if(getKind() == 1 || getKind() == 2)
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
		int jumpInstruction, conditionReg;

		get();
		condition();
		conditionReg = regCount-1;

		// Leave a space for the jump instruction that executes if the condition is false
		jumpInstruction = programCounter++;

		if(token != thensym)
			error(16);

		get();
		statement();

		// We need to write in the jump instruction to skip the body of the if when the condition is false
		sprintf(code[jumpInstruction], "8 %d 0 %d", conditionReg, programCounter);
	}
	else if(token == whilesym)
	{
		int loopAddress, jumpInstruction, conditionReg;

		loopAddress = programCounter;
		get();
		condition();

		// Leave a space for the jump instruction that executes if the condition is false
		jumpInstruction = programCounter++;
		conditionReg = regCount-1;

		if(token != dosym)
			error(18);

		get();
		statement();

		// Jump back to condition
		sprintf(code[programCounter++], "7 0 0 %d", loopAddress);

		// We need to write in the jump instruction to skip the blcok when the condition is false
		sprintf(code[jumpInstruction], "8 %d 0 %d", conditionReg, programCounter);
	}
	else if(token == readsym)
	{
		int target;

		get();

		// Get the address in the symbol table of the variable
		target = findIdentifier();

		// Read the user input and store it in a register
		sprintf(code[programCounter++], "10 %d 0 2", regCount++);
		// Store the value from the register in the target
		sprintf(code[programCounter++], "4 %d 0 %d", regCount-1, symbolTable[target].address);

		get();
	}
	else if(token == writesym)
	{
		int target;

		get();

		target = findIdentifier();

		// Load the value of the target into the registers
		sprintf(code[programCounter++], "3 %d 0 %d", regCount++, symbolTable[target].address);
		// Write the value in the register to the screen
		sprintf(code[programCounter++], "9 %d 0 1", regCount-1);

		get();
	}

	return;
}

void condition()
{
	if(token == oddsym)
	{
		get();
		expression();

		// Mod register[regCount-1] by 2 and store result in same register
		sprintf(code[programCounter++], "17 %d 0 0", regCount-1);
	}
	else
	{
		int expression1Reg, expression2Reg;

		expression();
		expression1Reg = regCount-1;

		int relation = token;

		get();
		expression();
		expression2Reg = regCount-1;

		switch(relation)
		{
			// equals
			case 9:
				sprintf(code[programCounter++], "19 %d %d %d", regCount++, expression1Reg, expression2Reg);
				break;
			// not equals
			case 10:
				sprintf(code[programCounter++], "20 %d %d %d", regCount++, expression1Reg, expression2Reg);
				break;
			// less than
			case 11:
				sprintf(code[programCounter++], "21 %d %d %d", regCount++, expression1Reg, expression2Reg);
				break;
			// Less than or equals
			case 12:
				sprintf(code[programCounter++], "22 %d %d %d", regCount++, expression1Reg, expression2Reg);
				break;
			// greater than
			case 13:
				sprintf(code[programCounter++], "23 %d %d %d", regCount++, expression1Reg, expression2Reg);
				break;
			// greater than or equals
			case 14:
				sprintf(code[programCounter++], "24 %d %d %d", regCount++, expression1Reg, expression2Reg);
				break;
			// Not a valid relational operator
			default:
				error(20);
				break;
		}
	}
	return;
}

void expression()
{
	int firstTerm;

	// Unary plus and minus operators
	if(token == plussym || token == minussym)
	{
		// TODO: what is a unary plus supposed to do?
		if(token == plussym)
		{
			get();
			term();
		}
		else if(token == minussym)
		{
			get();
			term();
			sprintf(code[programCounter++], "12 %d %d 0", regCount-1, regCount-1);
		}
	}
	else
		term();

	while(token == plussym || token == minussym)
	{
		if(token == plussym)
		{
			get();
			term();
			sprintf(code[programCounter++], "13 %d %d %d", regCount-2, regCount-2, regCount-1);
			regCount--;
		}
		else if(token == minussym)
		{
			get();
			term();
			sprintf(code[programCounter++], "14 %d %d %d", regCount-2, regCount-2, regCount-1);
			regCount--;
		}
		term();
	}
	return;
}

void term()
{
	factor();
	while(token == multsym || token == slashsym)
	{
		if(token == multsym)
		{
			get();
			factor();
			sprintf(code[programCounter++], "15 %d %d %d", regCount-2, regCount-2, regCount-1);
			regCount--;
		}
		else if(token == slashsym)
		{
			get();
			factor();
			sprintf(code[programCounter++], "16 %d %d %d", regCount-2, regCount-2, regCount-1);
			regCount--;
		}
	}

	return;
}

void factor()
{
	int value;

	if(token == identsym)
	{
		value = findIdentifier();
		if(value < 0)
			error(11);

		sprintf(code[programCounter++], "3 %d 0 %d", regCount, symbolTable[value].address);
		regCount++;
		get();
	}
	else if(token == numbersym)
	{
		if(!number())
			error(2);

		value = atoi(lexeme);
		sprintf(code[programCounter++], "1 %d 0 %d", regCount, value);
		regCount++;
		get();
	}
	else if(token == lparentsym)
	{
		get();
		expression();
		if(token != rparentsym)
			error(22);
		get();
	}
	else
	{
		// TODO: some error code goes here.
	}

	return;
}

void get()
{
	strcpy(lexeme, lexemeTable[currInstruction]);
	token = tokenTable[currInstruction];
	currInstruction++;

	return;
}

// Poorly named because isNumber() is already used in lexer.c
int number()
{
	int i, length = strlen(lexeme);

	for(i = 0; i < length; i++)
	{
		if(lexeme[i] < 48 || lexeme[i] > 57)
			return 0;
	}

	return 1;
}

int getKind()
{
	int i;

	for(i = 0; i < symbolIndex; i++)
	{
		if(strcmp(lexeme, symbolTable[i].name) == 0)
			return symbolTable[i].kind;
	}

	return 0;
}

int findIdentifier()
{
	int i;

	for(i = 0; i < symbolIndex; i++)
	{
		if(strcmp(lexeme, symbolTable[i].name) == 0)
			return i;
	}

	return -1;
}

void error(int errorCode)
{
	// Prevents VM from executing code
	halt = 1;

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
			break;
		case 9:
			printf("Error: expected period.\n");
			break;
		case 10:
			// TODO: unused error code.
			printf("Error: semicolon between statements missing\n");
			break;
		case 11:
			printf("Error: undeclared identifier.\n");
			break;
		case 12:
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
			printf("This number is too large.\n");
			break;
	}
}

void print(int directive)
{
	fprintf(out, "\n\nGenerated Code:\n");
	if(directive)
		printf("\n\nGenerated Code:\n");
	int i;
	for(i = 0; i < programCounter; i++)
	{
		fprintf(out, "%d: %s\n", i, code[i]);
		if(directive)
			printf("%d: %s\n", i, code[i]);
	}
	if(!halt)
	{
		fprintf(out, "\nThe program is syntactically correct.\n");
		if(directive)
			printf("\nThe program is syntactically correct.\n");
	}
}
