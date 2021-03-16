#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct listElem {struct listElem* next; struct listElem* prev; void* data;};
struct list {struct listElem head;struct listElem tail;};
struct list* list_create();
struct listElem* list_begin(struct list*);
struct listElem* list_end(struct list*);
void list_insert(struct list*, struct listElem*, void*);
void* list_get(struct list*, int);
void* list_pop(struct list*);
void* list_peek(struct list*);
void queue_push(struct list*, void*);
void stack_push(struct list*, void*);
int list_isEmpty(struct list*);


struct mapNode {char* key; int value; struct mapNode* next;};
struct map {struct mapNode** lists;};
struct map* map_create();
void map_put(struct map*, char*, int);
int map_get(struct map*, const char*);

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
struct token {enum tokenType type; char* data;};
struct astNode {enum astType type; struct list* children; char data[255];};

struct astNode* parser_parseAST(struct list* tokenQueue);

void interpreter_interpret(struct list*, struct map*);

#endif