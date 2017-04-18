Austin Shipley
Isaac Ehlers
4/15/2017
COP3402 with Euripides Montagne
HW 4 Compiler Completion

This program runs flawlessly on a windows machine, but when we tried to test
it on eustis it quit running prematurely. We only ask that you are as 
understanding with this as you were with homework 3. Also, we decided to 
exclude error recovery so that our final submission compiled properly.

Instructions:
To compile the project, make sure all 4 .c files, the 3 .h files and input.txt
are in the same directory and run:
    gcc main.c lexer.c parser.c vm.c

then run the executable with optional compiler directives:
    a -l -a -v

This will output the source program as read by the lexer, the lexeme list and
token table, the generated code, and the vm execution trace to the console. All
output, except error messages, is also directed to log.txt.

We have also included example "input.txt" and corresponding "output.txt" files to 
display our program running properly. The Included input contains a program that 
utilizes nested procedures, same-name variables and if/else statements. The output 
file contains all output with the -l, -a, and -v directives used.

Finally, to demonstrate possible errors, we have included an "errorExamples.txt" file
that labels multiple errors in an intentionally error-riddled piece of code.