#include "parser_ast.h"

/*								ПРИОРИТЕТ ОПЕРАЦИЙ:
				Унарный минус				-
				Операции типа умножения:	* div  mod  and
				Операции типа сложения:  	+  -  or
				Операции отношения:			=  <>  <  >  <=  >=

*/

void reverse(char s[])
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void itoa(int n, char s[])
{
	int i, sign;

	if ((sign = n) < 0)
		n = -n;			
	i = 0;
	do
	{
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

struct token DecimalValue(struct token Token)
{
	int DecNum = 0;

	switch (Token.Class)
	{
	case NUM_2:
		for (int i = 1; i < strlen(Token.Lexeme); i++)
		{
			DecNum += pow(2, strlen(Token.Lexeme) - i - 1) * (Token.Lexeme[i] - '0');
		}

		Token.Class = NUM_10;
		itoa(DecNum, Token.Lexeme);

		break;

	case NUM_8:
		for (int i = 1; i < strlen(Token.Lexeme); i++)
		{

			DecNum += pow(8, strlen(Token.Lexeme) - i - 1) * (Token.Lexeme[i] - '0');
		}

		Token.Class = NUM_10;
		itoa(DecNum, Token.Lexeme);

		break;

	case NUM_16:
		for (int i = 1; i < strlen(Token.Lexeme); i++)
		{
			int HexToDec = 0;

			switch (Token.Lexeme[i])
			{
			case 'a':
				HexToDec = 10;
				break;

			case 'b':
				HexToDec = 11;
				break;

			case 'c':
				HexToDec = 12;
				break;

			case 'd':
				HexToDec = 13;
				break;

			case 'e':
				HexToDec = 14;
				break;

			case 'f':
				HexToDec = 15;
				break;

			case 'A':
				HexToDec = 10;
				break;

			case 'B':
				HexToDec = 11;
				break;

			case 'C':
				HexToDec = 12;
				break;

			case 'D':
				HexToDec = 13;
				break;

			case 'E':
				HexToDec = 14;
				break;

			case 'F':
				HexToDec = 15;
				break;

			default:
				HexToDec = Token.Lexeme[i] - '0';
				break;
			}

			DecNum += pow(16, strlen(Token.Lexeme) - i - 1) * HexToDec;
		}

		Token.Class = NUM_10;
		itoa(DecNum, Token.Lexeme);

		break;

	default:
		break;
	}

	return Token;
}

int ErrorRecovery(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct token *Token, struct ReturnStruct *ReturnStruct)
{
	while (Token->Class != SEMI && Token->Class != _EOF)
	{
		*Token = NextToken(source_code, stream_cursor_position);
		ReturnStruct->LastToken = *Token;
	}

	*Token = NextToken(source_code, stream_cursor_position);
	ReturnStruct->LastToken = *Token;

	return 0;
}

struct AstNode *ParseProgram(char *path)
{

	struct token Token;
	struct ReturnStruct *ReturnStruct = NULL;

	struct stream_cursor_position stream_cursor_position;
	FILE *source_code = open_program_file(path, &stream_cursor_position);

	struct AstNode *Node = malloc(sizeof(struct AstNode));

	Node->Type = AstNodeRoot;
	Node->Root.DeclarationLink = NULL;
	Node->Root.BodyLink = NULL;
	Node->Root.ErrorCount = 0;
	Node->X = 0;
	Node->Y = 0;

	Token = NextToken(source_code, &stream_cursor_position);

	do
	{
		if (Token.Class == CONST)
		{
			ReturnStruct = ParseDeclarationConst(&stream_cursor_position, source_code, Node);

			if (ReturnStruct != NULL)
			{
				Token = ReturnStruct->LastToken;

				if (ReturnStruct->ReturnNode != NULL)
				{
					Node->Root.DeclarationLink = ReturnStruct->ReturnNode;
				}
				else
				{
					ErrorRecovery(&stream_cursor_position, source_code, &Token, ReturnStruct);

					continue;
				}
			}
			else
			{
				ErrorRecovery(&stream_cursor_position, source_code, &Token, ReturnStruct);

				continue;
			}
		}
		else if (Token.Class == VAR)
		{
			ReturnStruct = ParseDeclarationVar(&stream_cursor_position, source_code, Node);

			if (ReturnStruct != NULL)
			{
				Token = ReturnStruct->LastToken;

				if (ReturnStruct->ReturnNode != NULL)
				{
					Node->Root.DeclarationLink = ReturnStruct->ReturnNode;
				}
				else
				{
					ErrorRecovery(&stream_cursor_position, source_code, &Token, ReturnStruct);

					continue;
				}
			}
			else
			{
				ErrorRecovery(&stream_cursor_position, source_code, &Token, ReturnStruct);

				continue;
			}
		}

	} while (Token.Class == CONST || Token.Class == VAR);

	if (Token.Class == BEGIN)
	{
		ReturnStruct = ParseBody(&stream_cursor_position, source_code, Node, Node);

		if (ReturnStruct->ReturnNode == NULL)
		{
			if (ReturnStruct->ReturnNode == NULL)
			{
				while (Token.Class != END && Token.Class != _EOF)
				{
					Token = NextToken(source_code, &stream_cursor_position);
					ReturnStruct->LastToken = Token;
				}
			}
		}
		else
		{
			ReturnStruct->ReturnNode->X = Token.X;
			ReturnStruct->ReturnNode->Y = Token.Y;

			Node->Root.BodyLink = ReturnStruct->ReturnNode;
			Token = ReturnStruct->LastToken;
		}

		if (Token.Class == END)
		{
			Token = NextToken(source_code, &stream_cursor_position);

			if (Token.Class == POINT)
			{
				return Node;
			}
			else
			{
				printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \".\" вместо %s;\n",
					   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
				Node->Root.ErrorCount++;
			}
		}
		else
		{
			printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"END\" вместо %s;\n",
				   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
			printf("\033[01;31mИсправьте ошибки, чтобы скомпилировать программу.\033[0m\n");
			return NULL;
		}
	}
	else
	{
		printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось BEGIN вместо %s;\n",
			   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
	}

	close_program_file(source_code);

	return Node;
}

struct ReturnStruct *ParseBody(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *ParentLink, struct AstNode *RootLink)
{
	struct token Token;
	struct AstNode *Node = NULL;
	struct ReturnStruct *ReturnStruct = NULL;

	int VoidFlag = 0;

	ReturnStruct = malloc(sizeof(struct ReturnStruct));
	Node = malloc(sizeof(struct ReturnStruct));

	Node->Type = AstNodeBody;
	Node->Body.CommandCount = 0;
	Node->Body.ParentLink = ParentLink;

	Token = NextToken(source_code, stream_cursor_position);
	ReturnStruct->LastToken = Token;

	if (Token.Class == END)
	{
		VoidFlag = 1;

		Node->Type = AstNodeVoid;
		Node->Body.CommandCount = 0;
	}

	if (VoidFlag == 0)
	{
		do
		{
			int ErrorFlag = 0;
			int StartCommandCount = Node->Body.CommandCount;

			switch (Token.Class)
			{
			case IDENTIFIER:;
				struct AstNode *VarNode = NULL;

				VarNode = malloc(sizeof(struct AstNode));
				VarNode->Type = AstNodeVariable;
				strcpy(VarNode->Variable.Name, Token.Lexeme);
				VarNode->X = Token.X;
				VarNode->Y = Token.Y;

				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;

				if (Token.Class == L_BRACE)
				{
					struct ReturnStruct *OffsetExp;

					OffsetExp = ParseExpression(stream_cursor_position, source_code, VarNode, 0, R_BRACE, RootLink);

					if (OffsetExp != NULL)
					{
						if (OffsetExp->ReturnNode != NULL)
						{
							VarNode->Variable.OffsetExpression = OffsetExp->ReturnNode;
							Token = NextToken(source_code, stream_cursor_position);
							ReturnStruct->LastToken = Token;
						}
						else
						{
							ErrorFlag = 1;
							break;
						}
					}
					else
					{
						ErrorFlag = 1;
						break;
					}
				}

				if (Token.Class == ASSIGMENT)
				{
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * (Node->Body.CommandCount + 1));
					Node->Body.CommandList[Node->Body.CommandCount].Type = AstNodeAssign;
					Node->Body.CommandList[Node->Body.CommandCount].Assign.VariableLink = VarNode;
					VarNode->Variable.ParentLink = &Node->Body.CommandList[Node->Body.CommandCount];

					Node->Body.CommandList[Node->Body.CommandCount].X = Token.X;
					Node->Body.CommandList[Node->Body.CommandCount].Y = Token.Y;

					struct ReturnStruct *TempNode = ParseExpression(stream_cursor_position, source_code, &Node->Body.CommandList[Node->Body.CommandCount], 0, SEMI, RootLink);

					if (TempNode != NULL)
					{
						Token = TempNode->LastToken;
						ReturnStruct->LastToken = Token;

						if (TempNode->ReturnNode != NULL)
						{
							Node->Body.CommandList[Node->Body.CommandCount].Assign.RightLink = TempNode->ReturnNode;
							Node->Body.CommandCount++;
						}
						else
						{
							Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
							ErrorFlag = 1;
							break;
						}
					}
					else
					{
						Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
						ErrorFlag = 1;
						break;
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \":=\" вместо %s;\n",
						   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
					RootLink->Root.ErrorCount++;
					ErrorFlag = 1;
					break;
				}
				break;

			case WHILE:
			{
				Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * (Node->Body.CommandCount + 1));
				Node->Body.CommandList[Node->Body.CommandCount].Type = AstNodeWhile;
				Node->Body.CommandList[Node->Body.CommandCount].X = Token.X;
				Node->Body.CommandList[Node->Body.CommandCount].Y = Token.Y;

				struct ReturnStruct *TempNode = ParseExpression(stream_cursor_position, source_code, &Node->Body.CommandList[Node->Body.CommandCount], 0, DO, RootLink);

				if (TempNode != NULL)
				{
					Token = TempNode->LastToken;
					ReturnStruct->LastToken = Token;

					if (TempNode->ReturnNode != NULL)
					{
						Node->Body.CommandList[Node->Body.CommandCount].While.ConditionLink = TempNode->ReturnNode;
					}
					else
					{
						Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
						ErrorFlag = 1;
						break;
					}
				}
				else
				{
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
					ErrorFlag = 1;
					break;
				}

				if (Token.Class == DO)
				{
					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == BEGIN)
					{
						struct ReturnStruct *TempBody = ParseBody(stream_cursor_position, source_code, &Node->Body.CommandList[Node->Body.CommandCount], RootLink);

						if (TempBody != NULL)
						{
							Token = TempBody->LastToken;
							ReturnStruct->LastToken = Token;

							if (TempBody->ReturnNode != NULL)
							{
								TempBody->ReturnNode->X = Token.X;
								TempBody->ReturnNode->Y = Token.Y;
								Node->Body.CommandList[Node->Body.CommandCount].While.BodyLink = TempBody->ReturnNode;
							}
							else
							{
								ErrorFlag = 1;
								break;
							}
						}
						else
						{
							ErrorFlag = 1;
							break;
						}

						if (Token.Class != END)
						{
							printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"END\" вместо %s;\n",
								   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
							Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
							RootLink->Root.ErrorCount++;
							ErrorFlag = 1;
							break;
						}
						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"BEGIN\" вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
						RootLink->Root.ErrorCount++;
						ErrorFlag = 1;
						break;
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"DO\" вместо %s;\n",
						   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
					RootLink->Root.ErrorCount++;
					ErrorFlag = 1;
					break;
				}
				Node->Body.CommandCount++;

				break;
			}

			case IF:
				Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * (Node->Body.CommandCount + 1));
				Node->Body.CommandList[Node->Body.CommandCount].Type = AstNodeBranch;
				Node->Body.CommandList[Node->Body.CommandCount].X = Token.X;
				Node->Body.CommandList[Node->Body.CommandCount].Y = Token.Y;
				struct ReturnStruct *TempNode = ParseExpression(stream_cursor_position, source_code, &Node->Body.CommandList[Node->Body.CommandCount], 0, THEN, RootLink);

				if (TempNode != NULL)
				{
					Token = TempNode->LastToken;
					ReturnStruct->LastToken = Token;

					if (TempNode->ReturnNode != NULL)
					{
						Node->Body.CommandList[Node->Body.CommandCount].Branch.ConditionLink = TempNode->ReturnNode;
					}
					else
					{
						Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
						ErrorFlag = 1;
						break;
					}
				}
				else
				{
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
					ErrorFlag = 1;
					break;
				}

				if (Token.Class == THEN)
				{
					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == BEGIN)
					{
						struct ReturnStruct *TempBody = ParseBody(stream_cursor_position, source_code, &Node->Body.CommandList[Node->Body.CommandCount], RootLink);

						if (TempBody != NULL)
						{
							Token = TempBody->LastToken;
							ReturnStruct->LastToken = Token;

							if (TempBody->ReturnNode != NULL)
							{
								TempBody->ReturnNode->X = Token.X;
								TempBody->ReturnNode->Y = Token.Y;
								Node->Body.CommandList[Node->Body.CommandCount].Branch.IfBodyLink = TempBody->ReturnNode;
							}
							else
							{
								ErrorFlag = 1;
								break;
							}
						}
						else
						{
							ErrorFlag = 1;
							break;
						}

						if (Token.Class != END)
						{
							printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"END\" вместо %s;\n",
								   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
							Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
							RootLink->Root.ErrorCount++;
							ErrorFlag = 1;
							break;
						}
						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;

						if (Token.Class == ELSE)
						{
							Token = NextToken(source_code, stream_cursor_position);
							ReturnStruct->LastToken = Token;

							if (Token.Class == BEGIN)
							{
								struct ReturnStruct *TempBody = ParseBody(stream_cursor_position, source_code, &Node->Body.CommandList[Node->Body.CommandCount], RootLink);

								if (TempBody != NULL)
								{
									Token = TempBody->LastToken;
									ReturnStruct->LastToken = Token;

									if (TempBody->ReturnNode != NULL)
									{
										TempBody->ReturnNode->X = Token.X;
										TempBody->ReturnNode->Y = Token.Y;
										Node->Body.CommandList[Node->Body.CommandCount].Branch.ElseBodyLink = TempBody->ReturnNode;
									}
									else
									{
										ErrorFlag = 1;
										break;
									}
								}
								else
								{
									ErrorFlag = 1;
									break;
								}

								if (Token.Class != END)
								{
									printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"END\" вместо %s;\n",
										   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
									Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
									RootLink->Root.ErrorCount++;
									ErrorFlag = 1;
									break;
								}
								Token = NextToken(source_code, stream_cursor_position);
								ReturnStruct->LastToken = Token;
							}
							else
							{
								printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"BEGIN\" вместо %s;\n",
									   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
								Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
								RootLink->Root.ErrorCount++;
								ErrorFlag = 1;
								break;
							}
						}
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"BEGIN\" вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
						RootLink->Root.ErrorCount++;
						ErrorFlag = 1;
						break;
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"THEN\" вместо %s;\n",
						   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
					RootLink->Root.ErrorCount++;
					ErrorFlag = 1;
					break;
				}
				Node->Body.CommandCount++;

				break;

			case WRITELN:
				Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * (Node->Body.CommandCount + 1));
				Node->Body.CommandList[Node->Body.CommandCount].Type = AstNodeWrite;
				Node->Body.CommandList[Node->Body.CommandCount].X = Token.X;
				Node->Body.CommandList[Node->Body.CommandCount].Y = Token.Y;

				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;

				if (Token.Class == L_PAREN)
				{
					Node->Body.CommandList[Node->Body.CommandCount].Write.ArgCount = 0;
					struct AstNode *TempList = NULL;
					do
					{
						TempList = realloc(TempList, sizeof(struct AstNode) * (Node->Body.CommandList[Node->Body.CommandCount].Write.ArgCount + 1));

						struct ReturnStruct *TempNode = ParseExpression(stream_cursor_position, source_code, &TempList[Node->Body.CommandList[Node->Body.CommandCount].Write.ArgCount], 0, COMMA, RootLink);

						if (TempNode != NULL)
						{
							Token = TempNode->LastToken;
							ReturnStruct->LastToken = Token;

							if (TempNode->ReturnNode != NULL)
							{
								TempList[Node->Body.CommandList[Node->Body.CommandCount].Write.ArgCount] = *TempNode->ReturnNode;
								Node->Body.CommandList[Node->Body.CommandCount].Write.ArgCount++;
								Node->Body.CommandList[Node->Body.CommandCount].Write.ExpressionList = TempList;
							}
							else
							{
								Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
								ErrorFlag = 1;
								break;
							}
						}
						else
						{
							Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
							ErrorFlag = 1;
							break;
						}

					} while (Token.Class == COMMA);

					if (ErrorFlag != 0)
					{
						break;
					}

					if (Token.Class != R_PAREN)
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \")\" вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
						RootLink->Root.ErrorCount++;
						ErrorFlag = 1;
						break;
					}

					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"(\" вместо %s;\n",
						   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
					RootLink->Root.ErrorCount++;
					ErrorFlag = 1;
					break;
				}
				Node->Body.CommandCount++;

				break;

			case READLN:
				Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * (Node->Body.CommandCount + 1));
				Node->Body.CommandList[Node->Body.CommandCount].Type = AstNodeRead;
				Node->Body.CommandList[Node->Body.CommandCount].X = Token.X;
				Node->Body.CommandList[Node->Body.CommandCount].Y = Token.Y;

				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;

				if (Token.Class == L_PAREN)
				{
					Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount = 0;

					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == IDENTIFIER)
					{
						Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable = realloc(Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable, sizeof(struct AstNode) * (Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount + 1));
						strcpy(Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable[Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount].Name, Token.Lexeme);
					}
					else
					{
						if (Token.Class == R_PAREN)
						{
							Token = NextToken(source_code, stream_cursor_position);
							ReturnStruct->LastToken = Token;

							Node->Body.CommandCount++;

							break;
						}
						else
						{
							printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалась переменная вместо %s;\n",
								   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
							Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
							RootLink->Root.ErrorCount++;
							ErrorFlag = 1;
							break;
						}
					}

					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == L_BRACE)
					{
						struct ReturnStruct *OffsetExp;

						OffsetExp = ParseExpression(stream_cursor_position, source_code, VarNode, 0, R_BRACE, RootLink);

						if (OffsetExp != NULL)
						{
							if (OffsetExp->ReturnNode != NULL)
							{
								Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable[Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount].OffsetExpression = OffsetExp->ReturnNode;
							}
							else
							{
								ErrorFlag = 1;
								break;
							}
						}
						else
						{
							ErrorFlag = 1;
							break;
						}

						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;
					}

					Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount++;

					while (Token.Class == COMMA)
					{
						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;

						if (Token.Class == IDENTIFIER)
						{
							Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable = realloc(Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable, sizeof(struct AstNode) * (Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount + 1));
							strcpy(Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable[Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount].Name, Token.Lexeme);

							Token = NextToken(source_code, stream_cursor_position);
							ReturnStruct->LastToken = Token;

							if (Token.Class == L_BRACE)
							{
								struct ReturnStruct *OffsetExp;

								OffsetExp = ParseExpression(stream_cursor_position, source_code, VarNode, 0, R_BRACE, RootLink);

								if (OffsetExp != NULL)
								{
									if (OffsetExp->ReturnNode != NULL)
									{
										Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable[Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount].OffsetExpression = OffsetExp->ReturnNode;
										Token = NextToken(source_code, stream_cursor_position);
										ReturnStruct->LastToken = Token;
									}
									else
									{
										ErrorFlag = 1;
										break;
									}
								}
								else
								{
									ErrorFlag = 1;
									break;
								}
							}

							if (ErrorFlag != 0)
							{
								break;
							}

							Node->Body.CommandList[Node->Body.CommandCount].Read.VariableCount++;
						}
						else
						{
							printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалась переменная вместо %s;\n",
								   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
							Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable = realloc(Node->Body.CommandList[Node->Body.CommandCount].Read.ListVariable, 0);
							RootLink->Root.ErrorCount++;
							ErrorFlag = 1;
							break;
						}
					}

					if (Token.Class != R_PAREN)
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \")\" вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
						RootLink->Root.ErrorCount++;
						ErrorFlag = 1;
						break;
					}

					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"(\" вместо %s;\n",
						   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);
					RootLink->Root.ErrorCount++;
					ErrorFlag = 1;
					break;
				}
				Node->Body.CommandCount++;

				break;

			default:
				printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидались \"WHILE\", \"IF\", \"READLN\", \"WRITELN\" или переменная вместо %s;\n",
					   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
				RootLink->Root.ErrorCount++;
				ErrorFlag = 1;
				break;
			}

			if (ErrorFlag == 0)
			{
				if (Token.Class == SEMI)
				{
					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \";\" вместо %s;\n",
						   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
					RootLink->Root.ErrorCount++;
					Node->Body.CommandCount = StartCommandCount;
					Node->Body.CommandList = realloc(Node->Body.CommandList, sizeof(struct AstNode) * Node->Body.CommandCount);

					ErrorFlag = 1;
				}
			}

			if (ErrorFlag != 0)
			{
				ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

				continue;
			}

		} while (Token.Class == IDENTIFIER || Token.Class == WHILE || Token.Class == IF || Token.Class == READLN || Token.Class == WRITELN);
	}

	if (Node->Body.CommandCount > 0)
	{
		ReturnStruct->ReturnNode = Node;
	}
	else
	{
		ReturnStruct->ReturnNode = NULL;
	}

	return ReturnStruct;
}

