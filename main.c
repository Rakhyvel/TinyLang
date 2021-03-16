/*
    Date: 3/13/21
    Author: Joseph Shimel
*/

#include "include.h"

enum tokenState {BEGIN, TEXT, STRING};
static const char* tokenMap[] = {"!", "?", ":", "and", "or", "=", ">", "<", "+", "-", "*", "/",  "(", ")", "{", "}", "\n", "if", "while"};

int main(int argc, char **argv) {
    // open file
    if(argc < 2) exit(1);

    char* file = malloc(sizeof(char));
    /* Open file and read in characters to an ever growing string */ {
        FILE* fileptr = fopen(argv[1], "r");
        int buffer;
        for (int size = 1; (buffer = fgetc(fileptr)) != EOF; size++) {
            file = realloc(file, size + 1);
            file[size - 1] = (char)buffer;  // Convert int buffer to char, append
            file[size] = '\0'; // Add the sentinel value
        }
    }

    struct list* tokenQueue = list_create();
    int start = 0, end = 0, fileLength = strlen(file);
    /* Create tokens from text, add them to a token queue */ do {
        char* tokenBuffer = calloc(255, sizeof(char));
        // Find the end of the next token
        enum tokenState state = BEGIN;
        for(char nextChar = file[end]; nextChar != '\0'; end++, nextChar = file[end])
            if(state == BEGIN) {
                if(isalnum(nextChar)) {
                    state = TEXT;
                } else if(nextChar == '"') {
                    state = STRING;
                } else if (nextChar == '\n' || (!isdigit(nextChar) && !isalpha(nextChar) && !isspace(nextChar))){
                    end++;
                    break;
                }
            } else if(state == TEXT && (!isalnum(nextChar) && nextChar != '_')) {
                break;
            } else if (state == STRING && nextChar == '"') {
                end++;
                break;
            }
        strncpy(tokenBuffer, file + start, end-start);

        // Find token type
        enum tokenType tempType;
        for(tempType = 3; tempType <= 19 && strcmp(tokenMap[tempType - 3], tokenBuffer); tempType++);
        if(tempType == 20) {
            if(isdigit(tokenBuffer[0])) {
                tempType = TOKEN_NUM;
            } else if(tokenBuffer[0] == '"'){
                tempType = TOKEN_STR;
                for (int i = 1; tokenBuffer[i - 1] != '"'; i++)
                    tokenBuffer[i - 1] = tokenBuffer[i];
            } else tempType = TOKEN_IDENT;
        }

        // Create token, and add to list
        struct token* token = (struct token*) malloc(sizeof(struct token));
        token->type = tempType;
        token->data = tokenBuffer;
        queue_push(tokenQueue, token);

        // Advance pointer to next non-whitespace token
        for (char nextChar = file[end]; nextChar != '\0' && isspace(nextChar) && nextChar != '\n'; nextChar = file[++end]);
        start = end;
    } while(end < fileLength);

    // parse tokenQueue into program (list of ASTs and a symbol map)
    struct list* program = list_create();
    while(!list_isEmpty(tokenQueue)) {
        queue_push(program, parser_parseAST(tokenQueue));
    }

    // interpret program
    interpreter_interpret(program, map_create());
}

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

void interpreter_interpret(struct list* astList, struct map* varMap) {
    for (struct listElem* elem = list_begin(astList); elem != list_end(astList); elem = elem->next) 
        interpretAST(elem->data, varMap);
}