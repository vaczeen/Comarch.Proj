#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
// Define a structure to represent a line of assembly code
typedef struct {
    int address;  // Address(line) of assembly
    char label[MAXLINELENGTH];   // Label field 
    char opcode[MAXLINELENGTH];  // Opcode field 
    char arg0[MAXLINELENGTH];  // Field 0 (argument 0) field 
    char arg1[MAXLINELENGTH];  // Field 1 (argument 1) field 
    char arg2[MAXLINELENGTH];  // Field 2 (argument 2) field 
} AssemblyCode;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int checkLines(FILE *);
int codeParse(FILE *inFilePtr, AssemblyCode code[],int lines);
int map(AssemblyCode* code, int address,int lines);
void intToBinaryString(int value, char *binaryStr, int numBits);
int findLabelAddress(const AssemblyCode* code, int numInstructions, const char* label);
char* binaryToHex(const char* binary);
char* hexToDecimal(const char* hexString);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }


    const int lines = checkLines(inFilePtr);
    printf("Number of lines %d\n",lines);
    
    AssemblyCode code[lines];
    codeParse(inFilePtr,code,lines);
    printf("\n");

    int out[lines];
    for(int i = 0; i< lines;i++){
        out[i] = map(code,i,lines);
        fprintf(outFilePtr, "%d\n", out[i]); // Write the output to the file
    }

    printf("\n");
    for(int i = 0; i< lines;i++){
        printf("%d\n",out[i]);
    }


}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2) {
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }

    /* Check if the first string cluster is a label */
    ptr = line;
    while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t')) {
        ptr++; // Skip leading whitespace
    }
    
     /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]", opcode, arg0, arg1, arg2);
    return(1);
}

int isNumber(char *string){
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int checkLines(FILE *inFilePtr){
    int lines = 0;

    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        lines++;
    }

    rewind(inFilePtr);

    return lines;

}

int codeParse(FILE *inFilePtr, AssemblyCode code[],int lines) {
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    int address = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        // Check if the label is too long
        if (strlen(label) > 6) {
            fprintf(stderr, "error: label '%s' is longer than 6 characters \n", label);
            exit(0); // Return 0 to indicate an error
        }

        // Check for duplicate labels in the code array
        for (int i = 0; i < lines; i++) {
            // Check if both labels are not empty and are equal
            if (code[i].label[0] != '\0' && label[0] != '\0' && strcmp(code[i].label, label) == 0) {
                fprintf(stderr, "error: duplicate label '%s' \n", label);
                exit(0); // Return 0 to indicate an error
            }
        }



        if (!strcmp(opcode, "add") || !strcmp(opcode, "nand") || !strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
            code[address].address = address;
            strcpy(code[address].label, label);
            strcpy(code[address].opcode, opcode);
            strcpy(code[address].arg0, arg0);
            strcpy(code[address].arg1, arg1);
            strcpy(code[address].arg2, arg2);
        } else if (!strcmp(opcode, "jalr")) {
            code[address].address = address;
            strcpy(code[address].label, label);
            strcpy(code[address].opcode, opcode);
            strcpy(code[address].arg0, arg0);
            strcpy(code[address].arg1, arg1);
            code[address].arg2[0] = '\0'; // No arg2 in jalr
        } else if (!strcmp(opcode, ".fill")) {
            code[address].address = address;
            strcpy(code[address].label, label);
            strcpy(code[address].opcode, opcode);
            strcpy(code[address].arg0, arg0);
            code[address].arg1[0] = '\0'; // No arg1 in .fill
            code[address].arg2[0] = '\0'; // No arg2 in .fill
        } else if (!strcmp(opcode, "noop") || !strcmp(opcode, "halt")) {
            strcpy(code[address].label, label);
            strcpy(code[address].opcode, opcode);
            code[address].address = address;
            code[address].arg0[0] = '\0'; // No arg0 in O, halt
            code[address].arg1[0] = '\0'; // No arg1 in O, halt
            code[address].arg2[0] = '\0'; // No arg2 in O, halt
        } else {
            fprintf(stderr, "error: undefined opcode '%s' at address %d\n", opcode, address);
            exit(0); // Return 0 to indicate an error
        }

        printf("(address %d): label: %-6s opcode: %-5s arg0: %-5s arg1: %-5s arg2: %-5s \n",
               address, code[address].label, code[address].opcode, code[address].arg0, code[address].arg1, code[address].arg2);

        address++;
    }

    rewind(inFilePtr);

    return 1; // Return 1 to indicate success
}

