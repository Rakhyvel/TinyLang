/*  ast.c

    An Abstract Syntax Tree is a way to represent the syntax and higher level
    relationships in code in a tree-like structure.

    Author: Joseph Shimel
    Date: 3/12/21
*/

#include "common.h"


/*
    Allocates and initializes an Abstract Syntax Tree node, with the proper type */
struct astNode* ast_create(enum astType type) {
    struct astNode* retval = (struct astNode*) malloc(sizeof(struct astNode));
    retval->type = type;
    retval->children = list_create();
    return retval;
}

/*
    Used to convert between token types for operators, and ast types for 
    operators. Must have a one-to-one mapping. TOKEN_LPAREN for example, 
    does not have a one-to-one mapping with any AST type.
    
    Called by createExpression to convert operator tokens into ASTs */
enum astType ast_tokenToAST(enum tokenType type) {
    if(type <= TOKEN_QMARK) return (enum astType)type;
    exit(4);
}