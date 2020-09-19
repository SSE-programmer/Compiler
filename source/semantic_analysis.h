#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "lexer.h"
#include "parser_ast.h"  
#include "identifier_table.h"  


VariableType CheckingVariableType(struct AstNode *TopOperationNode, HashNode **Table, struct AstNode *RootLink);
int CheckingConstants(struct AstNode *ListVariable, HashNode **Table, struct AstNode *RootLink);
int CheckingCommandExpression(struct AstNode *BodyNode, HashNode **Table, struct AstNode *RootLink);

#endif