// Function to map assembly instruction to machine code
int map(AssemblyCode* code, int address,int lines) {
    static char machineCode[33]; // 32 characters + null terminator

    char value[16];

    // Initialize machine code to all zeros
    memset(machineCode, '0', 32);
    machineCode[32] = '\0'; // Null-terminate the string

    char opcode[4]; // 3 characters + null terminator

    char regA[4]; intToBinaryString(atoi(code[address].arg0),regA,3);
    char regB[4]; intToBinaryString(atoi(code[address].arg1),regB,3);


    char dest[4]; // 3 bits + null terminator (for add and nand)
    char offset[17]; //16 bits + null terminator (for beq lw sw)

    if (strcmp(code[address].opcode, "add") == 0) {
        strcpy(opcode, "000");
        intToBinaryString(atoi(code[address].arg2),dest,3);
        snprintf(machineCode, 33, "0000000%s%s%s0000000000000%s", opcode, regA, regB, dest);

    }else if (strcmp(code[address].opcode, "nand") == 0) {
        strcpy(opcode, "001");
        intToBinaryString(atoi(code[address].arg2),dest,3);
        snprintf(machineCode, 33, "0000000%s%s%s0000000000000%s", opcode, regA, regB, dest);
    }else if (strcmp(code[address].opcode, "lw") == 0) {
        strcpy(opcode, "010");
        if(isNumber(code[address].arg2)){
            intToBinaryString(atoi(code[address].arg2),offset,16);
        }else{
            int labelAddress = findLabelAddress(code, lines, code[address].arg2);
        if (labelAddress == -1) {
            fprintf(stderr, "error: label '%s' not found at address %d\n", code[address].arg2, address);
            exit(1);
            }
            intToBinaryString(code[labelAddress].address, offset, 16);
        }
        snprintf(machineCode, 33, "0000000%s%s%s%s", opcode, regA, regB, offset);
    }else if (strcmp(code[address].opcode, "sw") == 0) {
        strcpy(opcode, "011");
        if(isNumber(code[address].arg2)){
            intToBinaryString(atoi(code[address].arg2),offset,16);
        }else{
            int labelAddress = findLabelAddress(code, lines, code[address].arg2);
        if (labelAddress == -1) {
            fprintf(stderr, "error: label '%s' not found at address %d\n", code[address].arg2, address);
            exit(1);
            }
            intToBinaryString(code[labelAddress].address, offset, 16);
        }
        snprintf(machineCode, 33, "0000000%s%s%s%s", opcode, regA, regB, offset);
    }else if (strcmp(code[address].opcode, "beq") == 0) {
        strcpy(opcode, "100");
        if(isNumber(code[address].arg2)){
            intToBinaryString(atoi(code[address].arg2),offset,16);
        }else{
             // Find the address of the label in AssemblyCode
            int entryAddress = findLabelAddress(code, lines, code[address].arg2);
            if (entryAddress == -1) {
                fprintf(stderr, "error: label '%s' not found at address %d\n", code[address].arg2, address);
                exit(1);
            }
            // Calculate the offset
            int offsetValue = entryAddress - (address + 1);
            if (offsetValue < -32768 || offsetValue > 32767) {
                fprintf(stderr, "error: offset out of range for label '%s' at address %d\n", code[address].arg2, address);
                exit(1);
            }
            // Convert the offset value to a 16-bit binary string
            intToBinaryString(offsetValue, offset, 16);
        }
        snprintf(machineCode, 33, "0000000%s%s%s%s", opcode, regA, regB,offset);
    }else if (strcmp(code[address].opcode, "jalr") == 0) {
        strcpy(opcode, "101");
        snprintf(machineCode, 33, "0000000%s%s%s0000000000000", opcode, regA, regB);
    }else if (strcmp(code[address].opcode, "halt") == 0) {
        strcpy(opcode, "110");
        snprintf(machineCode, 33, "0000000%s0000000000000000000000", opcode);
    }else if (strcmp(code[address].opcode, "noop") == 0) {
        strcpy(opcode, "111");
        snprintf(machineCode, 33, "0000000%s0000000000000000000000", opcode);
    }else if (strcmp(code[address].opcode, ".fill") == 0) {
        strcpy(opcode, "000");
        if (isNumber(code[address].arg0)) {
            // If arg0 is a number, use it as machine code
            intToBinaryString(atoi(code[address].arg0), offset, 16);
            strcpy(value,code[address].arg0);
            if(atoi(code[address].arg0) > 0)snprintf(machineCode, 33, "0000000000000000%s", offset);
            else snprintf(machineCode, 33, "1111111111111111%s", offset);
            
        } else {
            int entryAddress = findLabelAddress(code, lines, code[address].arg0);
            sprintf(value, "%d", entryAddress); 
            intToBinaryString(entryAddress, offset, 16);
            if(entryAddress > 0 )snprintf(machineCode, 33, "0000000000000000%s", offset);
            else snprintf(machineCode, 33, "1111111111111111%s", offset);
        }

    }

        printf("(address %d): %s", address, machineCode);
        char * hex = binaryToHex(machineCode);
        printf(" = (hex) 0x%s",hex);
        if(strcmp(code[address].opcode,".fill")!=0){
            
            char* decimalString = hexToDecimal(hex);
            printf(" = %s \n",decimalString);
            return atoi(decimalString);
        }else{
            printf(" = %s \n",value);   
            return atoi(value);         
        }
}

