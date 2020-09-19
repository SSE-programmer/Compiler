#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "parser_ast.h"  
#include "identifier_table.h"

#define ASM_SYMBOL_COUNT 9
#define ASM_OR 0
#define ASM_AND 1
#define ASM_LESS 2
#define ASM_LOOP 3
#define ASM_EQUAL 4
#define ASM_BRANCH 5
#define ASM_LARGER 6
#define ASM_NOT_EQUAL 7
#define ASM_LESS_OR_EQUAL 8
#define ASM_LARGER_OR_EQUAL 9




static int Counter[ASM_SYMBOL_COUNT];

int AsmRodataSection(FILE *FAsm);
int AsmCodeGenerator(struct AstNode *AstTree, HashNode **IdentifierTable, int FileOut);
int AsmConstantDeclaration(FILE *FAsm, struct AstNode *AstTree, HashNode **IdentifierTable);
int AsmVariableDeclaration(FILE * FAsm, struct AstNode *AstTree, HashNode **IdentifierTable);
int AsmBodyCommands(FILE *FAsm, struct AstNode *BodyNode, HashNode **IdentifierTable);
int EmitExpression(FILE *FAsm, struct AstNode *ExpressionNode, HashNode **IdentifierTable);

#endif
