#include "common.h"

static int interpretAST(struct astNode* node, struct map* varMap);

void interpreter_interpret(struct list* astList, struct map* varMap) {
    for (struct listElem* elem = list_begin(astList); elem != list_end(astList); elem = elem->next) 
        interpretAST(elem->data, varMap);
}

static int interpretAST(struct astNode* node, struct map* varMap) {
    if(node==NULL) return 0;

    switch(node->type) {
    case AST_IDENT: { 
        return map_get(varMap, node->data);
    } case AST_NUM: { 
        return atoi(node->data);
    } case AST_STR:
        return 0; { 
	} case AST_ADD: { 
        return interpretAST(list_get(node->children, 1), varMap) + interpretAST(list_get(node->children, 0),varMap);
    } case AST_SUB: { 
        return interpretAST(list_get(node->children, 1), varMap) - interpretAST(list_get(node->children, 0),varMap);
    } case AST_MULT: { 
        return interpretAST(list_get(node->children, 1), varMap) * interpretAST(list_get(node->children, 0),varMap);
    } case AST_DIV: { 
        return interpretAST(list_get(node->children, 1), varMap) / interpretAST(list_get(node->children, 0),varMap);
    } case AST_IS: { 
        return interpretAST(list_get(node->children, 1), varMap) == interpretAST(list_get(node->children, 0),varMap);
    } case AST_GTR: { 
        return interpretAST(list_get(node->children, 1), varMap) > interpretAST(list_get(node->children, 0),varMap);
    } case AST_LSR: { 
        return interpretAST(list_get(node->children, 1), varMap) < interpretAST(list_get(node->children, 0),varMap);
    } case AST_AND: { 
        return interpretAST(list_get(node->children, 1), varMap) && interpretAST(list_get(node->children, 0),varMap);
    } case AST_OR:  { 
        return interpretAST(list_get(node->children, 1), varMap) || interpretAST(list_get(node->children, 0),varMap);
    } case AST_OUT: { 
        if(((struct astNode*)list_get(node->children, 0))->type == AST_STR) {
            printf("%s\n", (char*)((struct astNode*)list_get(node->children, 0))->data);
        } else
            printf("%d\n", interpretAST(list_get(node->children, 0), varMap));
    } break; case AST_IN: {
        printf("%s = ", (char*)((struct astNode*)list_get(node->children, 0))->data);
        int in; if (scanf("%d", &in) == EOF) {exit(6);}
        map_put(varMap, (char*)((struct astNode*)list_get(node->children, 0))->data, in);
    }break; case AST_BLOCK: {
        interpreter_interpret(node->children, varMap);
    }break; case AST_ASSIGN: { 
        map_put(varMap, node->data, interpretAST(list_get(node->children, 0), varMap));
    } break; case AST_IF: { 
        if(interpretAST(list_get(node->children, 0), varMap))
            interpretAST(list_get(node->children, 1), varMap);
    } break; case AST_WHILE: { 
        while(interpretAST(list_get(node->children, 0), varMap))
            interpretAST(list_get(node->children, 1), varMap);
    } break; default: break;}
    return 0;
}