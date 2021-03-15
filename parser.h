#ifndef PARSER_H
#define PARSER_H

#include "common.h"

struct astNode* parser_parseAST(struct list* tokenQueue);

#endif