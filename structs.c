#include "common.h"

/*
    Allocates and initializes an Abstract Syntax Tree node, with the proper type */
struct astNode* ast_create(enum astType type) {
    struct astNode* retval = (struct astNode*) malloc(sizeof(struct astNode));
    retval->type = type;
    retval->children = list_create();
    return retval;
}