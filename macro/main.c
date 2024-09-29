//
//  main.c
//  macro
//
//  Created by Елена Логинова on 29.09.2024.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>


#define size 16

FILE* input;
FILE* output;

char argList[size][size];
char tempLine[128];
int macroId = 0;
int paramId = 0;
int currentArgId = 0;
int macroQtr = 0;
bool inMacro = false;
bool isArgument = false;
bool macroExists = false;

struct{
    char macroName[128];
    char paramsList[size][size];
    char code[size*size];
    int paramsQty;
}macroArr[size];;

int main(void) {
    input = fopen("input.txt", "r");
    output = fopen("output.txt", "w");
    for (int i = 0; i < size; i++) {
        macroArr[i].paramsQty = 0;
    }
    while (fgets(tempLine, sizeof(tempLine), input)) { // reading string by string
        if (strstr(tempLine, "MACRO")) { // reading macro
            inMacro = true;
            strcpy(macroArr[macroQtr].macroName, strtok(tempLine, " "));

            strtok(NULL, " "); // skipping "macro" word

            char* parameter = strtok(NULL, " ,\n"); // reading parametres
            while (parameter) {
                strcpy(macroArr[macroQtr].paramsList[macroArr[macroQtr].paramsQty], parameter);
                parameter = strtok(NULL, " ,\n");
                macroArr[macroQtr].paramsQty++;
            }
        }
        if (strstr(tempLine, "ENDM")) { // checking the end of macro
            inMacro = false;
            macroQtr++;
            continue;
        }
        if (inMacro) { // writing macro
            strcat(macroArr[macroQtr].code, tempLine);
        }
        if (!inMacro) { // strings out of macro declaration
            macroExists = false;
            paramId = 0;
            for (int i = 0; i < macroQtr; i++) {
                if (strstr(tempLine, macroArr[i].macroName)) { // looking for marco
                    macroExists = true;
                    macroId = i;
                    break;
                }
            }
            if (macroExists) { // if there's a call for macro
                strtok(tempLine, " ,\n"); //Skip macro name

                char* parameter = strtok(NULL, " ,\n"); // reading arguments
                while (parameter) {
                    strcpy(argList[paramId], parameter);
                    parameter = strtok(NULL, " ,\n");
                    paramId++;
                }
                char* saveptr1 = NULL, * saveptr2 = NULL;  // Pointers to save contexts of strtok_s
                // Parse macro_string line by line. saveptr1 is used to keep context between calls
                char* macro_string = strtok_s(macroArr[macroId].code, "\n", &saveptr1);
                while (macro_string) {
                    // Parse macro_string word by word. saveptr2 is used to keep context between this inner calls
                    char* macro_word = strtok_s(macro_string, " ", &saveptr2);

                    for (int j = 0; j < 3; j++) {
                        isArgument = false;
                        if (macro_word == NULL)
                            break;
                        for (int i = 0; i < macroArr[macroId].paramsQty; i++) { // checking if the word is parameter
                            if (!strcmp(macro_word, macroArr[macroId].paramsList[i])) {
                                isArgument = true;
                                currentArgId = i;
                                break;
                            }
                        }
                        // if it's parameter, write it's argument
                        if (isArgument) {
                            fputs(" ", output);
                            fputs(argList[currentArgId], output);
                        }
                        else { // if not, just wright the word
                            fputs(" ", output);
                            fputs(macro_word, output);
                        }
                        macro_word = strtok_s(NULL, " ", &saveptr2);
                    }
                    fputs("\n", output);
                    macro_string = strtok_s(NULL, "\n", &saveptr1);
                }
            }
            else { // writing simple strings of code
                fputs(tempLine, output);
            }
        }
    }
}

