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
    Open specified file. Read in the next token, and execute it, free token, repeat */
int main(int argc, char **argv) {
    // Check command line arguments
    if(argc < 2) {
        fprintf(stderr, "Error: You need to specify a file to read!\n");
        exit(1);
    }

    /* Open file*/
    FILE* fileptr = fopen(argv[1], "r");
    if(fileptr == NULL) { // IO error with file
        perror(argv[1]);
        exit(2);
    }

    struct map* varMap = map_create();
    while(1) {
        interpretAST(parser_parseAST(fileptr), varMap, true);
    }

    fclose(fileptr);
}