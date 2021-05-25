/*  interpreter.c

    The interpreter takes the list of AST's that the parser created, and runs
    through each one recursively to provide their functionality when running the
    program
*/

#include "include.h"

static int memory[255];
static int nextAddr = 0;

static void interpretBlock(struct token* node, struct map* varMap) {
    for (struct listElem* elem = list_begin(node->children)->next; elem != list_end(node->children); elem = elem->next) {
        interpretAST(elem->data, varMap, true);
    }
}

/*
    Takes in the programs variable map, a token representing a node in an AST,
    and depending on it's type, performs some kind of action. Returns the AST
    evaluation, if there is one, or 0 if not. */
int interpretAST(struct token* node, struct map* varMap, bool deref) {
    switch(node->type) {
    // LITERALS: have a set value, no children. Leaf nodes in the AST
    case IDENT: {
        int addr = map_get(varMap, node->data);
        if(deref) {
            return memory[addr];
        } else {
            return addr;
        }
    } case NUM: { 
        return atoi(node->data);
    } case STR:
        return 0;
    // MATH TOKENS: Calculate their return value based on their left and right (index 1, and 0) nodes
    case PLUS: { 
        return interpretAST(list_get(node->children, 1), varMap, deref) + interpretAST(list_get(node->children, 0),varMap, deref);
    } case MINUS: {
        return interpretAST(list_get(node->children, 1), varMap, deref) - interpretAST(list_get(node->children, 0),varMap, deref);
    } case STAR: { 
        return interpretAST(list_get(node->children, 1), varMap, deref) * interpretAST(list_get(node->children, 0),varMap, deref);
    } case SLASH: { 
        return interpretAST(list_get(node->children, 1), varMap, deref) / interpretAST(list_get(node->children, 0),varMap, deref);
    } case IS: { 
        return interpretAST(list_get(node->children, 1), varMap, deref) == interpretAST(list_get(node->children, 0),varMap, deref);
    } case GTR: { 
        return interpretAST(list_get(node->children, 1), varMap, deref) > interpretAST(list_get(node->children, 0),varMap, deref);
    } case LSR: { 
        return interpretAST(list_get(node->children, 1), varMap, deref) < interpretAST(list_get(node->children, 0),varMap, deref);
    } case AND: { 
        return interpretAST(list_get(node->children, 1), varMap, deref) && interpretAST(list_get(node->children, 0),varMap, deref);
    } case OR:  { 
        return interpretAST(list_get(node->children, 1), varMap, deref) || interpretAST(list_get(node->children, 0),varMap, deref);
    } 
    // IO TOKENS: Perform some sort of IO side effect before returning their children's value
    case EMARK: { 
        if(((struct token*)list_get(node->children, 0))->type == STR) {
            printf("%s\n", (char*)((struct token*)list_get(node->children, 0))->data);
        } else {
            printf("%d\n", interpretAST(list_get(node->children, 0), varMap, true));
        }
        return interpretAST(list_get(node->children, 0), varMap, true);
    } case QMARK: {
        printf("%s = ", (char*)((struct token*)list_get(node->children, 0))->data);
        int in; 
        if (scanf("%d", &in) == EOF) {
            fprintf(stderr, "Bad input\n");
            exit(6);
        }
        map_put(varMap, (char*)((struct token*)list_get(node->children, 0))->data, in);
        return interpretAST(list_get(node->children, 0), varMap, true);
    } 
    // STATEMENTS: Don't have a real return type. Only perform actions
    case ASSIGN: {
        if(!map_contains(varMap, ((struct token*)list_get(node->children, 1))->data)) {
            map_put(varMap, ((struct token*)list_get(node->children, 1))->data, nextAddr++);
        }
        memory[interpretAST(list_get(node->children, 1), varMap, false)] = interpretAST(list_get(node->children, 0), varMap, true);
        return map_get(varMap, node->data);
    } case IF: { 
        if(interpretAST(list_get(node->children, 0), varMap, true)) {
            interpretBlock(node, varMap);
        }
        return 0;
    } case WHILE: { 
        while(interpretAST(list_get(node->children, 0), varMap, true)) {
            interpretBlock(node, varMap);
        }
        return 0;
    } case HALT: { 
        printf("Program ended.\n");
        exit(0);
    } 
    case SEMIC: default: break;
    }
    return 0;
}