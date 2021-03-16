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

enum type {
	// Literals
	IDENT, NUM, STR,
	// Variables
	EMARK, QMARK, ASSIGN, 
	// Branch operators
	AND, OR, 
	// Boolean operators
	IS, GTR, LSR,
	// Math operators
	PLUS, MINUS, STAR, SLASH,
	// Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, SEMIC,
	// Control flow structures
	IF, WHILE,
};
struct token {enum type type; char* data;};
struct astNode {enum type type; struct list* children; char data[255];};

struct astNode* parser_parseAST(struct list* tokenQueue);

void interpreter_interpret(struct list*, struct map*);

#endif