// Function to convert an integer to a binary string with leading zeros
void intToBinaryString(int value, char *binaryStr, int numBits) {
    for (int i = numBits - 1; i >= 0; i--) {
        binaryStr[i] = (value & 1) + '0'; // Convert the least significant bit to '0' or '1'
        value >>= 1; // Right-shift the value
    }
    binaryStr[numBits] = '\0'; // Null-terminate the string
}

// Function to find the address of an entry with a specific label
int findLabelAddress(const AssemblyCode* code, int numInstructions, const char* label) {
    for (int i = 0; i < numInstructions; i++) {
        if (strcmp(code[i].label, label) == 0) {
            return i;
        }
    }
    return -1; // Label not found
}


char* binaryToHex(const char* binary) {
    int binaryLength = strlen(binary);
    
    // Ensure that the length of the binary string is a multiple of 4
    if (binaryLength % 4 != 0) {
        fprintf(stderr, "error: Binary string length must be a multiple of 4.\n");
        exit(1);
    }
    
    int hexLength = binaryLength / 4;
    char* hexString = (char*)malloc(hexLength + 1); // +1 for null terminator
    
    if (hexString == NULL) {
        fprintf(stderr, "error: Memory allocation failed\n");
        exit(1);
    }
    
    for (int i = 0; i < hexLength; i++) {
        char binaryChunk[5]; // 4 bits + null terminator
        strncpy(binaryChunk, binary + i * 4, 4);
        binaryChunk[4] = '\0';
        int decimalValue = strtol(binaryChunk, NULL, 2);
        sprintf(hexString + i, "%x", decimalValue);
    }
    
    hexString[hexLength] = '\0'; // Null-terminate the string
    
    return hexString;
}


// Function to convert a hexadecimal string to a decimal string
char* hexToDecimal(const char* hexString) {
    // Convert the hexadecimal string to a long integer
    long decimalValue = strtol(hexString, NULL, 16);

    // Check for errors in conversion
    if (decimalValue == 0 && errno != 0) {
        perror("strtol");
        return NULL; // Conversion error
    }

    // Allocate memory for the decimal string (long enough to hold any long value)
    char* decimalString = (char*)malloc(21); // 20 digits + 1 null terminator

    // Convert the long integer to a decimal string
    snprintf(decimalString, 21, "%ld", decimalValue);

    return decimalString;
}
