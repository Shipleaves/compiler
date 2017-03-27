// Austin Shipley
// Isaac Ehlers
// 3/26/2017
// lexer.c for COP3402 System Software with Euripides Montagne HW3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

typedef struct
{
	int kind; 		// const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
}symbol;

char** lexemeTable;
int* tokenTable;
FILE* out;
int flag, idx = 0;

void lexer(int flag);
int isWhitespace(int ch);
int isLetter(int ch);
int isKeyword(char* word);
int isNumber(int ch);
int isSymbol(int ch);
void addIdentifier(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount);
void addKeyword(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount, int* level);
void addNumber(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount);
int addSymbol(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount);
void handleComments(char code[], int count);

void lexer(int directive)
{
    symbol symbolTable[5000];
    char code[5000];
    char* lexeme;
    int ch, level = 0, tokenCount = 0, length = 0, count = 0, endSymbol = 0;
    FILE* in;
	in = fopen("input.txt", "r");
	out = fopen("log.txt", "w");
	flag = directive;

    if(in == NULL)
    {
        printf("Error: File pointer failure.\n");
        return;
    }

	tokenTable = malloc(5000*sizeof(int));
	lexemeTable = malloc(5000*sizeof(char*));
    for(count = 0; count < 5000; count++)
        lexemeTable[count] = malloc(100*sizeof(char));

	if(tokenTable == NULL || lexemeTable == NULL)
	{
		printf("Memory allocation failure.");
		return;
	}

    count = 0;

    // Print out the source code and put it into the code array
    fprintf(out, "Source Program:\n");
	if(flag)
		printf("Source Program:\n");
    while(1)
    {
		ch = fgetc(in);
		if(feof(in))
			break;

        fprintf(out, "%c", ch);
		if(flag)
			printf("%c", ch);

        code[count] = ch;
        count++;
    }

	code[count] = '\0';
    fclose(in);

    // Begin scanning the source code
    while(idx < count-1)
    {
        ch = code[idx];

        // Ignore whitespace
        while(isWhitespace(ch) && idx < count)
        {
            ch = code[++idx];
        }

        lexeme = (char*)malloc(50*sizeof(char));
        if(lexeme == NULL)
        {
            printf("Error: Memory allocation failure\n");
            break;
        }
		strcpy(lexeme, "\0");

        if(isLetter(ch))
        {
            while(!isWhitespace(ch) && !isSymbol(ch) && sizeof(lexeme)/sizeof(char) <= 11)
            {
                // Read and append the char to lexeme
                length = strlen(lexeme);
                lexeme[length] = ch;
                lexeme[length+1] = '\0';
                ch = code[++idx];
				// Sometimes some kind of undetectable character gets appended
				// and isKeyword doesn't proc on line 111, so we double check here
				//if(isKeyword(lexeme) > 0 && !isLetter(ch))
				//	goto KEYWORD;
            }

            if(strlen(lexeme) > 11)
                printf("Error: Identifier longer than 11 letters\n");

            if(isKeyword(lexeme) > 0)
                addKeyword(lexeme, lexemeTable, tokenTable, &tokenCount, &level);
            else
                addIdentifier(lexeme, lexemeTable, tokenTable, &tokenCount);

			// Delete the lexeme so it doesn't persist into the next block
            strcpy(lexeme, "\0");
        }

        if(isNumber(ch))
        {
            while(!isWhitespace(ch) && !isSymbol(ch) && !isLetter(ch))
            {
                length = strlen(lexeme);
                lexeme[length] = ch;
                lexeme[length+1] = '\0';
                ch = code[++idx];
            }

            if(isLetter(ch))
            {
                printf("Error: identifier starts with number\n");
            }

            if(strlen(lexeme) > 5)
            {
                printf("Error: Number greater than 5 digits\n");
            }

            addNumber(lexeme, lexemeTable, tokenTable, &tokenCount);

			// Delete the lexeme so it doesn't persist into the next block
            strcpy(lexeme, "\0");
        }

        if(isSymbol(ch))
        {
            while(!isWhitespace(ch) && !isLetter(ch) && !isNumber(ch) && idx < count)
            {
                length = strlen(lexeme);
                lexeme[length] = ch;
                lexeme[length+1] = '\0';
				ch = code[++idx];
				// God damn parentheses
				if(ch == 40 || lexeme[0] == 40)
					break;
				if(ch == 41 || lexeme[0] == 41)
					break;
				if(lexeme[0] == '/' && ch == '*')
					handleComments(code, count);
            }

            if(strcmp(lexeme, "/*") == 0)
                handleComments(code, count);
            else
                addSymbol(lexeme, lexemeTable, tokenTable, &tokenCount);
        }

        free(lexeme);
    }

    fprintf(out, "\n\nLexeme Table:\nlexeme\t\ttoken type\n");
	if(flag)
		printf("\n\nLexeme Table:\nlexeme\t\ttoken type\n");
    for(count = 0; count < tokenCount; count++)
    {
        if(strlen(lexemeTable[count]) > 8)
		{
            fprintf(out, "%s\t%d\n", lexemeTable[count], tokenTable[count]);
			if(flag)
				printf("%s\t%d\n", lexemeTable[count], tokenTable[count]);
		}
		else
		{
			fprintf(out, "%s\t\t%d\n", lexemeTable[count], tokenTable[count]);
			if(flag)
				printf("%s\t\t%d\n", lexemeTable[count], tokenTable[count]);
		}
    }

    return;
}

