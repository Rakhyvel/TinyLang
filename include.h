/*	include.h

	.h files in C are used to share declartions of variables, structs, 
	functions, and enums across a project.	In this header file, I list some
	useful functions, enums, and structs that we use throughout the project
*/

// this is called an include gaurd
// it makes sure that this file is only ever included once
#ifndef COMMON_H
#define COMMON_H

// import some standard libraries to use
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// list.c declarations
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

// map.c declarations
struct mapNode {char* key; int value; struct mapNode* next;};
struct map {struct mapNode** lists;};
struct map* map_create();
void map_put(struct map*, char*, int);
int map_get(struct map*, const char*);

// token declarations
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
    LPAREN, RPAREN, SEMIC,
	// Control flow structures
	IF, WHILE, END, HALT, 
};
struct token {enum type type; struct list* children; char data[255];};

// lexer.c declarations
struct token* nextToken(FILE* fileptr);

// parser.c declarations
struct token* parser_parseAST(FILE* fileptr);

// interpreter.c declarations
int interpretAST(struct token* node, struct map* varMap, bool deref);

#endif