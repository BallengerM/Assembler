/*
Makayla Ballenger
CS 271
March 5, 2018
Assembler
Citations:
Prof. Bowe -> strcmp in my symbol table
           -> is digit in my symbol finder
*/

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "symbolTable.h"
#include <ctype.h>

char* splitAt(char*);
char* splitEqualsLast(char*);
char* splitEqualsFirst(char*);
char* splitSemiLast(char*);
char* splitSemiFirst(char*);
char* dest(char*);
char* jmp(char*);
char* comp(char*);
int charToInt(char);
int stringToInt(char*);
char intToChar(int);
char* intToBinary(int);
char* stripParenthesis(char*);


int main(){

    //First file pass through
    FILE* fp1;
    char code1[256];
    //Creation of the symbol table
    struct SymbolTable myTable;
	myTable.head = NULL;

	//Pre-defined symbols
	insertAtEnd(&myTable, "SP", 0); insertAtEnd(&myTable, "LCL", 1); insertAtEnd(&myTable, "ARG", 2); insertAtEnd(&myTable, "THIS", 3);
	insertAtEnd(&myTable, "THAT", 4); insertAtEnd(&myTable, "SCREEN", 16384); insertAtEnd(&myTable, "KBD", 24576); insertAtEnd(&myTable, "R0", 0);
    insertAtEnd(&myTable, "R1", 1); insertAtEnd(&myTable, "R2", 2); insertAtEnd(&myTable, "R3", 3); insertAtEnd(&myTable, "R4", 4);
    insertAtEnd(&myTable, "R5", 5); insertAtEnd(&myTable, "R6", 6); insertAtEnd(&myTable, "R7", 7); insertAtEnd(&myTable, "R8", 8);
    insertAtEnd(&myTable, "R9", 9); insertAtEnd(&myTable, "R10", 10); insertAtEnd(&myTable, "R11", 11); insertAtEnd(&myTable, "R12", 12);
    insertAtEnd(&myTable, "R13", 13); insertAtEnd(&myTable, "R14", 14); insertAtEnd(&myTable, "R15", 15);

	//Opens the file
    fp1 = fopen("assemblertestfile.txt", "r");
    if(fp1 == NULL){
        printf("Error opening file\n");
    }
    else{
        //Counter to determine what number should be assigned to the label
        int romCount = 0;
        while(fgets(code1, 256, fp1) != NULL){
            code1[strcspn(code1, "\r\n")] = 0;
            //If it does not begin with a parenthesis, only increment the romCount
            if (code1[0] != '('){
                romCount++;
            }
            //If it does begin with a parenthesis, it is a label and needs to be added to the table
            else if (code1[0] == '('){
                char* label = stripParenthesis(code1);
                insertAtEnd(&myTable, label , romCount);
            }
        }
        //closes the file
        fclose(fp1);
    }

    //Creation of an area for the machine instruction
    char* instruction = malloc(sizeof(char)*17);

    //Starts instructions with all zeroes.
    int i;
    for (i=0; i<17; i++){
        instruction[i]='0';
    }
    instruction[16]='\0';

    FILE* fp;
    char code[256];

    //Opens the file for the second time
    fp = fopen("assemblertestfile.txt", "r");

    //Prepares the file that the program will write to
    FILE* op;
    op = fopen("assemblerout.txt", "w");

    if(fp == NULL){
        printf("Error opening file\n");
    }
    else if(op == NULL){
        printf("Error opening file\n");
    }
    else{
        //Keeps track of where to put new variables into the symbol table
        int variableLocation = 16;
        while(fgets(code, 256, fp) != NULL){
            code[strcspn(code, "\r\n")] = 0;
            //Starts instruction with all zeroes
            int i;
            for (i=0; i<17; i++){
                instruction[i]='0';
            }
            instruction[16]='\0';

        //If the code does not start with a parenthesis
        if (code[0] != '('){
            //If the code starts with an @
            if (code[0] == '@'){
                //Start the instruction as all zeroes
                int i;
                for (i=0; i<17; i++){
                    instruction[i]='0';
                }
                instruction[16]='\0';
                //Removes the @ from the code
                char* ACode = splitAt(code);
                //SYMBOL STUFF
                //If the first part of ACode is not a digit
                if (isdigit(ACode[0]) == 0){
                    int addressNum = searchTable(&myTable, ACode);
                    //If it was not found in the table, add it at the location
                    if (addressNum == -1){

                        insertAtEnd(&myTable, ACode , variableLocation);
                        char* binary = intToBinary(variableLocation);
                        int i;
                        int j=0;
                        for(i=0; i<16; i++){
                            instruction[i+1]=binary[j];
                            j++;
                        }
                        variableLocation++;
                    }
                    //If it was found, convert the address into binary for the instruction
                    else {
                        char* binary = intToBinary(addressNum);
                        int i;
                        int j=0;
                        for(i=0; i<16; i++){
                            instruction[i+1]=binary[j];
                            j++;
                        }
                    }
                    printf("%s\n",instruction);
                    fputs(instruction,op);
                    fputs("\n",op);
                }
                //A INSTRUCTION STUFF
                else {
                    int num = stringToInt(ACode);
                    char* binary = intToBinary(num);
                    int i;
                    int j = 0;
                    for (i=0; i<16; i++){
                        instruction[i+1]=binary[j];
                        j++;
                    }
                printf("%s\n",instruction);
                fputs(instruction,op);
                fputs("\n",op);
                }
            }
            else if (code[0] == 'M' || code[0] == 'D' || code[0] == 'A' || code[0] == '0' || code[0] == '1' || code[0] == '-' || code[0] == '!'){
                //C INSTRUCTION STUFF
                //Sets the first 3 bits to 1 because it is a c instruction.
                instruction[0]='1';instruction[1]='1';instruction[2]='1';

                //Check for = or ;
                char* destination = NULL;
                char* jump = NULL;

                destination = strchr(code,'=');
                jump = strchr(code,';');

                //DESTINATION
                if(destination != NULL){
                    //COMP STUFF
                    char* last = splitEqualsLast(code);
                    char* resultLast = comp(last);
                    //Check for M in comp
                    char* Mbit = NULL;
                    Mbit = strchr(last,'M');
                    if (Mbit != NULL){
                        instruction[3]='1';
                    }
                    else {
                        instruction[3]='0';
                    }
                    //Sets 4 - 9
                    int i;
                    int j = 0;
                    for(i=4; i<10; i++){
                        instruction[i]=resultLast[j];
                        j++;
                    }
                    //DESTINATION STUFF
                    //Gets the destination code out of the full code.
                    char* first = splitEqualsFirst(code);
                    char* resultFirst = dest(first);
                    //Sets 10, 11, 12
                    instruction[10]=resultFirst[0];instruction[11]=resultFirst[1];instruction[12]=resultFirst[2];

                    printf("%s\n",instruction);
                    fputs(instruction,op);
                    fputs("\n",op);
                }
                else if(jump != NULL){
                    //JUMP STUFF
                    char* lastSemi = splitSemiLast(code);
                    char* resultLastSemi = jmp(lastSemi);
                    //Sets 13, 14, 15
                    instruction[13]=resultLastSemi[0];instruction[14]=resultLastSemi[1];instruction[15]=resultLastSemi[2];instruction[16]='\0';

                    //COMP STUFF
                    char* firstSemi = splitSemiFirst(code);
                    char* resultFirstSemi = comp(firstSemi);
                    //Check for M in comp
                    char* Mbit = NULL;
                    Mbit = strchr(firstSemi,'M');
                    if (Mbit != NULL){
                        instruction[3]='1';
                    }
                    else {
                        instruction[3]='0';
                    }
                    //Sets 4 - 9
                    int i;
                    int j = 0;
                    for(i=4; i<10; i++){
                        instruction[i]=resultFirstSemi[j];
                        j++;
                    }
                    printf("%s\n",instruction);
                    fputs(instruction,op);
                    fputs("\n",op);
                }
                else{
                    printf("ERROR\n");
                }
            }

            else {
                printf("ERROR\n");
            }
        }
        }
        destroytable(&myTable);
        fclose(fp);
        fclose(op);
    }

    return 0;

}

