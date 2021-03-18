/*  parser.c

    The parser takes the token stream that the lexer created, and creates Abstract
    Syntax Trees out of them. AST's allow the interpreter to easily and 
    recursively perform the actions in the program. They also make expression
    evaluation not just possible, but really easy.
*/

#include "include.h"

/*
    Takes in a queue of tokens, returns the first AST node that can be found */
struct token* parser_parseAST(struct list* tokenQueue) {
    struct token* node = ((struct token*) list_peek(tokenQueue));
    if(node->type == LBRACE) {
        list_pop(tokenQueue);
        while(!(list_isEmpty(tokenQueue) || ((struct token*) list_peek(tokenQueue))->type == RBRACE)) {
            queue_push(node->children, parser_parseAST(tokenQueue));
        }
        free(list_pop(tokenQueue));
    } else if (node->type == IF || node->type == WHILE) {
        list_pop(tokenQueue);
        queue_push(node->children, parser_parseAST(tokenQueue));
        queue_push(node->children, parser_parseAST(tokenQueue));
    } else if (node->type == HALT || node->type == SEMIC) {
        list_pop(tokenQueue);
    } else {
    // EXPRESSION
        // INFIX TO POSTFIX THE NEXT EXPRESSION
        struct list* expression = list_create();
        struct list* opStack = list_create();
        // Go through each token in expression token queue, rearrange to form postfix expression token queue
        for(struct token* token = ((struct token*) list_peek(tokenQueue)); 
            !list_isEmpty(tokenQueue) && token->type != SEMIC && token->type != LBRACE && token->type != RBRACE; 
            token = ((struct token*) list_peek(tokenQueue))) {
            // VALUE
            if(token->type == IDENT || token->type == NUM || token->type == STR) {
                queue_push(expression, list_pop(tokenQueue));
            } else if (token->type == LPAREN) { // OPEN PARENTHESIS
                stack_push(opStack, list_pop(tokenQueue));
            } else if (token->type == RPAREN) { // CLOSE PARENTHESIS
                list_pop(tokenQueue);
                // Pop all operations from opstack to astNode until original ( paren is found
                while(!list_isEmpty(opStack) && ((struct token*)list_peek(opStack))->type != LPAREN) {
                    queue_push(expression, list_pop(opStack));
                }
                list_pop(opStack); // Remove (
            } else { // OPERATOR
                // Pop all operations from opstack until an operation of lower precedence is found
                while(!list_isEmpty(opStack) && token->type <= ((struct token*)list_peek(opStack))->type && ((struct token*)list_peek(opStack))->type != LPAREN) {
                    queue_push(expression, list_pop(opStack));
                }
                stack_push(opStack, list_pop(tokenQueue));
            }
        }
        // Push all remaining operators to queue
        while(!list_isEmpty(opStack)) {
            queue_push(expression, list_pop(opStack));
        }
        // destroy opStack here

        // EXPRESSION TREE GENERATION
        struct list* argStack = list_create();
        while (!list_isEmpty(expression)) {
            struct token* token = (struct token*)list_pop(expression);

            switch(token->type) {
            default: // Assume operator
                queue_push(token->children, list_pop(argStack)); // Right child
                if(token->type != EMARK && token->type != QMARK) {
                    queue_push(token->children, list_pop(argStack)); // Left child (sometimes!)
                }
            case NUM:
            case IDENT:
            case STR: 
                stack_push(argStack, token);
            }
        }
        node = list_peek(argStack);
        // destroy expression and argStack here
    }
    return node;
}