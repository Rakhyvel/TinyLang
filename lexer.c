/*  lexer.c

    The lexer takes a string of characters from a file, and turns it into a 
    more easily manipulated token. Using tokens is far superior to working with
    the raw text alone.
*/

#include "include.h"

// Some tokens that are always the same
static const char* tokenMap[] = {"!", "?", ":", "and", "or", "=", ">", "<", "+", "-", "*", "/",  "(", ")", "{", "}", "\n", "if", "while", "halt"};

/*
    Takes in a string representing a file, and turns it into a list of tokens */
struct list* tokenize(char* file) {
    struct list* tokenQueue = list_create();
    int start = 0, end = 0, fileLength = strlen(file);
    /* Create tokens from text, add them to a token queue */ 
    do {
        // Create token and add it to the list
        struct token* token = (struct token*) calloc(1, sizeof(struct token));
        token->children = list_create();
        queue_push(tokenQueue, token);

        // Find the end of the next token
        enum tokenState {BEGIN, TEXT, STRING};
        enum tokenState state = BEGIN;
        for(char nextChar = file[end]; nextChar != '\0'; end++, nextChar = file[end]) {
            if(state == BEGIN) {
                if(isalnum(nextChar)) {
                    state = TEXT;
                } else if(nextChar == '"') {
                    state = STRING;
                } else if (nextChar == '\n' || (!isdigit(nextChar) && !isalpha(nextChar) && !isspace(nextChar))){
                    end++;
                    break;
                }
            } else if(state == TEXT && (!isalnum(nextChar) && nextChar != '_')) {
                break;
            } else if (state == STRING && nextChar == '"') {
                end++;
                break;
            }
        }
        strncpy(token->data, file + start, end-start);

        // Find token type
        for(token->type = 3; token->type <= 22 && strcmp(tokenMap[token->type - 3], token->data); token->type++);

        // If the token is not one of the pre-defined ones, check which literal it is
        if(token->type == 23) {
            if(isdigit(token->data[0])) {
                token->type = NUM;
            } else if(token->data[0] == '"'){
                token->type = STR;
                int i;
                for (i = 1; token->data[i] != '"'; i++)
                    token->data[i-1] = token->data[i];
                token->data[i-1] = '\0';
            } else {
                token->type = IDENT;
            }
        }

        // Advance pointer to next non-whitespace token
        for (char nextChar = file[end]; nextChar != '\0' && isspace(nextChar) && nextChar != '\n'; nextChar = file[++end]);
        start = end;
    } while(end < fileLength);

    return tokenQueue;
}