char* splitAt(char* string){
    int position = 0;
    char* data = malloc((sizeof(char))+1);
    int i = position+1;
    int d = 0;
    for (i = position+1; i<strlen(string)+1; i++){
        data[d]=string[i];
        d++;
    }
    data[strlen(string)]='\0';
    return data;
}

char* splitEqualsLast(char* string){
    int position = 0;
    while (string[position] != '='){
        position++;
    }
    char* first = malloc((sizeof(char)*(position+1)));
    int i = 0;
    for (i=0; i<position; i++){
        first[i]=string[i];
    }
    first[position]='\0';
    position++;
    char* last = malloc(strlen(string)-position);
    int holder=0;
    for (i=position; i<strlen(string); i++){
        last[holder]=string[i];
        holder++;
    }
    last[strlen(string)-position]='\0';
    return last;
}

char* splitEqualsFirst(char* string){
    int position = 0;
    while (string[position] != '='){
        position++;
    }
    char* first = malloc((sizeof(char)*(position+1)));
    int i = 0;
    for (i=0; i<position; i++){
        first[i]=string[i];
    }
    first[position]='\0';
    return first;
}

char* splitSemiLast(char* string){
    int position = 0;
    while (string[position] != ';'){
        position++;
    }
    char* first = malloc((sizeof(char)*(position+1)));
    int i = 0;
    for (i=0; i<position; i++){
        first[i]=string[i];
    }
    first[position]='\0';
    position++;
    char* last = malloc(strlen(string)-position);
    int holder=0;
    for (i=position; i<strlen(string); i++){
        last[holder]=string[i];
        holder++;
    }
    last[strlen(string)-position]='\0';
    return last;
}

