/*  lexer.c

    The lexer takes a string of characters from a file, and turns it into a 
    more easily manipulated token. Using tokens is far superior to working with
    the raw text alone.
*/

#include "include.h"

// Some tokens that are always the same
static const char* tokenMap[] = {"!", "?", ":", "and", "or", "=", ">", "<", "+", "-", "*", "/",  "(", ")", "\n", "if", "while", "end", "halt"};

/*
    Takes in a file handle, pulls out the first token from the file and returns
    it
    
    Token data is 0 length if EOF */
struct token* nextToken(FILE* fileptr) {
    int nextChar;

    // Create token and add it to the list
    struct token* token = (struct token*) calloc(1, sizeof(struct token));
    token->children = list_create();

    // State machine, loop through possible token states
    enum tokenState {BEGIN, TEXT, STRING};
    enum tokenState state = BEGIN;
    while ((nextChar = fgetc(fileptr)) != EOF) {
        if(state == TEXT && (!isalnum(nextChar) && nextChar != '_')) {
            ungetc(nextChar, fileptr);
            break;
        } else {
            int length = strlen(token->data);
            token->data[length] = nextChar;
            token->data[length+1] = 0;
            if(state == BEGIN) {
                if(isalnum(nextChar)) {
                    state = TEXT;
                } else if(nextChar == '"') {
                    state = STRING;
                } else if (nextChar == '\n' || (!isdigit(nextChar) && !isalpha(nextChar) && !isspace(nextChar))){
                    break;
                }
            } else if (state == STRING && nextChar == '"') {
                break;
            }
        }
    }

    // Find simple token types (not literals)
    for(token->type = 3; token->type < 22 && strcmp(tokenMap[token->type - 3], token->data); token->type++);

    // EOF token
    if (strlen(token->data) == 0) {
        token->type = HALT;
    }
    // If the token is not one of the pre-defined ones, check which literal it is
    else if(token->type == 22) {
        if(isdigit(token->data[0])) {
            token->type = NUM;
        } else if(token->data[0] == '"'){
            token->type = STR;
            int i;
            for (i = 1; token->data[i] != '"'; i++) {
                token->data[i-1] = token->data[i];
            }
            token->data[i-1] = '\0';
        } else {
            token->type = IDENT;
        }
    }

    // Advance to next non whitespace
    while(isspace(nextChar = fgetc(fileptr)) && nextChar != '\n');
    ungetc(nextChar, fileptr);

    return token;
}