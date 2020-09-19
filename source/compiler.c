#include "lexer.h"
#include "parser_ast.h"
#include "identifier_table.h"
#include "semantic_analysis.h"
#include "code_generator.h"

#define MAX_TOKENS 1000

struct token token_mass[MAX_TOKENS];
short token_index = 0;

int menu(int n_param, char **param);
int print_tokens(char *path);
int lexer(char *path);

int main(int argc, char **argv)
{

	menu(argc, argv);

	return 0;
}


int menu(int n_param, char **param)
{
	switch (n_param)
	{
	case 1:
		printf("Не указан файл!!!\n");
		return -2;
	case 2:
		if (!strcasecmp("--help", param[1]))
		{
			printf("Формат запуска программы:\n");
			printf("\tcompiler [Options] <input_file>\n");
			printf("Все допустимые параметры:\n");
			printf("\t--dump-tokens — вывести результат работы лексического анализатора\n");
			printf("\t--dump-ast — вывести AST\n");
			printf("\t--dump-asm — вывести ассемблер\n");
			printf("\t--help — вывести все допустимые параметры\n");
		}
		else if (strcasecmp("--dump-tokens", param[1]) && strcasecmp("--dump-ast", param[1]) && strcasecmp("--dump-asm", param[1]))
		{
			struct AstNode *AstTree = NULL;
			HashNode **IdentifierTable = NULL;

			AstTree = ParseProgram(param[n_param - 1]);

			if (AstTree != NULL)
			{
				if (AstTree->Root.ErrorCount == 0)
				{
					IdentifierTable = CreateHashTable(AstTree);

					if (AstTree->Root.DeclarationLink != NULL)
					{
						CheckingConstants(AstTree->Root.DeclarationLink, IdentifierTable, AstTree);
					}

					CheckingCommandExpression(AstTree->Root.BodyLink, IdentifierTable, AstTree);

					if (AstTree->Root.ErrorCount == 0)
					{
						AsmCodeGenerator(AstTree, IdentifierTable, 1);
						system("gcc -no-pie -g assembler/assembler.s -m32 -o assembler/assembler");
						system("./assembler/assembler");

					}
					else
					{
						printf("\033[01;31m<----------------   Всего семантических ошибок: %d   ---------------->\033[0m\n", AstTree->Root.ErrorCount);
					}
				}
				else
				{
					printf("\033[01;31m<----------------   Всего семантических ошибок: %d   ---------------->\033[0m\n", AstTree->Root.ErrorCount);
				}
			}
		}
		else
		{
			printf("Не указан файл!!!\n");
			return -2;
		}
		break;
	case 3:
		if (!strcasecmp("--dump-tokens", param[1]))
		{
			print_tokens(param[n_param - 1]);
			break;
		}
		else if (!strcasecmp("--dump-ast", param[1]))
		{
			struct AstNode *AstTree = NULL;
			HashNode **IdentifierTable = NULL;

			AstTree = ParseProgram(param[n_param - 1]);

			if (AstTree != NULL)
			{
				IdentifierTable = CreateHashTable(AstTree);
			}

			if (AstTree != NULL && AstTree->Root.ErrorCount > 0)
			{
				printf("\033[01;31m<----------------   Всего синтаксических ошибок: %d   ---------------->\033[0m\n", AstTree->Root.ErrorCount);
			}
			else
			{
				if (AstTree->Root.DeclarationLink != NULL)
				{
					CheckingConstants(AstTree->Root.DeclarationLink, IdentifierTable, AstTree);
				}

				CheckingCommandExpression(AstTree->Root.BodyLink, IdentifierTable, AstTree);

				if (AstTree != NULL && AstTree->Root.ErrorCount > 0)
				{
					printf("\033[01;31m<----------------   Всего семантических ошибок: %d   ---------------->\033[0m\n", AstTree->Root.ErrorCount);
				}
				else if (AstTree != NULL)
				{
					PrintAstTree(AstTree, 0);
				}
			}

			printf("\n");

			break;
		}
		else if (!strcasecmp("--dump-asm", param[1]))
		{
			struct AstNode *AstTree = NULL;
			HashNode **IdentifierTable = NULL;

			AstTree = ParseProgram(param[n_param - 1]);

			if (AstTree != NULL)
			{
				IdentifierTable = CreateHashTable(AstTree);
			}

			if (AstTree != NULL && AstTree->Root.ErrorCount > 0)
			{
				printf("\033[01;31m<----------------   Всего синтаксических ошибок: %d   ---------------->\033[0m\n", AstTree->Root.ErrorCount);
			}
			else
			{
				if (AstTree->Root.DeclarationLink != NULL)
				{
					CheckingConstants(AstTree->Root.DeclarationLink, IdentifierTable, AstTree);
				}

				CheckingCommandExpression(AstTree->Root.BodyLink, IdentifierTable, AstTree);

				if (AstTree != NULL && AstTree->Root.ErrorCount > 0)
				{
					printf("\033[01;31m<----------------   Всего семантических ошибок: %d   ---------------->\033[0m\n", AstTree->Root.ErrorCount);
				}
				else
				{
					AsmCodeGenerator(AstTree, IdentifierTable, 0);
				}
				
			}

			printf("\n");
			break;
		}
		else
		{
			printf("Указан неверный параметр!!!\n");
			printf("Для вывода всех доступных параметров запустите программу с параметром --help\n");
			return -3;
		}
	default:
		printf("Указано слишком много параметров!!!\n");
		return -2;
	}

	return 0;
}

int print_tokens(char *path)
{
	struct token the_token;
	struct stream_cursor_position stream_cursor_position;

	FILE *source_code = open_program_file(path, &stream_cursor_position);

	do
	{
		print_token(the_token = next_token(source_code, &stream_cursor_position));
	} while (the_token.Class != _EOF);

	close_program_file(source_code);

	return 0;
}

int lexer(char *path)
{
	token_index = 0;

	struct stream_cursor_position stream_cursor_position;

	FILE *source_code = open_program_file(path, &stream_cursor_position);

	do
	{
		token_mass[token_index] = next_token(source_code, &stream_cursor_position);
		token_index++;
	} while (token_mass[token_index - 1].Class != _EOF && token_index < MAX_TOKENS);
	close_program_file(source_code);
	return 0;
}
