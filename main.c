/*
    Date: 3/13/21
    Author: Joseph Shimel
*/

#include "include.h"

enum tokenState {BEGIN, TEXT, STRING};
static const char* tokenMap[] = {"!", "?", ":", "and", "or", "=", ">", "<", "+", "-", "*", "/",  "(", ")", "{", "}", "\n", "if", "while"};

int main(int argc, char **argv) {
    // open file
    if(argc < 2) exit(1);

    char* file = malloc(sizeof(char));
    /* Open file and read in characters to an ever growing string */ {
        FILE* fileptr = fopen(argv[1], "r");
        int buffer;
        for (int size = 1; (buffer = fgetc(fileptr)) != EOF; size++) {
            file = realloc(file, size + 1);
            file[size - 1] = (char)buffer;  // Convert int buffer to char, append
            file[size] = '\0'; // Add the sentinel value
        }
    }

    struct list* tokenQueue = list_create();
    int start = 0, end = 0, fileLength = strlen(file);
    char tokenBuffer[64];
    /* Create tokens from text, add them to a token queue */ do {
        // Find the end of the next token
        enum tokenState state = BEGIN;
        for(char nextChar = file[end]; nextChar != '\0'; end++, nextChar = file[end])
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

        // Copy over next token
        for(int i = 0; i < end-start; i++) {
            tokenBuffer[i] = file[i + start];
            tokenBuffer[i + 1] = '\0';
        }

        // Find token type
        enum tokenType tempType;
        for(tempType = 3; tempType <= 19 && strcmp(tokenMap[tempType - 3], tokenBuffer); tempType++);
        if(tempType == 20) {
            if(isdigit(tokenBuffer[0])) {
                tempType = TOKEN_NUM;
            } else if(tokenBuffer[0] == '"'){
                tempType = TOKEN_STR;
                for (int i = 1; tokenBuffer[i] != '"'; i++)
                    tokenBuffer[i - 1] = tokenBuffer[i];
            } else tempType = TOKEN_IDENT;
        }

        // Create token, and add to list
        struct token* token = (struct token*) malloc(sizeof(struct token));
        token->type = tempType;
        strncpy(token->data, tokenBuffer, 254);
        queue_push(tokenQueue, token);

        // Advance pointer to next non-whitespace token
        for (char nextChar = file[end]; nextChar != '\0' && isspace(nextChar) && nextChar != '\n'; nextChar = file[++end]);
        start = end;
    } while(end < fileLength);

    // parse tokenQueue into program (list of ASTs and a symbol map)
    struct list* program = list_create();
    while(!list_isEmpty(tokenQueue)) {
        queue_push(program, parser_parseAST(tokenQueue));
    }

    // interpret program
    interpreter_interpret(program, map_create());
}