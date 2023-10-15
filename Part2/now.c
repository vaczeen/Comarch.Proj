#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

//opcode
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

// Function to handle ADD operation
void addOperation(int* state, int* op_args) {
    int regA = state[op_args[0]];
    int regB = state[op_args[1]];
    state[op_args[2]] = regA + regB;
}

// Function to handle NAND operation
void nandOperation(int* state, int* op_args) {
    int regA = state[op_args[0]];
    int regB = state[op_args[1]];
    state[op_args[2]] = ~(regA & regB);
}

// Function to handle LW operation
void lwOperation(int* state, int* op_args, int* memory, int numMemory) {
    int offset = op_args[2] + state[op_args[0]];
    if (offset < 0 || offset >= numMemory) {
        fprintf(stderr, "ERROR: Invalid memory address: %d (out of range).\n", offset);
        exit(1);
    }
    state[op_args[1]] = memory[offset];
}

// Function to handle SW operation
void swOperation(int* state, int* op_args, int* memory, int numMemory) {
    int offset = op_args[2] + state[op_args[0]];
    if (offset < 0 || offset >= numMemory) {
        fprintf(stderr, "ERROR: Invalid memory address: %d (out of range).\n", offset);
        exit(1);
    }
    memory[offset] = state[op_args[1]];
}

// Function to handle BEQ operation
void beqOperation(int* state, int* op_args) {
    int regA = state[op_args[0]];
    int regB = state[op_args[1]];
    if (regA == regB) {
        state[6] = state[6] + op_args[2];
    }
}

// Function to perform JALR operation
void jalrOperation(int* state, int* op_args) {
    int regA = state[op_args[0]];
    state[op_args[2]] = regA;
    state[6] = regA;
}

// Function to get binary values from memory
// void getBinaryValues(int* memory, int numMemory) {
//     printf("Binary values are (32 bits):\n");

//     for (int i = 0; i < numMemory; i++) {
//         unsigned int dec = (unsigned int)memory[i];
//         unsigned int mask = 1 << 31; // Start from the leftmost bit
//         for (int j = 0; j < 32; j++) {
//             int bit = (dec & mask) ? 1 : 0;
//             printf("%d", bit);
//             mask >>= 1; // Shift the mask to the right
//         }
//         printf("\n");
//     }
// }

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;
    unsigned int i;
    stateType* statePtr = &state; // Pointer to the state

    if (argc != 2) {
        printf("error: usage: %s <machine-code-file.txt>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    // Initialize registers
    for (i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }

    /* Read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
         state.numMemory++) {
        if (sscanf(line, "%d", state.mem + state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        } else if (state.numMemory >= NUMMEMORY) {
            printf("error in Max number of words exceeded. Max: %d\n", NUMMEMORY);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    //getBinaryValues(state.mem, state.numMemory);

    int instructions_executed = 0;

    for (i = 0; i < 19; i++) {
        int op_type = state.mem[state.pc] >> 22;
        int op_args[3];

        op_args[0] = (state.mem[state.pc] >> 19) & 0x7;
        op_args[1] = (state.mem[state.pc] >> 16) & 0x7;
        op_args[2] = state.mem[state.pc] & 0x7;

        switch (op_type) {
            case ADD:
                addOperation(state.reg, op_args);
                break;
            case NAND:
                nandOperation(state.reg, op_args);
                break;
            case LW:
                lwOperation(state.reg, op_args, state.mem, state.numMemory);
                break;
            case SW:
                swOperation(state.reg, op_args, state.mem, state.numMemory);
                break;
            case BEQ:
                beqOperation(state.reg, op_args);
                break;
            case JALR:
                jalrOperation(state.reg, op_args);
                break;
            case HALT:
                printf("machine halted\ntotal of %d instructions executed\n", instructions_executed);
                i = -1;
                break;
            case NOOP:
                // Do nothing
                break;
            default:
                fprintf(stderr, "ERROR: Memory at location %i illegible.\n", state.pc);
                exit(1);
                break;
        }
        state.pc++;
        instructions_executed++;
        
   printf("\n@@@\nstate:\n");
        printf("\tpc %d\n", state.pc);
        printf("\tmemory:\n");
        for (int j = 0; j < state.numMemory; j++) {
            printf("\t\tmem[ %d ] %d\n", j, state.mem[j]);
        }
        printf("\tregisters:\n");
        for (int k = 0; k < NUMREGS; k++) {
            printf("\t\treg[ %d ] %d\n", k, state.reg[k]);
        }
        printf("end state\n");
    }
    return (0);
}

void printState(stateType *statePtr) {
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}