struct ReturnStruct *ParseDeclarationVar(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *RootLink)
{

	struct token Token;
	struct AstNode *Node = RootLink->Root.DeclarationLink;
	struct ReturnStruct *ReturnStruct = NULL;

	ReturnStruct = malloc(sizeof(struct ReturnStruct));

	if (Node == NULL)
	{

		Node = malloc(sizeof(struct AstNode));

		Node->Type = AstNodeDeclaration;
		Node->Declaration.RootLink = RootLink;
		Node->Declaration.VariableCount = 0;
		Node->Declaration.ListVariable = NULL;
	}

	int Start = Node->Declaration.VariableCount;

	int First = 0, Last = 0, ArrayFlag = 0;

	Token = NextToken(source_code, stream_cursor_position);
	ReturnStruct->LastToken = Token;

	do
	{
		if (Token.Class != IDENTIFIER)
		{
			printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось имя переменной вместо %s;\n",
				   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
			RootLink->Root.ErrorCount++;

			ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

			continue;
		}
		else
		{
			Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * (Node->Declaration.VariableCount + 1));
			strcpy(Node->Declaration.ListVariable[Node->Declaration.VariableCount].ParamVariable.Name, Token.Lexeme);
			Node->Declaration.ListVariable[Node->Declaration.VariableCount].X = Token.X;
			Node->Declaration.ListVariable[Node->Declaration.VariableCount].Y = Token.Y;
			Start = Node->Declaration.VariableCount;
			Node->Declaration.VariableCount++;
			First = 0, Last = 0, ArrayFlag = 0;
		}

