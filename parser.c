/*  parser.c

    The parser takes the token stream that the lexer created, and creates Abstract
    Syntax Trees out of them. AST's allow the interpreter to easily and 
    recursively perform the actions in the program. They also make expression
    evaluation not just possible, but really easy.
*/

#include "include.h"

/*
    Takes in a queue of tokens, returns the first AST node that can be found */
struct token* parser_parseAST(FILE* fileptr) {
    struct token* node = ((struct token*) nextToken(fileptr));
     if (node->type == IF || node->type == WHILE) {
        queue_push(node->children, parser_parseAST(fileptr));
        for(struct token* child; (child = parser_parseAST(fileptr))->type != END; queue_push(node->children, child)); // push all children to if/while child list
    } else if (node->type != HALT && node->type != SEMIC && node->type != END) {
    // EXPRESSION
        // INFIX TO POSTFIX THE NEXT EXPRESSION
        struct list* expression = list_create();
        struct list* opStack = list_create();
        struct token* token = node;
        // Go through each token in expression token queue, rearrange to form postfix expression token queue
        for(;strlen(token->data) > 0 && token->type != SEMIC && token->type != END; 
            token = ((struct token*) nextToken(fileptr))) {
            // VALUE
            if(token->type == IDENT || token->type == NUM || token->type == STR) {
                queue_push(expression, token);
            } else if (token->type == LPAREN) { // OPEN PARENTHESIS
                stack_push(opStack, token);
            } else if (token->type == RPAREN) { // CLOSE PARENTHESIS
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
                stack_push(opStack, token);
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
            token = (struct token*)list_pop(expression);
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