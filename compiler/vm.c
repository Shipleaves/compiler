// Austin Shipley
// Isaac Ehlers
// 3/26/2017
// vm.c for COP3402 System Software with Euripides Montagne HW3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "vm.h"
#include "parser.h"

typedef struct
{
    int op;
    int r;
    int l;
    int m;
} instruction;

// This struct array simulates the Instruction Register (or rather,
// each index is a state of the IR at a particular time)
instruction inst[5000];
int flag;

void vm(int);
void execute(FILE *out, char **opcode, instruction *inst, int numInst);
int stoBase(int *stack, int l, int base);
int base(int *stack, int l, int base);
instruction getInst(char *codeString);

void vm(int directive)
{

    if(halt)
        return;

    // An array for easy access to opcodes
    // The 0th entry is blank to make the rest 1-based
    char *opcode[25] = { "",
        "lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc",
        "sio", "sio", "sio", "neg", "add", "sub", "mul", "div",
        "odd", "mod", "eql", "neq", "lss", "leq", "gtr", "geq"
    };
    flag = directive;

    // Print the header
    fprintf(out, "\n\nVM Execution Trace\n");
    if(flag)
        printf("\n\nVM Execution Trace\n");

    fprintf(out, "Line	OP	R	L	M");
    if(flag)
        printf("Line	OP	R	L	M");

    // This while loop reads in and stores the instructions
    // Then formats and prints them, as long as there are instructions to be read.
    // fscanf returns the number of successful reads, hopefully 4 in our case
    int i = 0;
    for(i = 0; i < programCounter; i++)
    {
        inst[i] = getInst(code[i]);
        fprintf(out, "\n%3d %7s %5d %7d %7d", i, opcode[inst[i].op], inst[i].r, inst[i].l, inst[i].m);
        if(flag)
            printf("\n%3d %7s %5d %7d %7d", i, opcode[inst[i].op], inst[i].r, inst[i].l, inst[i].m);
    }

    execute(out, opcode, inst, i);

    fclose(out);

    return;
}