		do
		{
			Token = NextToken(source_code, stream_cursor_position);
			ReturnStruct->LastToken = Token;

			if (Token.Class == COMMA)
			{
				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;
			}
			else
			{
				break;
			}

			if (Token.Class == IDENTIFIER)
			{
				Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * (Node->Declaration.VariableCount + 1));
				strcpy(Node->Declaration.ListVariable[Node->Declaration.VariableCount].ParamVariable.Name, Token.Lexeme);
				Node->Declaration.ListVariable[Node->Declaration.VariableCount].X = Token.X;
				Node->Declaration.ListVariable[Node->Declaration.VariableCount].Y = Token.Y;
				Node->Declaration.VariableCount++;
			}

		} while (Token.Class == IDENTIFIER);

		if (Token.Class == COLON)
		{
			Token = NextToken(source_code, stream_cursor_position);
			ReturnStruct->LastToken = Token;

			if (Token.Class == ARRAY)
			{
				ArrayFlag = 1;

				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;

				if (Token.Class == L_BRACE)
				{
					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == NUM_10)
					{
						First = atoi(Token.Lexeme);
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидался целочисленный десятичный индекс массива вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
						Node->Declaration.VariableCount = Start;
						RootLink->Root.ErrorCount++;

						ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

						continue;
					}

					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == ELLIPSIS)
					{
						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"..\" вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
						Node->Declaration.VariableCount = Start;
						RootLink->Root.ErrorCount++;

						ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

						continue;
					}

					if (Token.Class == NUM_10)
					{
						Last = atoi(Token.Lexeme);
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидался целочисленный десятичный индекс массива вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
						Node->Declaration.VariableCount = Start;

						RootLink->Root.ErrorCount++;

						ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

						continue;
					}

					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == R_BRACE)
					{
						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"]\" вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
						Node->Declaration.VariableCount = Start;

						RootLink->Root.ErrorCount++;

						ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

						continue;
					}

					if (Token.Class == OF)
					{
						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"OF\" вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
						Node->Declaration.VariableCount = Start;
						RootLink->Root.ErrorCount++;

						ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

						continue;
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"[\"  вместо %s;\n",
						   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
					Node->Declaration.VariableCount = Start;
					Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);

					RootLink->Root.ErrorCount++;

					ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

					continue;
				}
			}

			int ErrorFlag = 0;

			switch (Token.Class)
			{
			case INTEGER:
				for (int i = Start; i < Node->Declaration.VariableCount; i++)
				{
					Node->Declaration.ListVariable[i].ParamVariable.First = First;
					Node->Declaration.ListVariable[i].ParamVariable.Last = Last;
					Node->Declaration.ListVariable[i].ParamVariable.Length = Last - First + 1;
					if (ArrayFlag == 1)
					{
						Node->Declaration.ListVariable[i].ParamVariable.ArrayFlag = 1;
					}
					Node->Declaration.ListVariable[i].ParamVariable.VarType = Integer;
				}

				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;

				break;

			case STRING:
				for (int i = Start; i < Node->Declaration.VariableCount; i++)
				{
					if (ArrayFlag == 1)
					{
						Node->Declaration.ListVariable[i].ParamVariable.First = First;
						Node->Declaration.ListVariable[i].ParamVariable.Last = Last;
						Node->Declaration.ListVariable[i].ParamVariable.Length = Last - First + 1;
						Node->Declaration.ListVariable[i].ParamVariable.ArrayFlag = 1;
					}
					else
					{
						Node->Declaration.ListVariable[i].ParamVariable.Length = 1;
					}

					Node->Declaration.ListVariable[i].ParamVariable.StringSize = 255;
					Node->Declaration.ListVariable[i].ParamVariable.VarType = String;
				}

				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;

				if (Token.Class == L_BRACE)
				{
					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == NUM_10)
					{
						if (atoi(Token.Lexeme) < 255)
						{
							for (int i = Start; i < Node->Declaration.VariableCount; i++)
							{
								Node->Declaration.ListVariable[i].ParamVariable.StringSize = atoi(Token.Lexeme);
							}
						}
						else
						{
							printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m размер строки не должен превышать 255;\n",
								   Token.Y, Token.X, Token.Lexeme);
							Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
							Node->Declaration.VariableCount = Start;
							RootLink->Root.ErrorCount++;
							ErrorFlag = 1;
						}
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидался целочисленный индекс массива вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
						Node->Declaration.VariableCount = Start;
						RootLink->Root.ErrorCount++;
						ErrorFlag = 1;
					}

					Token = NextToken(source_code, stream_cursor_position);
					ReturnStruct->LastToken = Token;

					if (Token.Class == R_BRACE)
					{
						Token = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = Token;
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"]\"  вместо %s;\n",
							   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
						Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
						Node->Declaration.VariableCount = Start;
						RootLink->Root.ErrorCount++;
						ErrorFlag = 1;
					}
				}

				break;

			default:
				printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидался тип переменной (INTEGER или STRING)  вместо %s;\n",
					   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
				Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
				Node->Declaration.VariableCount = Start;
				RootLink->Root.ErrorCount++;
				ErrorFlag = 1;
			}

			if (ErrorFlag != 0)
			{
				ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

				continue;
			}
		}
		else
		{
			printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \":\"  вместо %s;\n",
				   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);
			Node->Declaration.ListVariable = realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * Start);
			Node->Declaration.VariableCount = Start;

			RootLink->Root.ErrorCount++;

			ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

			continue;
		}

		Token = NextToken(source_code, stream_cursor_position);
		ReturnStruct->LastToken = Token;

	} while (Token.Class == IDENTIFIER);

	ReturnStruct->ReturnNode = Node;

	return ReturnStruct;
}

