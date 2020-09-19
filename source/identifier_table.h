#ifndef IDENTIFIER_TABLE_H
#define IDENTIFIER_TABLE_H 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "lexer.h"
#include "parser_ast.h"  

#define TABLE_SIZE 256

typedef enum {
	_Integer,
	_String,
	_Array
} IdentifierType;

typedef enum {
	__Integer,
	__String,
	__Array
} IdentifierMachineType;

typedef enum {
	Quick,
	Slow	
} IdentifierMemoryType;

struct IdentifierAttributes {
	char Name[32];
	int Volatile;						//Boolean
	int Size_Bytes;
	int Alignment_Bytes;
	IdentifierType Type;
	IdentifierType BaseType;
	int AmountOfElements;
	int Bias;
	int Initialized;					//Boolean
};

typedef struct _Node {
    struct _Node *Next;
    struct IdentifierAttributes *Data;
} HashNode;

int HashFunction(char *Name);
HashNode *InsertNode(char *Name, struct IdentifierAttributes *INode, HashNode **Table);
HashNode *FindNode(char *Name, HashNode **Table);
HashNode **CreateHashTable(struct AstNode *Root);
void PrintIdentifierAttributes(struct IdentifierAttributes *Attributes);
void PrintHashTable(HashNode **Table);

#endif
