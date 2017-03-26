// Austin Shipley
// 3/26/2017
// vm.c for COP3402 System Software with Euripides Montagne HW3

#include <stdio.h>
#include <stdlib.h>
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
FILE* out;
int flag;

void vm(int);
void execute(FILE *in, FILE *out, char **opcode, instruction *inst, int numInst);
int base(int *stack, int l, int base);
void output(char*);

void vm(int directive)
{
    printf("\nBegin VM\n\n");
    // An array for easy access to opcodes
    // The 0th entry is blank to make the rest 1-based
    char *opcode[25] = { "",
        "lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc",
        "sio", "sio", "sio", "neg", "add", "sub", "mul", "div",
        "odd", "mod", "eql", "neq", "lss", "leq", "gtr", "geq"
    };
    int i = 0;
    FILE* in = fopen("instructions.txt", "r");
    out = fopen("log.txt", "w");
    flag = directive;

    if(in == NULL || out == NULL)
    {
        output("File pointer failure.");
        return;
    }

    // Print the header
    output("Line	OP	R	L	M");

    // This while loop reads in and stores the instructions
    // Then formats and prints them, as long as there are instructions to be read.
    // fscanf returns the number of successful reads, hopefully 4 in our case
    while(fscanf(in, "%d %d %d %d", &inst[i].op, &inst[i].r, &inst[i].l, &inst[i].m) == 4)
    {
        fprintf(out, "\n%3d %7s %5d %7d %7d", i, opcode[inst[i].op], inst[i].r, inst[i].l, inst[i].m);
        if(flag)
            printf("\n%3d %7s %5d %7d %7d", i, opcode[inst[i].op], inst[i].r, inst[i].l, inst[i].m);
        i++;
    }

    // Check if read terminated due to EOF or an error
    if (!feof(in))
    {
        output("\nThe instructions were not read in successfully. Please see the readme for debugging help.");
        return;
    }

    execute(in, out, opcode, inst, i);

    fclose(in);
    fclose(out);

    return;
}

void execute(FILE *in, FILE *out, char **opcode, instruction *inst, int length)
{
    // An instruction struct to represent the current instruction
    instruction ir;
    // Arrays to simulate the stack and the registers in the Register File
    int stack[2000] = {0}, rf[5000] = {0};
    // Another array to track where to put pipes for the activation records
    int ar[1000] = {0};
    int pc = 0, bp = 1, sp= 0;
    int halt = 0, index = 0;

    output("\n\nInital Values			pc    bp    sp");

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
                index = base(stack, ir.l, bp) + ir.m;
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
                switch(ir.m)
                {
                    case 1:
                        // sio print
                        fprintf(out, "\n%d", rf[ir.r]);
                        if(flag)
                            printf("\n%d", rf[ir.r]);
                        break;
                    case 2:
                        // sio read
                        fscanf(in, "%d", &rf[ir.r]);
                        break;
                    case 3:
                        // sio halt
                        halt = 1;
                        break;
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
                output("    | ");

            fprintf(out, " %4d", stack[index]);
            if(flag)
                printf(" %4d", stack[index]);
            ++index;
        }
    } // End while
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
