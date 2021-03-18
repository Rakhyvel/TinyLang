/*  interpreter.c

    The interpreter takes the list of AST's that the parser created, and runs
    through each one recursively to provide their functionality when running the
    program
*/

#include "include.h"

/*
    Takes in the programs variable map, a token representing a node in an AST,
    and depending on it's type, performs some kind of action. Returns the AST
    evaluation, if there is one, or 0 if not. */
static int interpretAST(struct token* node, struct map* varMap) {
    switch(node->type) {
    // LITERALS: have a set value, no children. Leaf nodes in the AST
    case IDENT: { 
        return map_get(varMap, node->data);
    } case NUM: { 
        return atoi(node->data);
    } case STR:
        return 0;
    // MATH TOKENS: Calculate their return value based on their left and right (index 1, and 0) nodes
    case PLUS: { 
        return interpretAST(list_get(node->children, 1), varMap) + interpretAST(list_get(node->children, 0),varMap);
    } case MINUS: { 
        return interpretAST(list_get(node->children, 1), varMap) - interpretAST(list_get(node->children, 0),varMap);
    } case STAR: { 
        return interpretAST(list_get(node->children, 1), varMap) * interpretAST(list_get(node->children, 0),varMap);
    } case SLASH: { 
        return interpretAST(list_get(node->children, 1), varMap) / interpretAST(list_get(node->children, 0),varMap);
    } case IS: { 
        return interpretAST(list_get(node->children, 1), varMap) == interpretAST(list_get(node->children, 0),varMap);
    } case GTR: { 
        return interpretAST(list_get(node->children, 1), varMap) > interpretAST(list_get(node->children, 0),varMap);
    } case LSR: { 
        return interpretAST(list_get(node->children, 1), varMap) < interpretAST(list_get(node->children, 0),varMap);
    } case AND: { 
        return interpretAST(list_get(node->children, 1), varMap) && interpretAST(list_get(node->children, 0),varMap);
    } case OR:  { 
        return interpretAST(list_get(node->children, 1), varMap) || interpretAST(list_get(node->children, 0),varMap);
    } 
    // IO TOKENS: Perform some sort of IO side effect before returning their children's value
    case EMARK: { 
        if(((struct token*)list_get(node->children, 0))->type == STR) {
            printf("%s\n", (char*)((struct token*)list_get(node->children, 0))->data);
        } else {
            printf("%d\n", interpretAST(list_get(node->children, 0), varMap));
        }
        return interpretAST(list_get(node->children, 0), varMap);
    } case QMARK: {
        printf("%s = ", (char*)((struct token*)list_get(node->children, 0))->data);
        int in; 
        if (scanf("%d", &in) == EOF) {
            fprintf(stderr, "Bad input\n");
            exit(6);
        }
        map_put(varMap, (char*)((struct token*)list_get(node->children, 0))->data, in);
        return interpretAST(list_get(node->children, 0), varMap);
    } 
    // STATEMENTS: Don't have a real return type. Only perform actions
    case LBRACE: {
        return interpreter_interpret(node->children, varMap);
    } case ASSIGN: { 
        map_put(varMap, node->data, interpretAST(list_get(node->children, 0), varMap));
        return map_get(varMap, node->data);
    } case IF: { 
        if(interpretAST(list_get(node->children, 0), varMap)) {
            interpretAST(list_get(node->children, 1), varMap);
        }
        return 0;
    } case WHILE: { 
        while(interpretAST(list_get(node->children, 0), varMap)) {
            interpretAST(list_get(node->children, 1), varMap);
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

/*
    Runs through a given list of AST's, performs each one. Used for the main 
    program list, and any blocks */
int interpreter_interpret(struct list* astList, struct map* varMap) {
    for (struct listElem* elem = list_begin(astList); elem != list_end(astList); elem = elem->next) {
        interpretAST(elem->data, varMap);
    }
    return 0;
}