char* splitSemiFirst(char* string){
    int position = 0;
    while (string[position] != ';'){
        position++;
    }
    char* first = malloc((sizeof(char)*(position+1)));
    int i = 0;
    for (i=0; i<position; i++){
        first[i]=string[i];
    }
    first[position]='\0';
    return first;
}

char* dest(char* string){
    char* destinationInstruct = malloc(sizeof(char)*4);
    if (strlen(string) == 1){
        if (string[0] == 'M'){
            destinationInstruct[0] = '0';destinationInstruct[1] = '0';destinationInstruct[2] = '1';destinationInstruct[3] = '\0';
            return destinationInstruct;
        }
        else if (string[0] == 'D'){
            destinationInstruct[0] = '0';destinationInstruct[1] = '1';destinationInstruct[2] = '0';destinationInstruct[3] = '\0';
            return destinationInstruct;
        }
        else if (string[0] == 'A'){
            destinationInstruct[0] = '1';destinationInstruct[1] = '0';destinationInstruct[2] = '0';destinationInstruct[3] = '\0';
            return destinationInstruct;
        }
        else{
            printf("ERROR\n");
        }
    }
    else if (strlen(string) == 2){
        if (string[0] == 'M' && string[1] == 'D'){
            destinationInstruct[0] = '0';destinationInstruct[1] = '1';destinationInstruct[2] = '1';destinationInstruct[3] = '\0';
            return destinationInstruct;
        }
        else if (string[0] == 'A' && string[1] == 'M'){
            destinationInstruct[0] = '1';destinationInstruct[1] = '0';destinationInstruct[2] = '1';destinationInstruct[3] = '\0';
            return destinationInstruct;
        }
        else if (string[0] == 'A' && string[1] == 'D'){
            destinationInstruct[0] = '1';destinationInstruct[1] = '1';destinationInstruct[2] = '0';destinationInstruct[3] = '\0';
            return destinationInstruct;
        }
        else{
            printf("ERROR\n");
        }
    }
    else if (strlen(string) == 3){
        if (string[0] == 'A' && string[1] == 'M' && string[2] == 'D'){
            destinationInstruct[0] = '1';destinationInstruct[1] = '1';destinationInstruct[2] = '1';destinationInstruct[3] = '\0';
            return destinationInstruct;
        }
        else{
            printf("ERROR\n");
        }
    }
    else {
        printf("ERROR\n");
    }
}

char* jmp(char* string){
    char* jumpInstruct = malloc(sizeof(char)*4);
    if (string[0] == 'J' && string[1] == 'G' && string[2] == 'T'){
        jumpInstruct[0] = '0';jumpInstruct[1] = '0';jumpInstruct[2] = '1';jumpInstruct[3] = '\0';
        return jumpInstruct;
    }
    else if (string[0] == 'J' && string[1] == 'E' && string[2] == 'Q'){
        jumpInstruct[0] = '0';jumpInstruct[1] = '1';jumpInstruct[2] = '0';jumpInstruct[3] = '\0';
        return jumpInstruct;
    }
    else if (string[0] == 'J' && string[1] == 'G' && string[2] == 'E'){
        jumpInstruct[0] = '0';jumpInstruct[1] = '1';jumpInstruct[2] = '1';jumpInstruct[3] = '\0';
        return jumpInstruct;
    }
    else if (string[0] == 'J' && string[1] == 'L' && string[2] == 'T'){
        jumpInstruct[0] = '1';jumpInstruct[1] = '0';jumpInstruct[2] = '0';jumpInstruct[3] = '\0';
        return jumpInstruct;
    }
    else if (string[0] == 'J' && string[1] == 'N' && string[2] == 'E'){
        jumpInstruct[0] = '1';jumpInstruct[1] = '0';jumpInstruct[2] = '1';jumpInstruct[3]= '\0';
        return jumpInstruct;
    }
    else if (string[0] == 'J' && string[1] == 'L' && string[2] == 'E'){
        jumpInstruct[0] = '1';jumpInstruct[1] = '1';jumpInstruct[2] = '0';jumpInstruct[3] = '\0';
        return jumpInstruct;
    }
    else if (string[0] == 'J' && string[1] == 'M' && string[2] == 'P'){
        jumpInstruct[0] = '1';jumpInstruct[1] = '1';jumpInstruct[2] = '1';jumpInstruct[3] = '\0';
        return jumpInstruct;
    }
    else{
        printf("ERROR\n");
    }
}

