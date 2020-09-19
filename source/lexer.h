#ifndef LEXER_H
#define LEXER_H 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define N_ID 20
#define LEN_ID 10

#define N_TSM 48
#define LEN_TSM 20

extern char TypeStringMass[N_TSM][LEN_TSM];

typedef enum {
	ASSIGMENT, BEGIN, END, IF, THEN, ELSE, WHILE, DO, AND, OR, CONST, VAR, ARRAY, OF, PLUS, MINUS, MULT,
    DIV, MOD, INTEGER, STRING, STRING_LITERAL, L_PAREN, R_PAREN, COMMENT, SEMI, COLON, POINT, COMMA, _EOF,
    UNKNOWN, EQUAL, NOT_EQUAL, LESS, LARGER, LESS_OR_EQUAL, LARGER_OR_EQUAL, NUM_2, NUM_8, NUM_10, NUM_16,
    IDENTIFIER, L_BRACE, R_BRACE, ELLIPSIS, READLN, WRITELN } TClass;

struct stream_cursor_position {
    short X;
    short Y;
};

struct token {
    unsigned short X;
    unsigned short Y;
    TClass Class;
    char Lexeme[256];
};

FILE *open_program_file(const char *name, struct stream_cursor_position *stream_cursor_position);
int close_program_file(FILE *name);
struct token next_token(FILE *source_code, struct stream_cursor_position *stream_cursor_position);
struct token NextToken(FILE *source_code, struct stream_cursor_position *stream_cursor_position);
int print_token(struct token token); 

#endif


