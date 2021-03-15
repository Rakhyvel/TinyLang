/*  lexer.c

    Lexical analysis is done before the compiler can do anything else.
    
    The characters in the text file are grouped together into tokens,
    which can be manipulated and parsed easier in later stages of the
    compilation. 

    - The lexer DOES NOT care if the tokens are in a proper order
    - The lexer ONLY turns the text data into a token queue

    Author: Joseph Shimel
    Date: 2/3/21
*/

#include "common.h"

static const char* tokenMap[] = {"+", "-", "*", "/", ":=", "=", "!=", ">", "<", "and", "or", "!", "?", "(", ")", "{", "}", "\n", "if", "while"};

/*
    Runs a basic state machine.

    Symbols are one character long
    Keywords/identifiers start with alpha, contain only alphanumeric
    Numbers contain only digits
    
    Returns the index of the begining of the next token */
static int nextToken(const char* file, int start) {
    enum tokenState {BEGIN, TEXT, INTEGER, STRING, PUNCTUATION};
    enum tokenState state = BEGIN;

    for(char nextChar = file[start]; nextChar != '\0'; start++, nextChar = file[start])
        if(state == BEGIN) {
            if(isalpha(nextChar)) {
                state = TEXT;
            } else if(isdigit(nextChar)) {
                state = INTEGER;
            } else if(nextChar == '"') {
                state = STRING;
            } else if(nextChar == '\n') {
                start++;
                break;
            } else {
                state = PUNCTUATION;
            }
        } else if((state == TEXT && !isalpha(nextChar) && !isdigit(nextChar) && nextChar != '_')
                  || (state == INTEGER && !isdigit(nextChar))
                  || (state == PUNCTUATION && (isdigit(nextChar) || isalpha(nextChar) || isspace(nextChar)))) {
            break;
        } else if (state == STRING && nextChar == '"') {
            start++;
            break; 
        }
    return start;
}

/*
    Takes in a file represented as a string, and creates a list of tokens */
struct list* lexer_tokenize(FILE* fileptr) {
    int buffer;
    int size = 0;
    char*file = malloc(sizeof(char));
    while((buffer = fgetc(fileptr)) != EOF) {
        file = realloc(file, size++ + 1);
        file[size - 1] = (char)buffer;  /* Convert int buffer to char, append */
    }
    file[size] = '\0'; /* Add the sentinel value */

    struct list* tokenQueue = list_create();
    int start = 0, end, fileLength = strlen(file);
    char tokenBuffer[64];

    do {
        end = nextToken(file, start);
        // Copy over next token
        int i;
        for(i = 0; i < end-start; i++)
            tokenBuffer[i] = file[i + start];
        tokenBuffer[i] = '\0';

        // Find token type
        enum tokenType tempType = 3;
        while(tempType <= 22) {
            if(!strcmp(tokenMap[tempType - 3], tokenBuffer)) break;
            tempType++;
        }
        if(tempType == 23) {
            if(isdigit(tokenBuffer[0])) {
                tempType = TOKEN_NUM;
            } else if(tokenBuffer[0] == '"'){
                tempType = TOKEN_STR;
                char temp[255];
                memset(temp, 0, 255);
                for (int i = 1; tokenBuffer[i] != '"'; i++)
                    temp[i - 1] = tokenBuffer[i];
                strcpy(tokenBuffer, temp);
            } else tempType = TOKEN_IDENT;
        }

        // Create token, and add to list
        queue_push(tokenQueue, token_create(tempType, tokenBuffer));

        // Advance pointer to next non-whitespace token
        for (char nextChar = file[end]; nextChar != '\0'; nextChar = file[++end]) if(!isspace(nextChar) || nextChar == '\n') break;
        start = end;
    } while(end < fileLength);
    return tokenQueue;
}