struct ReturnStruct *ParseDeclarationConst(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *RootLink)
{
	struct token Token;
	struct AstNode *Node = RootLink->Root.DeclarationLink;
	struct ReturnStruct *ReturnStruct = NULL, *TempNode = NULL;

	ReturnStruct = malloc(sizeof(struct ReturnStruct));

	if (Node == NULL)
	{
		Node = malloc(sizeof(struct AstNode));

		Node->Type = AstNodeDeclaration;
		Node->Declaration.RootLink = RootLink;
		Node->Declaration.VariableCount = 0;
		Node->Declaration.ListVariable = NULL;
	}

	Token = NextToken(source_code, stream_cursor_position);
	ReturnStruct->LastToken = Token;

	char ConstName[32];
	int ConstX = Token.X, ConstY = Token.Y;

	do
	{

		if (Token.Class == IDENTIFIER)
		{
			strcpy(ConstName, Token.Lexeme);

			Token = NextToken(source_code, stream_cursor_position);
			ReturnStruct->LastToken = Token;

			if (Token.Class == EQUAL)
			{
				TempNode = ParseExpression(stream_cursor_position, source_code, &Node->Declaration.ListVariable[Node->Declaration.VariableCount], 0, SEMI, RootLink);

				if (TempNode == NULL)
				{
					ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

					continue;
				}
				else
				{
					ReturnStruct->LastToken = TempNode->LastToken;
					if (TempNode->ReturnNode == NULL)
					{
						ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

						continue;
					}
				}

				Token = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = Token;
			}
			else
			{
				printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось \"=\" вместо %s;\n",
					   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);

				RootLink->Root.ErrorCount++;

				ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

				continue;
			}
		}
		else
		{
			printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось имя константы вместо %s;\n",
				   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);

