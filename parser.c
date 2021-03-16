#include "include.h"

/*
    Allocates and initializes an Abstract Syntax Tree node, with the proper type */
struct astNode* ast_create(enum astType type) {
    struct astNode* retval = (struct astNode*) malloc(sizeof(struct astNode));
    retval->type = type;
    retval->children = list_create();
    return retval;
}

/*
    Verifies that the next token is what is expected, and removes it. Errors
    otherwise */
static void assertRemove(struct list* tokenQueue, enum tokenType expected) {
    if(list_isEmpty(tokenQueue)) return;
    if(((struct token*) list_peek(tokenQueue))->type == expected) free(list_pop(tokenQueue));
    else exit(2);
}

struct astNode* parser_parseAST(struct list* tokenQueue) {
    struct astNode* astNode = NULL;
    while(!list_isEmpty(tokenQueue)) {
        if(((struct token*)list_peek(tokenQueue))->type == TOKEN_LBRACE) {
            // BLOCK
            astNode = ast_create(AST_BLOCK);
            assertRemove(tokenQueue, TOKEN_LBRACE);
            while(!(list_isEmpty(tokenQueue) || ((struct token*)list_peek(tokenQueue))->type == TOKEN_RBRACE))
                queue_push(astNode->children, parser_parseAST(tokenQueue));
            assertRemove(tokenQueue, TOKEN_RBRACE);
        } else if (((struct token*)list_peek(tokenQueue))->type == TOKEN_IF) {
            // IF
            astNode = ast_create(AST_IF);
            assertRemove(tokenQueue, TOKEN_IF);
            queue_push(astNode->children, parser_parseAST(tokenQueue));
            queue_push(astNode->children, parser_parseAST(tokenQueue));
        } else if (((struct token*)list_peek(tokenQueue))->type == TOKEN_WHILE) {
            // WHILE
            astNode = ast_create(AST_WHILE);
            assertRemove(tokenQueue, TOKEN_WHILE);
            queue_push(astNode->children, parser_parseAST(tokenQueue));
            queue_push(astNode->children, parser_parseAST(tokenQueue));
        } else if (((struct token*)list_peek(tokenQueue))->type == TOKEN_SEMIC) {
            assertRemove(tokenQueue, TOKEN_SEMIC); // will cause astNode to be NULL
        } else {
        // EXPRESSION
            // INFIX TO POSTFIX THE NEXT EXPRESSION
            struct list* expression = list_create();
            struct list* opStack = list_create();
            // Go through each token in expression token queue, rearrange to form postfix expression token queue
            while(!list_isEmpty(tokenQueue)) {
                struct token* token = ((struct token*) list_peek(tokenQueue));
                if(token->type == TOKEN_SEMIC || token->type == TOKEN_LBRACE || token->type == TOKEN_RBRACE)
                    break;

                // VALUE
                if(token->type == TOKEN_IDENT || token->type == TOKEN_NUM || token->type == TOKEN_STR) {
                    queue_push(expression, list_pop(tokenQueue));
                } else if (token->type == TOKEN_LPAREN) { // OPEN PARENTHESIS
                    stack_push(opStack, list_pop(tokenQueue));
                } else if (token->type == TOKEN_RPAREN) { // CLOSE PARENTHESIS
                    list_pop(tokenQueue);
                    // Pop all operations from opstack to astNode until original ( paren is found
                    while(!list_isEmpty(opStack) && ((struct token*)list_peek(opStack))->type != TOKEN_LPAREN) {
                        queue_push(expression, list_pop(opStack));
                    }
                    list_pop(opStack); // Remove (
                } else { // OPERATOR
                    // Pop all operations from opstack until an operation of lower precedence is found
                    while(!list_isEmpty(opStack) && token->type <= ((struct token*)list_peek(opStack))->type && ((struct token*)list_peek(opStack))->type != TOKEN_LPAREN) {
                        queue_push(expression, list_pop(opStack));
                    }
                    stack_push(opStack, list_pop(tokenQueue));
                }
            }
            // Push all remaining operators to queue
            while(!list_isEmpty(opStack))
                queue_push(expression, list_pop(opStack));
            // destroy opStack here

            // EXPRESSION TREE GENERATION
            struct list* argStack = list_create();
            while (!list_isEmpty(expression)) {
                struct token* token = (struct token*)list_pop(expression);
                struct astNode* node = ast_create(token->type);

                switch(token->type) {
                case TOKEN_NUM:
                case TOKEN_IDENT:
                case TOKEN_STR: {
                    strcpy(node->data, token->data);
                    stack_push(argStack, node);
                } break;
                default: // Assume operator
                    queue_push(node->children, list_pop(argStack)); // Right
                    if(node->type != AST_OUT && node->type != AST_IN) {
                        queue_push(node->children, list_pop(argStack)); // Left
                    }
                    stack_push(argStack, node);
                }
                free(token);
            }
            return list_peek(argStack);
            // destroy expression and argStack here
        }
    }
    return astNode;
}