char* comp(char* string){
    char* compInstruct = malloc(sizeof(char)*7);
    if (strlen(string) == 1){
        if (string[0] == '0'){
            compInstruct[0] = '1';compInstruct[1] = '0';compInstruct[2] = '1';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == '1'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '1';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '1';compInstruct[3] = '1';compInstruct[4] = '0';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'A'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '0';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'M'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '0';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else{
            printf("ERROR\n");
        }
    }
    else if (strlen(string) == 2){
        if (string[0] == '-' && string[1] == '1'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '1';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == '-' && string[1] == 'D'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '1';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == '-' && string[1] == 'A'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == '!' && string[1] == 'D'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '1';compInstruct[3] = '1';compInstruct[4] = '0';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == '!' && string[1] == 'A'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '0';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == '!' && string[1] == 'M'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '0';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == '-' && string[1] == 'M'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else {
            printf("ERROR\n");
        }
    }
    else if (strlen(string) == 3){
        if (string[0] == 'D' && string[1] == '+' && string[2] == '1'){
            compInstruct[0] = '0';compInstruct[1] = '1';compInstruct[2] = '1';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'A' && string[1] == '+' && string[2] == '1'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '-' && string[2] == '1'){
            compInstruct[0] = '0';compInstruct[1] = '1';compInstruct[2] = '1';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'A' && string[1] == '-' && string[2] == '1'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '+' && string[2] == 'A'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '-' && string[2] == 'A'){
            compInstruct[0] = '0';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'A' && string[1] == '-' && string[2] == 'D'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '0';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '&' && string[2] == 'A'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '0';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '|' && string[2] == 'A'){
            compInstruct[0] = '0';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '1';compInstruct[4] = '0';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'M' && string[1] == '+' && string[2] == '1'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'M' && string[1] == '-' && string[2] == '1'){
            compInstruct[0] = '1';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '+' && string[2] == 'M'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '-' && string[2] == 'M'){
            compInstruct[0] = '0';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'M' && string[1] == '-' && string[2] == 'D'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '0';compInstruct[3] = '1';compInstruct[4] = '1';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '&' && string[2] == 'M'){
            compInstruct[0] = '0';compInstruct[1] = '0';compInstruct[2] = '0';compInstruct[3] = '0';compInstruct[4] = '0';compInstruct[5] = '0';compInstruct[6] = '\0';
            return compInstruct;
        }
        else if (string[0] == 'D' && string[1] == '|' && string[2] == 'M'){
            compInstruct[0] = '0';compInstruct[1] = '1';compInstruct[2] = '0';compInstruct[3] = '1';compInstruct[4] = '0';compInstruct[5] = '1';compInstruct[6] = '\0';
            return compInstruct;
        }
        else{
            printf("ERROR\n");
        }
    }
    else {
        printf("ERROR\n");
    }
}

int charToInt(char digit){
	return digit - '0';
}

char intToChar(int num){
	return num + '0';
}

int stringToInt(char* string){
	int len = strlen(string);
	int sum = 0;
	int power = 0;

	int i;
	for(i=len-1; i>=0; i--){
        sum = sum + (charToInt(string[i])*pow(10,power++));
	}

	return sum;
}

char* intToBinary(int num){
	char *result = (char*)malloc(16*sizeof(char));

	int length = 16;
	int i;

	for(i=0; i<length; i++){
        result[i]='0';
	}

	int position = 14;

    while (num > 0){

        int remainder = num % 2;
        num = num/2;

        result[position] = intToChar(remainder);
        position--;
    }

	result[15] = '\0';
	return result;
}

char* stripParenthesis(char* string){
    int position = 0;
    char* data = malloc((sizeof(char))+1);
    int i = position+1;
    int d = 0;
    for (i = position+1; i<strlen(string)-1; i++){
        data[d]=string[i];
        d++;
    }
    data[strlen(string)-2]='\0';
    return data;
}
