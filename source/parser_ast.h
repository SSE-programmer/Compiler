#ifndef PARSER_AST_H
#define PARSER_AST_H 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "lexer.h"

typedef enum {
	_UNARY_MINUS, _AND, _MULT, _DIV, _MOD, _OR, _PLUS, _MINUS, _EQUAL, _NOT_EQUAL, _LESS, _LARGER, _LESS_OR_EQUAL, _LARGER_OR_EQUAL 
	} Operation;

typedef enum {
	AstNodeRoot,
	AstNodeBody,
	AstNodeDeclaration,
	AstNodeExpression,
	AstNodeCompare,
	AstNodeAssign,
	AstNodeWhile,
	AstNodeBranch,
	AstNodeRead,
	AstNodeWrite,
	AstNodeVariable,
	AstNodeConstant,
	AstNodeVoid
} AstNodeType;

typedef enum {
	Integer,
	String,
	Constant
} VariableType;


struct _ParamVariable {
	char Name[32];
	int  First, Last, ArrayFlag, Length, StringSize;
	VariableType VarType;
	struct AstNode *ExpressionLink;
};

struct _Root {
	struct AstNode *DeclarationLink;
	struct AstNode *BodyLink;
	int ErrorCount;
};

struct _Body {
	int CommandCount;
	struct AstNode *ParentLink;
	struct AstNode *CommandList;
};

struct _Declaration { 
	int VariableCount;
	struct AstNode *RootLink;
	struct AstNode *ListVariable;	//Динамический массив переменных; 
											//Добавляем элементы при помощи функции void* realloc (void* ptr, size_t size);
};

struct _Expression {
	struct AstNode *ParentLink;
	struct AstNode *LeftLink;
	struct AstNode *RightLink;
	Operation Operation;
	VariableType ResultType;
};

struct _Assign {
	struct AstNode *ParentLink;
	struct AstNode *VariableLink;
	struct AstNode *RightLink;
};

struct _While {
	struct AstNode *ParentLink;
	struct AstNode *ConditionLink;
	struct AstNode *BodyLink;
};

struct _Branch {
	struct AstNode *ParentLink;
	struct AstNode *ConditionLink;
	struct AstNode *IfBodyLink;
	struct AstNode *ElseBodyLink;
};

struct _Read {
	struct AstNode *ParentLink;
	int VariableCount;
	struct _Variable *ListVariable;
};

struct _Write {
	struct AstNode *ParentLink;
	struct AstNode *ExpressionList;
	int ArgCount;
};

struct _Variable {
	struct AstNode *ParentLink;
	char Name[32];
	struct AstNode *OffsetExpression;
};

struct _Constant {
	struct AstNode *ParentLink;
	char Value[256];
	VariableType Type;
};


struct AstNode {
	union {
		struct _Root Root;
		struct _Body Body;
		struct _Declaration Declaration;
		struct _Expression Expression;
		struct _Assign Assign;
		struct _While While;
		struct _Branch Branch;
		struct _Read Read;
		struct _Write Write;
		struct _Variable Variable;
		struct _Constant Constant;
		struct _ParamVariable ParamVariable;
	};
	AstNodeType Type;
	int X, Y;
};

struct ReturnStruct {
	struct AstNode *ReturnNode;
	struct token LastToken;
};

void reverse(char s[]);
void itoa(int n, char s[]);

struct token DecimalValue(struct token Token);

int ErrorRecovery(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct token *Token, struct ReturnStruct *ReturnStruct);

struct AstNode * ParseProgram(char * path);
struct ReturnStruct * ParseBody(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *ParentLink, struct AstNode *RootLink);
struct ReturnStruct * ParseDeclarationVar(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *RootLink);
struct ReturnStruct * ParseDeclarationConst(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *RootLink);
struct ReturnStruct * ParseExpression(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *ParentLink, int ParenFlag, TClass Separator, struct AstNode *RootLink);

void PrintAstTree(struct AstNode *Node, int Level);
void FreeAstTree(struct AstNode *Node);

#endif
