OBGURL = build/
SOURCEURL = source/
CC=gcc

CFLAGS=-c -Wall -g

all: compiler

compiler: compiler.o lexer.o parser_ast.o identifier_table.o semantic_analysis.o code_generator.o
	$(CC) $(OBGURL)compiler.o $(OBGURL)lexer.o $(OBGURL)parser_ast.o $(OBGURL)identifier_table.o $(OBGURL)semantic_analysis.o $(OBGURL)code_generator.o -o compiler -lm

compiler.o: $(SOURCEURL)compiler.c
	$(CC) $(CFLAGS) $(SOURCEURL)compiler.c -o $(OBGURL)compiler.o

lexer.o: $(SOURCEURL)lexer.c
	$(CC) $(CFLAGS) $(SOURCEURL)lexer.c -o $(OBGURL)lexer.o

parser_ast.o: $(SOURCEURL)parser_ast.c
	$(CC) $(CFLAGS) $(SOURCEURL)parser_ast.c -o $(OBGURL)parser_ast.o

identifier_table.o: $(SOURCEURL)identifier_table.c
	$(CC) $(CFLAGS) $(SOURCEURL)identifier_table.c -o $(OBGURL)identifier_table.o

semantic_analysis.o: $(SOURCEURL)semantic_analysis.c
	$(CC) $(CFLAGS) $(SOURCEURL)semantic_analysis.c -o $(OBGURL)semantic_analysis.o

code_generator.o: $(SOURCEURL)code_generator.c
	$(CC) $(CFLAGS) $(SOURCEURL)code_generator.c -o $(OBGURL)code_generator.o

clean:
	rm -rf $(OBGURL)*.o compiler