int isWhitespace(int ch)
{
    if(ch == 32 || ch == 9 || ch == 10)
        return 1;

    return 0;
}

int isLetter(int ch)
{
    if((ch >= 65 && ch <=90) || (ch >= 97 && ch <= 122))
        return 1;

    return 0;
}

int isNumber(int ch)
{
    if(ch >= 48 && ch <=57)
        return 1;

    return 0;
}

int isSymbol(int ch)
{
    if((ch >= 40 && ch <=47) || (ch >= 58 && ch <= 62) )
        return 1;

    return 0;
}

int isSingleSymbol(int ch)
{
    if(ch)

    return 0;
}

int isKeyword(char* word)
{
    if(strcmp(word, "const") == 0)
        return 28;
	else if(strcmp(word, "var") == 0)
		return 29;
	else if(strcmp(word, "procedure") == 0)
    	return 30;
	else if(strcmp(word, "call") == 0)
    	return 27;
	else if(strcmp(word, "begin") == 0)
    	return 21;
	else if(strcmp(word, "end") == 0)
    	return 22;
	else if(strcmp(word, "if") == 0)
    	return 23;
	else if(strcmp(word, "then") == 0)
    	return 24;
	else if(strcmp(word, "else") == 0)
    	return 33;
	else if(strcmp(word, "while") == 0)
    	return 25;
	else if(strcmp(word, "do") == 0)
    	return 26;
	else if(strcmp(word, "read") == 0)
    	return 32;
	else if(strcmp(word, "write") == 0)
        return 31;
    else if(strcmp(word, "odd") == 0)
        return 8;

    return 0;
}

void addKeyword(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount, int* level)
{
    int length;
    // Add the word and its symbol to the lexeme table
    // Can't use switch for strings, RIP
    if(strcmp(lexeme, "const") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 28;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "var") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 29;
        *tokenCount += 1;
    }
    // If we encounter procedure, we have moved up a lexicographic level
    else if(strcmp(lexeme, "procedure") == 0)
    {
        *level += 1;
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 30;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "call") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 27;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "begin") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 21;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "end") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 22;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "if") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 23;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "then") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 24;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "else") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 33;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "while") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 25;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "do") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 26;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "read") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 32;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "write") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 31;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "odd") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 8;
        *tokenCount += 1;
    }
}

void addIdentifier(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount)
{
    if(strlen(lexeme) < 1)
        return;

    strcpy(lexemeTable[*tokenCount], lexeme);
    tokenTable[*tokenCount] = 2;
    *tokenCount += 1;
}

void addNumber(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount)
{
    strcpy(lexemeTable[*tokenCount], lexeme);
    tokenTable[*tokenCount] = 3;
    *tokenCount += 1;
}

int addSymbol(char* lexeme, char* lexemeTable[], int tokenTable[], int* tokenCount)
{
    int ch, length;

    if(strcmp(lexeme, "+") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 4;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "-") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 5;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "*") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 6;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "/") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 7;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "(") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 15;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, ")") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 16;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "=") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 9;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, ",") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 17;
        *tokenCount += 1;
    }
	else if(strcmp(lexeme, ".") == 0)
	{
		strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 19;
        *tokenCount += 1;
	}
    else if(strcmp(lexeme, "<") == 0)
    {
            strcpy(lexemeTable[*tokenCount], lexeme);
            tokenTable[*tokenCount] = 11;
            *tokenCount += 1;
    }
    else if(strcmp(lexeme, "<>") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 10;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, "<=") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 12;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, ">") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 13;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, ">=") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 14;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, ";") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 18;
        *tokenCount += 1;
    }
    else if(strcmp(lexeme, ":=") == 0)
    {
        strcpy(lexemeTable[*tokenCount], lexeme);
        tokenTable[*tokenCount] = 20;
        *tokenCount += 1;
    }

    return 1;
}

void handleComments(char code[], int count)
{
    int ch;

    // Read until end of comment
    while(1)
    {
    	ch = code[++idx];
        // So we don't accidentally get stuck in here forever
        if(idx >= count)
        {
            printf("Error: EOF unexpectedly encountered\n");
            return;
        }

        // Char is *
        if(ch == '*')
        {
            ch = code[idx+1];
            // Char is /
            if(ch == '/')
            {
                // End of comment
                return;
            }
        }
    }
}
