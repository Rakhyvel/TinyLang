/*
    Date: 3/13/21
    Author: Joseph Shimel
*/

#include "include.h"

static const char* tokenMap[] = {"!", "?", ":", "and", "or", "=", ">", "<", "+", "-", "*", "/",  "(", ")", "{", "}", "\n", "if", "while", "halt"};

/*
    The main function opens the specified file, turns the text into a list of 
    tokens, creates the list of AST's, and then finally interprets that list
    into as a program */
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
        struct token* token = (struct token*) malloc(sizeof(struct token));
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
        strncpy(token->data, file + start, end-start);

        // Find token type
        for(token->type = 3; token->type <= 22 && strcmp(tokenMap[token->type - 3], token->data); token->type++);
        if(token->type == 23) {
            if(isdigit(token->data[0])) {
                token->type = NUM;
            } else if(token->data[0] == '"'){
                token->type = STR;
                int i;
                for (i = 1; token->data[i] != '"'; i++)
                    token->data[i-1] = token->data[i];
                token->data[i-1] = '\0';
            } else token->type = IDENT;
        }

        // Create token, and add to list
        token->children = list_create();
        queue_push(tokenQueue, token);

        // Advance pointer to next non-whitespace token
        for (char nextChar = file[end]; nextChar != '\0' && isspace(nextChar) && nextChar != '\n'; nextChar = file[++end]);
        start = end;
    } while(end < fileLength);

    // parse tokenQueue into program (list of ASTs and a symbol map)
    struct list* program = list_create();
    while(!list_isEmpty(tokenQueue))
        queue_push(program, parser_parseAST(tokenQueue));

    // interpret program
    interpreter_interpret(program, map_create());
}

struct token* parser_parseAST(struct list* tokenQueue) {
    struct token* node = ((struct token*) list_peek(tokenQueue));
    if(node->type == LBRACE) {
        list_pop(tokenQueue);
        while(!(list_isEmpty(tokenQueue) || ((struct token*) list_peek(tokenQueue))->type == RBRACE))
            queue_push(node->children, parser_parseAST(tokenQueue));
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
        for(struct token* token = ((struct token*) list_peek(tokenQueue)); token->type != SEMIC && token->type != LBRACE && token->type != RBRACE; token = ((struct token*) list_peek(tokenQueue))) {
            // VALUE
            if(token->type == IDENT || token->type == NUM || token->type == STR) {
                queue_push(expression, list_pop(tokenQueue));
            } else if (token->type == LPAREN) { // OPEN PARENTHESIS
                stack_push(opStack, list_pop(tokenQueue));
            } else if (token->type == RPAREN) { // CLOSE PARENTHESIS
                list_pop(tokenQueue);
                // Pop all operations from opstack to astNode until original ( paren is found
                while(!list_isEmpty(opStack) && ((struct token*)list_peek(opStack))->type != LPAREN)
                    queue_push(expression, list_pop(opStack));
                list_pop(opStack); // Remove (
            } else { // OPERATOR
                // Pop all operations from opstack until an operation of lower precedence is found
                while(!list_isEmpty(opStack) && token->type <= ((struct token*)list_peek(opStack))->type && ((struct token*)list_peek(opStack))->type != LPAREN)
                    queue_push(expression, list_pop(opStack));
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

            switch(token->type) {
            default: // Assume operator
                queue_push(token->children, list_pop(argStack)); // Right
                if(token->type != EMARK && token->type != QMARK)
                    queue_push(token->children, list_pop(argStack)); // Left
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

static int interpretAST(struct token* node, struct map* varMap) {
    switch(node->type) {
    case IDENT: { 
        return map_get(varMap, node->data);
    } case NUM: { 
        return atoi(node->data);
    } case STR:
        return 0; { 
	} case PLUS: { 
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
    } case EMARK: { 
        if(((struct token*)list_get(node->children, 0))->type == STR) {
            printf("%s\n", (char*)((struct token*)list_get(node->children, 0))->data);
        } else
            printf("%d\n", interpretAST(list_get(node->children, 0), varMap));
        return interpretAST(list_get(node->children, 0), varMap);
    } break; case QMARK: {
        printf("%s = ", (char*)((struct token*)list_get(node->children, 0))->data);
        int in; if (scanf("%d", &in) == EOF) {exit(6);}
        map_put(varMap, (char*)((struct token*)list_get(node->children, 0))->data, in);
        return interpretAST(list_get(node->children, 0), varMap);
    }break; case LBRACE: {
        interpreter_interpret(node->children, varMap);
    }break; case ASSIGN: { 
        map_put(varMap, node->data, interpretAST(list_get(node->children, 0), varMap));
    } break; case IF: { 
        if(interpretAST(list_get(node->children, 0), varMap))
            interpretAST(list_get(node->children, 1), varMap);
    } break; case WHILE: { 
        while(interpretAST(list_get(node->children, 0), varMap))
            interpretAST(list_get(node->children, 1), varMap);
    } break; case HALT: { 
        printf("Program ended.\n");
        exit(0);
    } break; case SEMIC: default: break;}
    return 0;
}

void interpreter_interpret(struct list* astList, struct map* varMap) {
    for (struct listElem* elem = list_begin(astList); elem != list_end(astList); elem = elem->next) 
        interpretAST(elem->data, varMap);
}