			RootLink->Root.ErrorCount++;

			ErrorRecovery(stream_cursor_position, source_code, &Token, ReturnStruct);

			continue;
		}
		Node->Declaration.ListVariable =
			realloc(Node->Declaration.ListVariable, sizeof(struct AstNode) * (Node->Declaration.VariableCount + 1));
		strcpy(Node->Declaration.ListVariable[Node->Declaration.VariableCount].ParamVariable.Name, ConstName);
		Node->Declaration.ListVariable[Node->Declaration.VariableCount].ParamVariable.VarType = Constant;
		Node->Declaration.ListVariable[Node->Declaration.VariableCount].ParamVariable.ExpressionLink = TempNode->ReturnNode;
		Node->Declaration.ListVariable[Node->Declaration.VariableCount].X = ConstX;
		Node->Declaration.ListVariable[Node->Declaration.VariableCount].Y = ConstY;
		Node->Declaration.VariableCount++;

	} while (Token.Class == IDENTIFIER);

	ReturnStruct->ReturnNode = Node;

	return ReturnStruct;
}

struct ReturnStruct *ParseExpression(struct stream_cursor_position *stream_cursor_position, FILE *source_code, struct AstNode *ParentLink, int ParenFlag, TClass Separator, struct AstNode *RootLink)
{
	struct ReturnStruct *ReturnStruct = NULL, *TempStruct = NULL;

	ReturnStruct = malloc(sizeof(struct ReturnStruct));

	int OperationPriority[14];
	int PFlag = 0;

	OperationPriority[_UNARY_MINUS] = 0;
	OperationPriority[_AND] = 1;
	OperationPriority[_MULT] = 1;
	OperationPriority[_DIV] = 1;
	OperationPriority[_MOD] = 1;
	OperationPriority[_OR] = 2;
	OperationPriority[_PLUS] = 2;
	OperationPriority[_MINUS] = 2;
	OperationPriority[_EQUAL] = 3;
	OperationPriority[_NOT_EQUAL] = 3;
	OperationPriority[_LESS] = 3;
	OperationPriority[_LARGER] = 3;
	OperationPriority[_LESS_OR_EQUAL] = 3;
	OperationPriority[_LARGER_OR_EQUAL] = 3;

	struct token Token;

	struct AstNode *Node = NULL, *LeftNode = NULL, *RightNode = NULL;

	Token = NextToken(source_code, stream_cursor_position);
	ReturnStruct->LastToken = Token;

	if (Token.Class == L_PAREN || Token.Class == MINUS || Token.Class == IDENTIFIER || Token.Class == NUM_2 || Token.Class == NUM_8 ||
		Token.Class == NUM_10 || Token.Class == NUM_16 || Token.Class == STRING_LITERAL)
	{
		LeftNode = malloc(sizeof(struct AstNode));

		LeftNode->Variable.ParentLink = NULL;
		LeftNode->Variable.OffsetExpression = NULL;

		switch (Token.Class)
		{
		case L_PAREN:
			TempStruct = ParseExpression(stream_cursor_position, source_code, ParentLink, 1, Separator, RootLink);

			if (TempStruct->ReturnNode != NULL)
			{
				LeftNode = TempStruct->ReturnNode;

				Token = TempStruct->LastToken;
				ReturnStruct->LastToken = Token;
			}
			else
			{
				ReturnStruct->LastToken = Token;
				ReturnStruct->ReturnNode = NULL;
				free(TempStruct);

				return ReturnStruct;
			}

			if (Token.Class != R_PAREN)
			{
				printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m не хватает закрывающей скобки;\n",
					   Token.Y, Token.X, Token.Lexeme);

				RootLink->Root.ErrorCount++;
				ReturnStruct->ReturnNode = NULL;
				free(TempStruct);

				return ReturnStruct;
			}

			break;

		case MINUS:
			LeftNode->Type = AstNodeExpression;
			LeftNode->Expression.RightLink = NULL;
			LeftNode->Expression.Operation = _UNARY_MINUS;
			LeftNode->Expression.ResultType = Constant;
			LeftNode->X = Token.X;
			LeftNode->Y = Token.Y;

			struct token TempToken;

			TempToken = NextToken(source_code, stream_cursor_position);
			ReturnStruct->LastToken = TempToken;

			struct AstNode *TempNode;

			if (TempToken.Class == NUM_2 || TempToken.Class == NUM_8 || TempToken.Class == NUM_10 || TempToken.Class == NUM_16)
			{
				TempNode = malloc(sizeof(struct AstNode));

				TempToken = DecimalValue(TempToken);

				TempNode->Type = AstNodeConstant;
				TempNode->Constant.Type = Integer;
				strcpy(TempNode->Constant.Value, TempToken.Lexeme);
				TempNode->Constant.ParentLink = LeftNode;
				TempNode->X = Token.X;
				TempNode->Y = Token.Y;
				LeftNode->Expression.LeftLink = TempNode;
			}
			else
			{
				printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось целое число перед унарным минусом вместо %s;\n",
					   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);

				RootLink->Root.ErrorCount++;
				ReturnStruct->ReturnNode = NULL;
				free(TempStruct);

				return ReturnStruct;
			}
			break;

		case IDENTIFIER:
			LeftNode->Type = AstNodeVariable;
			strcpy(LeftNode->Variable.Name, Token.Lexeme);
			LeftNode->X = Token.X;
			LeftNode->Y = Token.Y;
			break;

		case NUM_2:
			LeftNode->Type = AstNodeConstant;
			LeftNode->Constant.Type = Integer;
			Token = DecimalValue(Token);
			strcpy(LeftNode->Constant.Value, Token.Lexeme);
			LeftNode->X = Token.X;
			LeftNode->Y = Token.Y;
			break;

		case NUM_8:
			LeftNode->Type = AstNodeConstant;
			LeftNode->Constant.Type = Integer;
			Token = DecimalValue(Token);
			strcpy(LeftNode->Constant.Value, Token.Lexeme);
			LeftNode->X = Token.X;
			LeftNode->Y = Token.Y;
			break;

		case NUM_10:
			LeftNode->Type = AstNodeConstant;
			LeftNode->Constant.Type = Integer;
			strcpy(LeftNode->Constant.Value, Token.Lexeme);
			LeftNode->X = Token.X;
			LeftNode->Y = Token.Y;
			break;

		case NUM_16:
			LeftNode->Type = AstNodeConstant;
			LeftNode->Constant.Type = Integer;
			Token = DecimalValue(Token);
			strcpy(LeftNode->Constant.Value, Token.Lexeme);
			LeftNode->X = Token.X;
			LeftNode->Y = Token.Y;
			break;

		case STRING_LITERAL:
			LeftNode->Type = AstNodeConstant;
			LeftNode->Constant.Type = String;
			strcpy(LeftNode->Constant.Value, Token.Lexeme);
			LeftNode->X = Token.X;
			LeftNode->Y = Token.Y;
			break;