void execute(FILE *out, char **opcode, instruction *inst, int length)
{
    // An instruction struct to represent the current instruction
    instruction ir;
    // Arrays to simulate the stack and the registers in the Register File
    int stack[2000] = {0}, rf[5000] = {0};
    // Another array to track where to put pipes for the activation records
    int ar[1000] = {0};
    int pc = 0, bp = 1, sp= 0;
    int halt = 0, index = 0, printFlag = 0, readFlag = 0, sioOutput;

    fprintf(out, "\n\nInital Values			pc    bp    sp");
    if(flag)
        printf("\n\nInital Values			pc    bp    sp");

    while(pc < length)
    {
        // Check if we should exit
        if(bp == 0 || halt == 1)
            return;

        ir = inst[pc];
        pc++;

        // Print the # of the current instruction, the name of the instruction,
        // and the arguments given to the instruction
        fprintf(out, "\n%3d %7s %4d %4d %4d", pc, opcode[ir.op], ir.r, ir.l, ir.m);
        if(flag)
            printf("\n%3d %7s %4d %4d %4d", pc, opcode[ir.op], ir.r, ir.l, ir.m);

        // Execute the instruction at PC
        switch(ir.op)
        {
            case 1:
                // lit
                rf[ir.r] = ir.m;
                break;
            case 2:
                // rtn
                sp = bp - 1;
                bp = stack[sp + 3];
                pc = stack[sp + 4];
                ar[bp] = 0;
                break;
            case 3:
                // lod
                index = base(stack, ir.l, bp) + ir.m;
                rf[ir.r] = stack[index];
                break;
            case 4:
                // sto
                //index = base(stack, ir.l, bp) + ir.m;
                index = stoBase(stack, ir.l, bp) + ir.m;
                stack[index] = rf[ir.r];
                break;
            case 5:
                // cal
                stack[sp + 1] = 0;                          // Return value
                stack[sp + 2] = base(stack, ir.l, bp);      // Static link
                stack[sp + 3] = bp;                         // Dynamic link
                stack[sp + 4] = pc;                         // Return address
                bp = sp + 1;
                pc = ir.m;
                ar[bp] = 1;
                break;
            case 6:
                // inc
                sp = sp + ir.m;
                break;
            case 7:
                // jmp
                pc = ir.m;
                break;
            case 8:
                // jpc
                if(rf[ir.r] == 0)
                {
                    pc = ir.m;
                }
                break;
            // The three sio opcodes make things a little more complicated
            case 9:
            case 10:
            case 11:
                if(ir.m == 1)
                {
                    // sio print
                    sioOutput = rf[ir.r];
                    printFlag = 1;
                }
                else if(ir.m == 2)
                {
                    // sio read
                    readFlag = 1;
                }
                else if(ir.m == 3)
                {
                    // sio halt
                    halt = 1;
                }
                break;
            case 12:
                // neg
                rf[ir.r] = -rf[ir.l];
                break;
            case 13:
                // add
                rf[ir.r] = rf[ir.l] + rf[ir.m];
                break;
            case 14:
                // sub
                rf[ir.r] = rf[ir.l] - rf[ir.m];
                break;
            case 15:
                // mul
                rf[ir.r] = rf[ir.l] * rf[ir.m];
                break;
            case 16:
                // div
                rf[ir.r] = rf[ir.l] / rf[ir.m];
                break;
            case 17:
                // odd
                rf[ir.r] = rf[ir.r] % 2;
                break;
            case 18:
                // mod
                rf[ir.r] = rf[ir.l] % rf[ir.m];
                break;
            case 19:
                // eql
                rf[ir.r] = (rf[ir.l] == rf[ir.m]) ? 1 : 0;
                break;
            case 20:
                // neq
                rf[ir.r] = (rf[ir.l] != rf[ir.m]) ? 1 : 0;
                break;
            case 21:
                // lss
                rf[ir.r] = (rf[ir.l] < rf[ir.m]) ? 1 : 0;
                break;
            case 22:
                // leq
                rf[ir.r] = (rf[ir.l] <= rf[ir.m]) ? 1 : 0;
                break;
            case 23:
                // gtr
                rf[ir.r] = (rf[ir.l] > rf[ir.m]) ? 1 : 0;
                break;
            case 24:
                // geq
                rf[ir.r] = (rf[ir.l] >= rf[ir.m]) ? 1 : 0;
                break;
        } // End switch

        // Print the pc, bp, and sp.
        fprintf(out, "%8d  %4d  %4d", pc, bp, sp);
        if(flag)
            printf("%8d  %4d  %4d", pc, bp, sp);

        // Print the stack
        index = 1;
        while(index <= sp)
        {
            // Print pipes at start of activation records
            if(ar[index] == 1)
            {
                fprintf(out, "    | ");
                if(flag)
                    printf("    | ");
            }

            fprintf(out, " %4d", stack[index]);
            if(flag)
                printf(" %4d", stack[index]);
            ++index;
        }

        if(printFlag)
        {
            fprintf(out, "\nProgram Output: %d\n", sioOutput);
            printf("\nProgram Output: %d\n", sioOutput);

            printFlag = 0;
        }

        if(readFlag)
        {
            printf("\nEnter a value: ");
            scanf("%d", &rf[ir.r]);
            readFlag = 0;
        }

    } // End while
}

int stoBase(int *stack, int l, int base)
{
    int b1 = base;

    while(l > 0)
    {
        b1 = stack[b1 + 2];
        l--;
    }

    return b1;
}

int base(int *stack, int l, int base)
{
    int b1 = base;

    while(l > 0)
    {
        b1 = stack[b1 + 1];
        l--;
    }

    return b1;
}

instruction getInst(char *codeString) {
    char *op = malloc(5 * sizeof(char));
    char *r = malloc(5 * sizeof(char));
    char *l = malloc(5 * sizeof(char));
    char *m = malloc(5 * sizeof(char));
    char *instArr[4] = {op, r, l, m};
    int length = strlen(codeString);
    codeString[length] = '\0';
    op[0] = '\0';
    r[0] = '\0';
    l[0] = '\0';
    m[0] = '\0';

    int instNum = 0;
    int strCtr = 0;
    int codeCtr = 0;
    char c = codeString[codeCtr++];
    while(c != '\0') {
        if(c != ' ') {
            instArr[instNum][strCtr] = c;
            strCtr++;
        }
        else {
            instArr[instNum][strCtr] = '\0';
            instNum++;
            strCtr = 0;
        }
        instArr[instNum][strCtr] = '\0';
        c = codeString[codeCtr++];
    }

    instruction inst;
    inst.op = atoi(instArr[0]);
    inst.r = atoi(instArr[1]);
    inst.l = atoi(instArr[2]);
    inst.m = atoi(instArr[3]);

    return inst;
}
