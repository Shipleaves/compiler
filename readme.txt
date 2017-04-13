Austin Shipley
Isaac Ehlers
3/26/2017
COP3402 with Euripides Montagne
HW 3 Parser and Code Gen

This program runs perfectly well on a windows machine, but when we tried to test
it on eustis it gave all kinds of strange errors. Eustis crashed shortly after,
so we weren't able to keep testing on eustis and are unsure of the programs
stability on linux machines.

To compile the project, make sure all 4 .c files, the 3 .h files and input.txt
are in the same directory and run:
    gcc main.c lexer.c parser.c vm.c

then run the executable with optional compiler directives:
    a -l -a -v

This will output the source program as read by the lexer, the lexeme list and
token table, the generated code, and the vm execution trace to the console. All
output, except error messages, is also directed to log.txt.