		default:
			printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидались переменная, числовая/строковая константа или \"(\") вместо %s;\n",
				   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);

			RootLink->Root.ErrorCount++;
			ReturnStruct->ReturnNode = NULL;
			free(TempStruct);

			return ReturnStruct;
		}

		free(TempStruct);

		struct token TempToken;

		if (LeftNode->Type == AstNodeVariable)
		{
			TempToken = NextToken(source_code, stream_cursor_position);
			ReturnStruct->LastToken = TempToken;

			if (TempToken.Class == L_BRACE)
			{
				struct ReturnStruct *OffsetExp = NULL;

				OffsetExp = ParseExpression(stream_cursor_position, source_code, LeftNode->Variable.OffsetExpression, 0, R_BRACE, RootLink);

				if (OffsetExp != NULL)
				{
					if (OffsetExp->ReturnNode != NULL)
					{
						LeftNode->Variable.OffsetExpression = OffsetExp->ReturnNode;
						TempToken = NextToken(source_code, stream_cursor_position);
						ReturnStruct->LastToken = TempToken;
					}
					else
					{
						RootLink->Root.ErrorCount++;
						ReturnStruct->ReturnNode = NULL;
						free(OffsetExp);

						return ReturnStruct;
					}
				}
				else
				{
					RootLink->Root.ErrorCount++;
					ReturnStruct->ReturnNode = NULL;
					free(OffsetExp);

					return ReturnStruct;
				}
			}
		}
		else
		{
			TempToken = NextToken(source_code, stream_cursor_position);
			ReturnStruct->LastToken = TempToken;
		}

		while (TempToken.Class == PLUS || TempToken.Class == MINUS || TempToken.Class == MULT || TempToken.Class == DIV || TempToken.Class == MOD || TempToken.Class == AND || TempToken.Class == OR || TempToken.Class == LARGER || TempToken.Class == LARGER_OR_EQUAL || TempToken.Class == LESS || TempToken.Class == LESS_OR_EQUAL ||
			   TempToken.Class == Separator || TempToken.Class == R_PAREN || TempToken.Class == EQUAL || TempToken.Class == NOT_EQUAL)
		{
			struct AstNode *NewNode = NULL;

			switch (TempToken.Class)
			{
			case R_PAREN:
				if (ParenFlag == 0)
				{
					if (Separator == COMMA)
					{
						if (Node == NULL)
						{
							Node = LeftNode;
						}

						ReturnStruct->ReturnNode = Node;

						return ReturnStruct;
					}
					else
					{
						printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m лишняя закрывающая скобка;\n",
							   TempToken.Y, TempToken.X, TempToken.Lexeme);

						RootLink->Root.ErrorCount++;
						ReturnStruct->ReturnNode = NULL;

						return ReturnStruct;
					}
				}

				if (Node == NULL)
				{
					ReturnStruct->ReturnNode = LeftNode;
					return ReturnStruct;
				}
				ReturnStruct->ReturnNode = Node;

				return ReturnStruct;

			case PLUS:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _PLUS;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case MINUS:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _MINUS;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case MULT:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _MULT;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case DIV:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _DIV;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case MOD:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _MOD;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case AND:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _AND;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case OR:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _OR;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case EQUAL:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _EQUAL;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case NOT_EQUAL:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _NOT_EQUAL;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case LESS:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _LESS;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case LESS_OR_EQUAL:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _LESS_OR_EQUAL;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case LARGER:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _LARGER;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			case LARGER_OR_EQUAL:
				NewNode = malloc(sizeof(struct AstNode));

				NewNode->Type = AstNodeExpression;
				NewNode->Expression.Operation = _LARGER_OR_EQUAL;
				NewNode->Expression.ResultType = Constant;
				NewNode->X = TempToken.X;
				NewNode->Y = TempToken.Y;
				break;

			default:
				if (TempToken.Class == Separator)
				{
					if (Node == NULL)
					{
						ReturnStruct->ReturnNode = LeftNode;
						return ReturnStruct;
					}
					ReturnStruct->ReturnNode = Node;
					return ReturnStruct;
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалась логическая/бинарная операция вместо %s;\n",
						   TempToken.Y, TempToken.X, TempToken.Lexeme, TypeStringMass[TempToken.Class]);

					RootLink->Root.ErrorCount++;
					ReturnStruct->ReturnNode = NULL;

					return ReturnStruct;
				}

				break;
			}

			if (Node != NULL)
			{
				if (OperationPriority[NewNode->Expression.Operation] >= OperationPriority[Node->Expression.Operation])
				{
					PFlag = 1;

					NewNode->Expression.ParentLink = Node->Expression.ParentLink;
					if (NewNode->Expression.ParentLink != NULL && NewNode->Expression.ParentLink->Type == AstNodeExpression)
						NewNode->Expression.ParentLink->Expression.RightLink = NewNode;
					NewNode->Expression.LeftLink = Node;
					Node->Expression.ParentLink = NewNode;
					NewNode->Expression.RightLink = NULL;
				}
				else
				{
					PFlag = 0;

					NewNode->Expression.LeftLink = Node->Expression.RightLink;
					switch (Node->Expression.RightLink->Type)
					{
					case AstNodeExpression:
						Node->Expression.RightLink->Expression.ParentLink = NewNode;
						break;
					case AstNodeConstant:
						Node->Expression.RightLink->Constant.ParentLink = NewNode;
						break;
					case AstNodeVariable:
						Node->Expression.RightLink->Variable.ParentLink = NewNode;
						break;
					default:
						break;
					}
					Node->Expression.RightLink = NewNode;
					NewNode->Expression.ParentLink = Node;
					NewNode->Expression.RightLink = NULL;
				}
			}
			else
			{
				NewNode->Expression.ParentLink = ParentLink;
				NewNode->Expression.LeftLink = LeftNode;

				if (LeftNode->Type == AstNodeVariable)
					LeftNode->Variable.ParentLink = NewNode;
				else
					LeftNode->Constant.ParentLink = NewNode;

				NewNode->Expression.RightLink = NULL;
			}

			//if (Node->Expression.ParentLink->Type == Exp)

			TempToken = NextToken(source_code, stream_cursor_position);

			ReturnStruct->LastToken = TempToken;

			switch (TempToken.Class)
			{
			case L_PAREN:
				RightNode = malloc(sizeof(struct AstNode));

				TempStruct = ParseExpression(stream_cursor_position, source_code, NewNode, 1, Separator, RootLink);

				if (TempStruct != NULL)
				{
					RightNode = TempStruct->ReturnNode;
					TempToken = TempStruct->LastToken;
				}

				if (TempToken.Class != R_PAREN)
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m не хватает закрывающей скобки;\n",
						   TempToken.Y, TempToken.X, TempToken.Lexeme);

					RootLink->Root.ErrorCount++;
					ReturnStruct->ReturnNode = NULL;

					return ReturnStruct;
				}
				break;

			case MINUS:
				RightNode = malloc(sizeof(struct AstNode));
				RightNode->Type = AstNodeExpression;
				RightNode->Expression.ParentLink = NewNode;
				RightNode->Expression.RightLink = NULL;
				RightNode->Expression.Operation = _UNARY_MINUS;
				RightNode->Expression.ResultType = Constant;
				RightNode->X = Token.X;
				RightNode->Y = Token.Y;

				TempToken = NextToken(source_code, stream_cursor_position);

				ReturnStruct->LastToken = TempToken;

				struct AstNode *TempNode;

				if (TempToken.Class == NUM_2 || TempToken.Class == NUM_8 || TempToken.Class == NUM_10 || TempToken.Class == NUM_16)
				{
					TempNode = malloc(sizeof(struct AstNode));

					TempNode->Type = AstNodeConstant;
					TempNode->Constant.Type = Integer;
					TempToken = DecimalValue(TempToken);
					strcpy(TempNode->Constant.Value, TempToken.Lexeme);
					TempNode->X = Token.X;
					TempNode->Y = Token.Y;
					TempNode->Constant.ParentLink = RightNode;
					RightNode->Expression.LeftLink = TempNode;
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидалось целое число перед унарным минусом вместо %s;\n",
						   TempToken.Y, TempToken.X, TempToken.Lexeme, TypeStringMass[TempToken.Class]);

					RootLink->Root.ErrorCount++;
					ReturnStruct->ReturnNode = NULL;

					return ReturnStruct;
				}
				break;

			case IDENTIFIER:
				RightNode = malloc(sizeof(struct AstNode));

				RightNode->Type = AstNodeVariable;
				strcpy(RightNode->Variable.Name, TempToken.Lexeme);
				RightNode->X = Token.X;
				RightNode->Y = Token.Y;
				RightNode->Variable.ParentLink = NewNode;
				break;

			case NUM_2:
				RightNode = malloc(sizeof(struct AstNode));

				RightNode->Type = AstNodeConstant;
				RightNode->Constant.Type = Integer;
				TempToken = DecimalValue(TempToken);
				strcpy(RightNode->Constant.Value, TempToken.Lexeme);
				RightNode->X = Token.X;
				RightNode->Y = Token.Y;
				RightNode->Constant.ParentLink = NewNode;
				break;

			case NUM_8:
				RightNode = malloc(sizeof(struct AstNode));

				RightNode->Type = AstNodeConstant;
				RightNode->Constant.Type = Integer;
				TempToken = DecimalValue(TempToken);
				strcpy(RightNode->Constant.Value, TempToken.Lexeme);
				RightNode->X = Token.X;
				RightNode->Y = Token.Y;
				RightNode->Constant.ParentLink = NewNode;
				break;

			case NUM_10:
				RightNode = malloc(sizeof(struct AstNode));

				RightNode->Type = AstNodeConstant;
				RightNode->Constant.Type = Integer;
				strcpy(RightNode->Constant.Value, TempToken.Lexeme);
				RightNode->X = Token.X;
				RightNode->Y = Token.Y;
				RightNode->Constant.ParentLink = NewNode;
				break;

			case NUM_16:
				RightNode = malloc(sizeof(struct AstNode));

				RightNode->Type = AstNodeConstant;
				RightNode->Constant.Type = Integer;
				TempToken = DecimalValue(TempToken);
				strcpy(RightNode->Constant.Value, TempToken.Lexeme);
				RightNode->X = Token.X;
				RightNode->Y = Token.Y;
				RightNode->Constant.ParentLink = NewNode;
				break;

			case STRING_LITERAL:
				RightNode = malloc(sizeof(struct AstNode));

				RightNode->Type = AstNodeConstant;
				RightNode->Constant.Type = String;
				strcpy(RightNode->Constant.Value, TempToken.Lexeme);
				RightNode->X = Token.X;
				RightNode->Y = Token.Y;
				RightNode->Constant.ParentLink = NewNode;
				break;

			default:
				printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидались переменная, числовая/строковая константа или \"(\") вместо %s;\n",
					   TempToken.Y, TempToken.X, TempToken.Lexeme, TypeStringMass[TempToken.Class]);

				RootLink->Root.ErrorCount++;
				ReturnStruct->ReturnNode = NULL;

				return ReturnStruct;
			}

			if (RightNode->Type == AstNodeVariable)
			{
				TempToken = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = TempToken;

				if (TempToken.Class == L_BRACE)
				{
					struct ReturnStruct *OffsetExp = NULL;

					OffsetExp = ParseExpression(stream_cursor_position, source_code, RightNode->Variable.OffsetExpression, 0, R_BRACE, RootLink);

					if (OffsetExp != NULL)
					{
						if (OffsetExp->ReturnNode != NULL)
						{
							RightNode->Variable.OffsetExpression = OffsetExp->ReturnNode;
							TempToken = NextToken(source_code, stream_cursor_position);
							ReturnStruct->LastToken = TempToken;
						}
						else
						{
							RootLink->Root.ErrorCount++;
							ReturnStruct->ReturnNode = NULL;

							return ReturnStruct;
						}
					}
					else
					{
						RootLink->Root.ErrorCount++;
						ReturnStruct->ReturnNode = NULL;

						return ReturnStruct;
					}
				}
			}
			else
			{
				TempToken = NextToken(source_code, stream_cursor_position);
				ReturnStruct->LastToken = TempToken;
			}

			NewNode->Expression.RightLink = RightNode;

			if (Node == NULL || PFlag == 1)
				Node = NewNode;
		}

		if (TempToken.Class != PLUS && TempToken.Class != MINUS && TempToken.Class != MULT && TempToken.Class != DIV && TempToken.Class != MOD && TempToken.Class != AND && TempToken.Class != OR && TempToken.Class != LARGER && TempToken.Class != LARGER_OR_EQUAL && TempToken.Class != LESS && TempToken.Class != LESS_OR_EQUAL &&
			TempToken.Class != Separator && TempToken.Class != R_PAREN && TempToken.Class != EQUAL)
		{
			printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m недопустимый токен \"%s\" в выражении (Ожидалась бинарная операция, \";\", или закрытие открывающей скобки);\n",
				   TempToken.Y, TempToken.X, TempToken.Lexeme, TempToken.Lexeme);

			RootLink->Root.ErrorCount++;
			ReturnStruct->ReturnNode = NULL;

			return ReturnStruct;
		}
	}
	else
	{
		printf("\033[01;31m<%d:%d> \"%s\" Синтаксическая ошибка:\033[0m ожидались переменная или числовая/строковая константа вместо %s;\n",
			   Token.Y, Token.X, Token.Lexeme, TypeStringMass[Token.Class]);

		RootLink->Root.ErrorCount++;
		ReturnStruct->ReturnNode = NULL;

		return ReturnStruct;
	}
	while (Node->Expression.ParentLink->Type == AstNodeExpression)
		Node = Node->Expression.ParentLink;

	ReturnStruct->ReturnNode = Node;

	return ReturnStruct;
}

