/*
    Author: Joseph Shimel
    Date: 3/12/21
*/

#include "common.h"

/*
    AST nodes have types that tell them apart */
enum astType {
	// Literals
    AST_IDENT, AST_NUM, AST_STR,
    // Math operators
	AST_ADD, AST_SUB, AST_MULT, AST_DIV, AST_ASSIGN, 
    // Branch operators
    AST_IS, AST_ISNT, AST_GTR, AST_LSR,
    // Boolean operators
    AST_AND, AST_OR, 
    // IO operatios
    AST_OUT, AST_IN,
    // StatementNode types
	AST_BLOCK, AST_IF, AST_WHILE,
    // Anonymous
    AST_NOP
};

/*
    Abstract Syntax Trees describe the actual code of a language in a more
    efficient way. */
struct astNode {enum astType type; struct list* children; void* data;};

struct astNode* ast_create(enum astType);
enum astType ast_tokenToAST(enum tokenType);