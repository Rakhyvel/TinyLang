/*  tinylang.c

    This interpreter is kept intentionally small in order to show how a basic
    interpreter works. It doesn't have functions, arrays, structs/objects, or 
    types. Just the bare minimum to learn about tokenization, and parsing.

    To see how a fully featured programming language is made, I suggest you 
    check out my programming language, Orange:
    https://github.com/Rakhyvel/Orange 

    Date: 3/13/21
    Author: Joseph Shimel
*/

#include "include.h"


/*
    The main function opens the specified file, turns the text into a list of 
    tokens, creates the list of AST's, and then finally interprets that list
    into as a program */
int main(int argc, char **argv) {
    // Check command line arguments
    if(argc < 2) {
        fprintf(stderr, "Error: You need to specify a file to read!\n");
        exit(1);
    }

    /* Open file*/
    FILE* fileptr = fopen(argv[1], "r");
    if(fileptr == NULL) {
        perror(argv[1]); // will print out an error message for us
        exit(2);
    }
    // Read in contents to a large string
    char* file = malloc(sizeof(char));
    int buffer;
    for (int size = 1; (buffer = fgetc(fileptr)) != EOF; size++) {
        file = realloc(file, size + 1);
        file[size - 1] = (char)buffer;  // Convert int buffer to char, append
        file[size] = '\0'; // Add the sentinel value
    }
    fclose(fileptr);

    struct list* tokenQueue = tokenize(file);    

    // Parse token queue into a list of AST's
    struct list* program = list_create();
    while(!list_isEmpty(tokenQueue)) {
        queue_push(program, parser_parseAST(tokenQueue));
    }

    // interpret program
    interpreter_interpret(program, map_create());
}