void PrintAstTree(struct AstNode *Node, int Level)
{
	switch (Node->Type)
	{
	case AstNodeRoot:
		printf("\n");
		printf("<AST---ДЕРЕВО>");
		if (Node->Root.DeclarationLink != NULL)
			PrintAstTree(Node->Root.DeclarationLink, Level + 1);
		if (Node->Root.BodyLink != NULL)
			PrintAstTree(Node->Root.BodyLink, Level + 1);

		break;

	case AstNodeDeclaration:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<Объявление переменных и констант>");

		for (int i = 0; i < Node->Declaration.VariableCount; i++)
		{
			printf("\n");
			for (int i = 0; i < Level + 1; i++)
			{
				printf("   ");
			}
			printf("<Имя = \"%s\", ", Node->Declaration.ListVariable[i].ParamVariable.Name);

			switch (Node->Declaration.ListVariable[i].ParamVariable.VarType)
			{
			case Constant:
				printf("Тип = \"Константа\">");
				if (Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink != NULL)
					PrintAstTree(Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink, Level + 2);
				break;

			case Integer:
				printf("Тип = \"Целочисленная переменная\", ");
				break;

			case String:
				printf("Тип = \"Строковая переменная\", ");
				printf("Длина строки = %d, ", Node->Declaration.ListVariable[i].ParamVariable.StringSize);
				break;

			default:
				break;
			}

			if (Node->Declaration.ListVariable[i].ParamVariable.VarType != Constant)
			{
				if (Node->Declaration.ListVariable[i].ParamVariable.ArrayFlag == 0)
				{
					printf(" Массив = \"Нет\">");
				}
				else
				{
					printf("Массив = \"Да\", ");
					printf("Начальный индекс = %d, ", Node->Declaration.ListVariable[i].ParamVariable.First);
					printf("Конечный индекс = %d, ", Node->Declaration.ListVariable[i].ParamVariable.Last);
					printf("Длина = %d>", Node->Declaration.ListVariable[i].ParamVariable.Length);
				}
			}
		}

		break;

	case AstNodeExpression:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		switch (Node->Expression.Operation)
		{
		case _UNARY_MINUS:
			printf("<Унарный \"-\">");
			break;
		case _AND:
			printf("<\"AND\">");
			break;
		case _MULT:
			printf("<\"*\">");
			break;
		case _DIV:
			printf("<\"DIV\">");
			break;
		case _MOD:
			printf("<\"MOD\">");
			break;
		case _OR:
			printf("<\"OR\">");
			break;
		case _PLUS:
			printf("<\"+\">");
			break;
		case _MINUS:
			printf("<\"-\">");
			break;
		case _EQUAL:
			printf("<\"=\">");
			break;
		case _NOT_EQUAL:
			printf("<\"!=\">");
			break;
		case _LESS:
			printf("<\"<\">");
			break;
		case _LARGER:
			printf("<\">\">");
			break;
		case _LESS_OR_EQUAL:
			printf("<\"<=\">");
			break;
		case _LARGER_OR_EQUAL:
			printf("<\">=\">");
			break;
		}

		if (Node->Expression.LeftLink != NULL)
			PrintAstTree(Node->Expression.LeftLink, Level + 1);
		if (Node->Expression.RightLink != NULL)
			PrintAstTree(Node->Expression.RightLink, Level + 1);

		break;

	case AstNodeConstant:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		if (Node->Constant.Type == String)
			printf("<%s>", Node->Constant.Value);
		else if (Node->Constant.Type == Integer)
		{
			printf("<%s>", Node->Constant.Value);
		}

		break;

	case AstNodeBody:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<Тело>");

		for (int i = 0; i < Node->Body.CommandCount; i++)
		{
			PrintAstTree(&Node->Body.CommandList[i], Level + 1);
		}

		break;

	case AstNodeBranch:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<IF>");
		printf("\n");
		for (int i = 0; i < Level + 1; i++)
		{
			printf("  ");
		}
		printf("<Условие>");
		if (Node->Branch.ConditionLink != NULL)
			PrintAstTree(Node->Branch.ConditionLink, Level + 2);
		printf("\n");
		for (int i = 0; i < Level + 1; i++)
		{
			printf("  ");
		}
		printf("<Если истина>");
		if (Node->Branch.IfBodyLink != NULL)
			PrintAstTree(Node->Branch.IfBodyLink, Level + 2);
		printf("\n");
		for (int i = 0; i < Level + 1; i++)
		{
			printf("  ");
		}
		printf("<Если ложь>");
		if (Node->Branch.ElseBodyLink != NULL)
			PrintAstTree(Node->Branch.ElseBodyLink, Level + 2);

		break;

	case AstNodeAssign:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("\"=:\"");

		PrintAstTree(Node->Assign.VariableLink, Level + 1);
		PrintAstTree(Node->Assign.RightLink, Level + 1);

		break;

	case AstNodeVariable:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<%s>", Node->Variable.Name);

		if (Node->Variable.OffsetExpression != NULL)
		{
			printf("\n");

			for (int i = 0; i < Level; i++)
			{
				printf("   ");
			}

			printf("<Индекс массива>");
			PrintAstTree(Node->Variable.OffsetExpression, Level + 1);
		}

		break;

	case AstNodeRead:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<ReadLn>");

		for (int i = 0; i < Node->Read.VariableCount; i++)
		{
			printf("\n");

			for (int i = 0; i < Level + 1; i++)
			{
				printf("   ");
			}
			printf("<%s>", Node->Read.ListVariable[i].Name);

			if (Node->Read.ListVariable[i].OffsetExpression != NULL)
			{
				printf("\n");

				for (int i = 0; i < Level + 1; i++)
				{
					printf("   ");
				}

				printf("<Индекс массива>");
				PrintAstTree(Node->Read.ListVariable[i].OffsetExpression, Level + 2);
			}
		}

		break;

	case AstNodeVoid:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<Пустой узел>");

		break;

	case AstNodeWhile:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<WHILE>");
		printf("\n");

		for (int i = 0; i < Level + 1; i++)
		{
			printf("   ");
		}
		printf("<Условие>");

		if (Node->While.ConditionLink != NULL)
			PrintAstTree(Node->While.ConditionLink, Level + 2);

		if (Node->While.BodyLink != NULL)
			PrintAstTree(Node->While.BodyLink, Level + 1);

		break;

	case AstNodeWrite:
		printf("\n");

		for (int i = 0; i < Level; i++)
		{
			printf("   ");
		}

		printf("<WriteLn>");

		for (int i = 0; i < Node->Write.ArgCount; i++)
		{
			for (int i = 0; i < Level + 1; i++)
			{
				printf("   ");
			}
			PrintAstTree(&Node->Write.ExpressionList[i], Level + 1);
		}

		break;

	default:
		break;
	}
}

