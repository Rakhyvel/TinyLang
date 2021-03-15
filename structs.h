#ifndef STRUCTS_H
#define STRUCTS_H

enum tokenType {
	// Literals
	TOKEN_IDENT, TOKEN_NUM, TOKEN_STR,
	// Variables
	TOKEN_EMARK, TOKEN_QMARK, TOKEN_ASSIGN, 
	// Branch operators
	TOKEN_AND, TOKEN_OR, 
	// Boolean operators
	TOKEN_IS, TOKEN_GTR, TOKEN_LSR,
	// Math operators
	TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
	// Punctuation
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMIC,
	// Control flow structures
	TOKEN_IF, TOKEN_WHILE,
};

/*
    AST nodes have types that tell them apart */
enum astType {
	// Literals
    AST_IDENT, AST_NUM, AST_STR,
    // Variable operations
    AST_OUT, AST_IN, AST_ASSIGN, 
    // Boolean operators
    AST_AND, AST_OR,
    // Branch operators
    AST_IS, AST_GTR, AST_LSR,
    // Math operators
	AST_ADD, AST_SUB, AST_MULT, AST_DIV, 
    // StatementNode types
	AST_BLOCK, AST_IF, AST_WHILE,
    // Anonymous
    AST_NOP
};

struct token {enum tokenType type; char data[255];};

struct astNode {enum astType type; struct list* children; char data[255];};

struct astNode* ast_create(enum astType);

#endif