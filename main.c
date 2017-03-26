// Austin Shipley
// 3/26/2017
// main.c for COP3402 System Software with Euripides Montagne HW3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "vm.h"

int main(int argc, char* argv[])
{
    int i;
    int lexerFlag = 0, parserFlag = 0, vmFlag = 0;

    printf("Compiling with the following directives:\n");
    for(i = 1; i < argc; i++)
    {
        printf("%s\n", argv[i]);

        if(strcmp(argv[i], "-l") == 0)
            lexerFlag = 1;
        else if(strcmp(argv[i], "-a") == 0)
            parserFlag = 1;
        else if(strcmp(argv[i], "-v") == 0)
            vmFlag = 1;
    }

    lexer(lexerFlag);
    parser(parserFlag);
    vm(vmFlag);

    return 1;
}

/*
-l : print the list of lexemes/tokens (scanner output) to the screen
-a : print the generated assembly code (parser/codegen output) to the screen
-v : print virtual machine execution trace (virtual machine output) to the screen
*/