void FreeAstTree(struct AstNode *Node)
{
	switch (Node->Type)
	{
	case AstNodeRoot:
		if (Node->Root.DeclarationLink != NULL)
			FreeAstTree(Node->Root.DeclarationLink);
		if (Node->Root.BodyLink != NULL)
			FreeAstTree(Node->Root.BodyLink);

		free(Node);

		break;

	case AstNodeDeclaration:
		for (int i = 0; i < Node->Declaration.VariableCount; i++)
		{
			if (Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink != NULL)
				FreeAstTree(Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink);
		}

		free(Node->Declaration.ListVariable);
		free(Node);

		break;

	case AstNodeExpression:
		if (Node->Expression.LeftLink != NULL)
			FreeAstTree(Node->Expression.LeftLink);
		if (Node->Expression.RightLink != NULL)
			FreeAstTree(Node->Expression.RightLink);

		free(Node);

		break;

	case AstNodeConstant:
		free(Node);

		break;

	case AstNodeBody:
		for (int i = 0; i < Node->Body.CommandCount; i++)
		{
			FreeAstTree(&Node->Body.CommandList[i]);
		}

		break;

	case AstNodeBranch:
		if (Node->Branch.ConditionLink != NULL)
			FreeAstTree(Node->Branch.ConditionLink);
		if (Node->Branch.IfBodyLink != NULL)
			FreeAstTree(Node->Branch.IfBodyLink);
		if (Node->Branch.ElseBodyLink != NULL)
			FreeAstTree(Node->Branch.ElseBodyLink);

		break;

	case AstNodeAssign:
		if (Node->Assign.VariableLink != NULL)
			FreeAstTree(Node->Assign.VariableLink);
		if (Node->Assign.RightLink != NULL)
			FreeAstTree(Node->Assign.RightLink);

		break;

	case AstNodeVariable:
		if (Node->Variable.OffsetExpression != NULL)
			FreeAstTree(Node->Variable.OffsetExpression);

		free(Node);

		break;

	case AstNodeRead:
		for (int i = 0; i < Node->Read.VariableCount; i++)
			if (Node->Read.ListVariable[i].OffsetExpression != NULL)
				FreeAstTree(Node->Read.ListVariable[i].OffsetExpression);

		free(Node->Read.ListVariable);

		break;

	case AstNodeVoid:
		free(Node);

		break;

	case AstNodeWhile:
		if (Node->While.ConditionLink != NULL)
			FreeAstTree(Node->While.ConditionLink);
		if (Node->While.BodyLink != NULL)
			FreeAstTree(Node->While.BodyLink);

		break;

	case AstNodeWrite:
		free(Node->Write.ExpressionList);

		break;

	default:
		break;
	}
}