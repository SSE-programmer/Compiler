#include <stdio.h>
#include "../source/lexer.h"
#include "../source/parser_ast.h"
#include "../Unity-master/src/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void start_array_testing(struct token *mass, const int size, char *path)
{
	struct token token;
	struct stream_cursor_position stream_cursor_position;

	FILE *source_code = open_program_file(path, &stream_cursor_position);

	for (int i = 0; i < size; i++)
	{
		token = next_token(source_code, &stream_cursor_position);

		TEST_ASSERT_EQUAL_INT16(mass[i].X, token.X);
		TEST_ASSERT_EQUAL_INT16(mass[i].Y, token.Y);
		TEST_ASSERT_EQUAL_INT16(mass[i].Class, token.Class);
		TEST_ASSERT_EQUAL_STRING(mass[i].Lexeme, token.Lexeme);
	}

	close_program_file(source_code);
}

void test_Numbers_Token(void)
{
	const int TOKEN_COUNT = 20;

	struct token check_tokens[20] = {
		{3, 1, NUM_10, "10"},
		{6, 1, UNKNOWN, "10f"},
		{10, 1, IDENTIFIER, "f34"},
		{14, 1, IDENTIFIER, "d45f"},
		{1, 2, NUM_2, "%101011"},
		{9, 2, UNKNOWN, "%10012"},
		{16, 2, UNKNOWN, "%a54"},
		{21, 2, UNKNOWN, "%"},
		{1, 3, NUM_16, "$9ADE"},
		{7, 3, L_PAREN, "("},
		{8, 3, NUM_16, "$4afa"},
		{13, 3, R_PAREN, ")"},
		{15, 3, UNKNOWN, "$235GH"},
		{22, 3, UNKNOWN, "$"},
		{1, 4, NUM_8, "&12345670"},
		{11, 4, UNKNOWN, "&1234678"},
		{20, 4, UNKNOWN, "&32432a"},
		{28, 4, UNKNOWN, "&wq1234"},
		{36, 4, UNKNOWN, "&e2345g"},
		{44, 4, UNKNOWN, "&"}};

	start_array_testing(check_tokens, TOKEN_COUNT, "test_program/numbers.pas");
}

void test_Identifier_Token(void)
{
	const int TOKEN_COUNT = 16;

	struct token check_tokens[16] = {
		{1, 1, IDENTIFIER, "word"},
		{6, 1, IDENTIFIER, "word1"},
		{12, 1, UNKNOWN, "1word"},
		{1, 2, IDENTIFIER, "word_1"},
		{8, 2, UNKNOWN, "1_word"},
		{15, 2, IDENTIFIER, "_word"},
		{21, 2, IDENTIFIER, "_word_"},
		{28, 2, IDENTIFIER, "____word_word____"},
		{1, 3, IDENTIFIER, "w_q_q_ee__wqe1_1_23_1_2_"},
		{1, 4, L_PAREN, "("},
		{2, 4, IDENTIFIER, "qeweq"},
		{7, 4, UNKNOWN, "%@"},
		{9, 4, IDENTIFIER, "word"},
		{1, 5, IDENTIFIER, "___"},
		{5, 5, IDENTIFIER, "_1"},
		{8, 5, IDENTIFIER, "_"}};

	start_array_testing(check_tokens, TOKEN_COUNT, "test_program/identifier.pas");
}

void test_Keyword_Token(void)
{
	const int TOKEN_COUNT = 20;

	struct token check_tokens[20] = {
		{1, 1, BEGIN, "BEGIN"},
		{7, 1, END, "END"},
		{1, 2, IDENTIFIER, "function"},
		{1, 3, IF, "IF"},
		{1, 4, THEN, "THEN"},
		{6, 4, ELSE, "ELSE"},
		{12, 4, WHILE, "WHILE"},
		{1, 5, DO, "DO"},
		{1, 6, CONST, "CONST"},
		{1, 7, VAR, "VAR"},
		{1, 8, INTEGER, "INTEGER"},
		{9, 8, STRING, "STRING"},
		{16, 8, ARRAY, "ARRAY"},
		{22, 8, OF, "OF"},
		{1, 9, DIV, "DIV"},
		{1, 10, MOD, "MOD"},
		{5, 10, OR, "OR"},
		{1, 11, AND, "AND"},
		{1, 12, READLN, "READLN"},
		{1, 13, WRITELN, "WRITELN"}};

	start_array_testing(check_tokens, TOKEN_COUNT, "test_program/keyword.pas");
}

void test_Comparison_Token(void)
{
	const int TOKEN_COUNT = 14;

	struct token check_tokens[14] = {
		{1, 1, LESS, "<"},
		{3, 1, LARGER, ">"},
		{5, 1, EQUAL, "="},
		{7, 1, LESS_OR_EQUAL, "<="},
		{10, 1, LARGER_OR_EQUAL, ">="},
		{13, 1, NOT_EQUAL, "<>"},
		{1, 2, EQUAL, "="},
		{2, 2, LARGER_OR_EQUAL, ">="},
		{4, 2, LESS_OR_EQUAL, "<="},
		{6, 2, EQUAL, "="},
		{7, 2, LARGER, ">"},
		{8, 2, LARGER, ">"},
		{9, 2, LESS, "<"},
		{10, 2, LESS, "<"},
	};

	start_array_testing(check_tokens, TOKEN_COUNT, "test_program/comparison.pas");
}

void test_Valid_Character_Token(void)
{
	const int TOKEN_COUNT = 16;

	struct token check_tokens[17] = {
		{1, 1, SEMI, ";"},
		{2, 1, ELLIPSIS, ".."},
		{4, 1, POINT, "."},
		{5, 1, PLUS, "+"},
		{6, 1, MINUS, "-"},
		{7, 1, MULT, "*"},
		{8, 1, COLON, ":"},
		{9, 1, ASSIGMENT, ":="},
		{11, 1, L_PAREN, "("},
		{12, 1, R_PAREN, ")"},
		{13, 1, L_BRACE, "["},
		{14, 1, R_BRACE, "]"},
		{15, 1, COMMENT, "{my\nlong \ncomment}"},
		{9, 3, STRING_LITERAL, "\'string literal\'"},
		{25, 3, COMMA, ","},
		{1, 4, _EOF, "EOF"}};

	start_array_testing(check_tokens, TOKEN_COUNT, "test_program/valid_character.pas");
}

void test_Invalid_Character_Token(void)
{
	const int TOKEN_COUNT = 6;

	struct token check_tokens[6] = {
		{1, 1, UNKNOWN, "!#^\"}~`"},
		{9, 1, UNKNOWN, "#/"},
		{1, 2, UNKNOWN, "'literal "},
		{1, 3, STRING, "string"},
		{7, 3, UNKNOWN, "'"},
		{1, 4, UNKNOWN, "{my\nlong \ncomment\n"}};

	start_array_testing(check_tokens, TOKEN_COUNT, "test_program/invalid_character.pas");
}










/*********************************************************************************************/
/*...###########.......######.......###########...###########...###########...###########....*/
/*...###########......########......###########...###########...###########...###########....*/
/*...###.....###.....###....###.....###.....###...###......##...###...........###.....###....*/
/*...###.....###....###......###....###....####...###...........###...........###.....###....*/
/*...###########....############....###########...###########...###########...###########....*/
/*...###...........##############...#######...............###...###...........#######........*/
/*...###...........###........###...###..####.....##......###...###...........###..####......*/
/*...###...........###........###...###.....###...###########...###########...###.....###....*/
/*..#####.........#####......#####.#####....#####.###########...###########..#####....#####..*/
/*********************************************************************************************/

int *TestAST(struct AstNode *Node, struct AstNode *TestNode)
{
/*
	TEST_ASSERT_EQUAL_INT16(Node->Type, TestNode->Type);
	TEST_ASSERT_EQUAL_INT16(Node->X, TestNode->X);
	TEST_ASSERT_EQUAL_INT16(Node->Y, TestNode->Y);
*/

	switch (Node->Type)
	{
	case AstNodeRoot:

		TEST_ASSERT_EQUAL_INT16(Node->Root.ErrorCount, TestNode->Root.ErrorCount);

		if (Node->Root.DeclarationLink != NULL)
		{
			TestAST(Node->Root.DeclarationLink, TestNode->Root.DeclarationLink);
		}

		if (Node->Root.BodyLink != NULL)
		{
			TestAST(Node->Root.BodyLink, TestNode->Root.BodyLink);
		}

		break;

	case AstNodeDeclaration:

		TEST_ASSERT_EQUAL_INT16(Node->Declaration.VariableCount, TestNode->Declaration.VariableCount);

		for (int i = 0; i < Node->Declaration.VariableCount; i++)
		{

			TEST_ASSERT_EQUAL_STRING(Node->Declaration.ListVariable[i].ParamVariable.Name,
									 TestNode->Declaration.ListVariable[i].ParamVariable.Name);

			TEST_ASSERT_EQUAL_INT16(Node->Declaration.ListVariable[i].ParamVariable.ArrayFlag,
									TestNode->Declaration.ListVariable[i].ParamVariable.ArrayFlag);

			TEST_ASSERT_EQUAL_INT16(Node->Declaration.ListVariable[i].ParamVariable.First,
									TestNode->Declaration.ListVariable[i].ParamVariable.First);

			TEST_ASSERT_EQUAL_INT16(Node->Declaration.ListVariable[i].ParamVariable.Last,
									TestNode->Declaration.ListVariable[i].ParamVariable.Last);

			TEST_ASSERT_EQUAL_INT16(Node->Declaration.ListVariable[i].ParamVariable.Length,
									TestNode->Declaration.ListVariable[i].ParamVariable.Length);

			TEST_ASSERT_EQUAL_INT16(Node->Declaration.ListVariable[i].ParamVariable.StringSize,
									TestNode->Declaration.ListVariable[i].ParamVariable.StringSize);

			TEST_ASSERT_EQUAL_INT16(Node->Declaration.ListVariable[i].ParamVariable.VarType,
									TestNode->Declaration.ListVariable[i].ParamVariable.VarType);

			if (Node->Declaration.ListVariable[i].ParamVariable.VarType == Constant)
			{
				if (Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink != NULL)
				{
					TestAST(Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink,
							TestNode->Declaration.ListVariable[i].ParamVariable.ExpressionLink);
				}
			}
		}

		break;

	case AstNodeExpression:

		TEST_ASSERT_EQUAL_INT16(Node->Expression.Operation, TestNode->Expression.Operation);

		TEST_ASSERT_EQUAL_INT16(Node->Expression.ResultType, TestNode->Expression.ResultType);

		if (Node->Expression.LeftLink != NULL)
		{
			TestAST(Node->Expression.LeftLink, TestNode->Expression.LeftLink);
		}

		if (Node->Expression.RightLink != NULL)
		{
			TestAST(Node->Expression.RightLink, TestNode->Expression.RightLink);
		}

		break;

	case AstNodeConstant:

		TEST_ASSERT_EQUAL_INT16(Node->Constant.Type, TestNode->Constant.Type);

		TEST_ASSERT_EQUAL_STRING(Node->Constant.Value, TestNode->Constant.Value);

		break;

	case AstNodeBody:

		TEST_ASSERT_EQUAL_INT16(Node->Body.CommandCount, TestNode->Body.CommandCount);

		for (int i = 0; i < Node->Body.CommandCount; i++)
		{
			TestAST(&Node->Body.CommandList[i], &TestNode->Body.CommandList[i]);
		}

		break;

	case AstNodeBranch:

		if (Node->Branch.ConditionLink != NULL)
		{
			TestAST(Node->Branch.ConditionLink, TestNode->Branch.ConditionLink);
		}

		if (Node->Branch.IfBodyLink != NULL)
		{
			TestAST(Node->Branch.IfBodyLink, TestNode->Branch.IfBodyLink);
		}

		if (Node->Branch.ElseBodyLink != NULL)
		{
			TestAST(Node->Branch.ElseBodyLink, TestNode->Branch.ElseBodyLink);
		}

		break;

	case AstNodeAssign:

		if (Node->Assign.VariableLink != NULL)
		{
			TestAST(Node->Assign.VariableLink, TestNode->Assign.VariableLink);
		}

		if (Node->Assign.RightLink != NULL)
		{
			TestAST(Node->Assign.RightLink, TestNode->Assign.RightLink);
		}

		break;

	case AstNodeVariable:

		TEST_ASSERT_EQUAL_STRING(Node->Variable.Name, TestNode->Variable.Name);

		if (Node->Variable.OffsetExpression != NULL)
		{
			TestAST(Node->Variable.OffsetExpression, TestNode->Variable.OffsetExpression);
		}

		break;

	case AstNodeRead:

		TEST_ASSERT_EQUAL_INT16(Node->Read.VariableCount, TestNode->Read.VariableCount);

		for (int i = 0; i < Node->Read.VariableCount; i++)
		{
			TEST_ASSERT_EQUAL_STRING(Node->Read.ListVariable[i].Name, TestNode->Read.ListVariable[i].Name);

			if (Node->Read.ListVariable[i].OffsetExpression != NULL)
			{
				TestAST(Node->Read.ListVariable[i].OffsetExpression, TestNode->Read.ListVariable[i].OffsetExpression);
			}
		}

		break;

	case AstNodeVoid:

		break;

	case AstNodeWhile:

		if (Node->While.ConditionLink != NULL)
		{
			TestAST(Node->While.ConditionLink, TestNode->While.ConditionLink);
		}

		if (Node->While.BodyLink != NULL)
		{
			TestAST(Node->While.BodyLink, TestNode->While.BodyLink);
		}

		break;

	case AstNodeWrite:
		
		TEST_ASSERT_EQUAL_INT16(Node->Write.ArgCount, TestNode->Write.ArgCount);

		for (int i = 0; i < Node->Write.ArgCount; i++)
		{
			TestAST(&Node->Write.ExpressionList[i], &TestNode->Write.ExpressionList[i]);
		}

		break;

	default:
		break;
	}

	return 0;
}

struct AstNode *CreateAST_Declaration_And_Print_Constant()
{
	struct AstNode *TestAST = NULL;

	struct AstNode *RootNode = malloc(sizeof(struct AstNode));

	RootNode->Type = AstNodeRoot;
	RootNode->Root.ErrorCount = 0;
	RootNode->X = 0;
	RootNode->Y = 0;

	struct AstNode *DeclarationNode = malloc(sizeof(struct AstNode));
	
	RootNode->Root.DeclarationLink = DeclarationNode; 

	DeclarationNode->Type = AstNodeDeclaration;
	DeclarationNode->Declaration.RootLink = RootNode;
	DeclarationNode->Declaration.VariableCount = 2;
	DeclarationNode->Declaration.ListVariable = NULL;

	DeclarationNode->Declaration.ListVariable = malloc(sizeof(struct AstNode) * DeclarationNode->Declaration.VariableCount);

	/***************************************************************************************************************************/
	struct AstNode *Constant1ExpressionNode = NULL;

	Constant1ExpressionNode = malloc(sizeof(struct AstNode));

	Constant1ExpressionNode->Variable.ParentLink = NULL;
	Constant1ExpressionNode->Variable.OffsetExpression = NULL;
	Constant1ExpressionNode->Type = AstNodeConstant;
	Constant1ExpressionNode->Constant.Type = String;
	strcpy(Constant1ExpressionNode->Constant.Value, "'Hello, world'");
	Constant1ExpressionNode->Y = 1;
	Constant1ExpressionNode->X = 22;

	char Constant1Name[32] = "string_const";

	strcpy(DeclarationNode->Declaration.ListVariable[0].ParamVariable.Name, Constant1Name);
	DeclarationNode->Declaration.ListVariable[0].ParamVariable.VarType = Constant;
	DeclarationNode->Declaration.ListVariable[0].ParamVariable.ExpressionLink = Constant1ExpressionNode;
	DeclarationNode->Declaration.ListVariable[0].X = 7;
	DeclarationNode->Declaration.ListVariable[0].Y = 1;

	/***************************************************************************************************************************/
	/***************************************************************************************************************************/

	struct AstNode *Constant2ExpressionNode = NULL;

	/***************************************************************************************************************************/

	struct AstNode *Constant2ExpressionOperation1Node = NULL;

	Constant2ExpressionOperation1Node = malloc(sizeof(struct AstNode));

	Constant2ExpressionOperation1Node->Type = AstNodeExpression;
	Constant2ExpressionOperation1Node->Expression.Operation = _PLUS;
	Constant2ExpressionOperation1Node->Expression.ResultType = Constant;
	Constant2ExpressionOperation1Node->X = 23;
	Constant2ExpressionOperation1Node->Y = 2;

	/***************************************************************************************************************************/

	struct AstNode *Constant2ExpressionLeftNode = NULL;

	Constant2ExpressionLeftNode = malloc(sizeof(struct AstNode));
	Constant2ExpressionLeftNode->Variable.ParentLink = Constant2ExpressionOperation1Node;
	Constant2ExpressionLeftNode->Variable.OffsetExpression = NULL;
	Constant2ExpressionLeftNode->Type = AstNodeConstant;
	Constant2ExpressionLeftNode->Constant.Type = Integer;
	strcpy(Constant2ExpressionLeftNode->Constant.Value, "2");
	Constant2ExpressionLeftNode->Y = 2;
	Constant2ExpressionLeftNode->X = 19;
	Constant2ExpressionLeftNode->Constant.ParentLink = Constant2ExpressionOperation1Node;

	/***************************************************************************************************************************/

	struct AstNode *Constant2ExpressionRightNode = NULL;

	Constant2ExpressionRightNode = malloc(sizeof(struct AstNode));
	Constant2ExpressionRightNode->Type = AstNodeConstant;
	Constant2ExpressionRightNode->Constant.Type = Integer;
	strcpy(Constant2ExpressionRightNode->Constant.Value, "8");
	Constant2ExpressionRightNode->X = 25;
	Constant2ExpressionRightNode->Y = 2;
	Constant2ExpressionRightNode->Constant.ParentLink = Constant2ExpressionOperation1Node;

	/***************************************************************************************************************************/

	struct AstNode *Constant2ExpressionOperation2Node = NULL;

	Constant2ExpressionOperation2Node = malloc(sizeof(struct AstNode));

	Constant2ExpressionOperation2Node->Type = AstNodeExpression;
	Constant2ExpressionOperation2Node->Expression.Operation = _PLUS;
	Constant2ExpressionOperation2Node->Expression.ResultType = Constant;
	Constant2ExpressionOperation2Node->X = 29;
	Constant2ExpressionOperation2Node->Y = 2;

	/***************************************************************************************************************************/

	struct AstNode *Constant2ExpressionRightNodeRightNode = NULL;

	Constant2ExpressionRightNodeRightNode = malloc(sizeof(struct AstNode));
	Constant2ExpressionRightNodeRightNode->Type = AstNodeConstant;
	Constant2ExpressionRightNodeRightNode->Constant.Type = Integer;
	strcpy(Constant2ExpressionRightNodeRightNode->Constant.Value, "20");
	Constant2ExpressionRightNodeRightNode->X = 31;
	Constant2ExpressionRightNodeRightNode->Y = 2;
	Constant2ExpressionRightNodeRightNode->Constant.ParentLink = Constant2ExpressionOperation2Node;

	/***************************************************************************************************************************/

	Constant2ExpressionOperation2Node->Expression.ParentLink = &DeclarationNode->Declaration.ListVariable[1];
	Constant2ExpressionOperation2Node->Expression.LeftLink = Constant2ExpressionOperation1Node;
	Constant2ExpressionOperation2Node->Expression.RightLink = Constant2ExpressionRightNodeRightNode;
	Constant2ExpressionOperation1Node->Expression.ParentLink = Constant2ExpressionOperation2Node;
	Constant2ExpressionOperation1Node->Expression.LeftLink = Constant2ExpressionLeftNode;
	Constant2ExpressionOperation1Node->Expression.RightLink = Constant2ExpressionRightNode;

	Constant2ExpressionNode = Constant2ExpressionOperation2Node;

	char Constant2Name[32] = "integer_const";

	strcpy(DeclarationNode->Declaration.ListVariable[1].ParamVariable.Name, Constant2Name);
	DeclarationNode->Declaration.ListVariable[1].ParamVariable.VarType = Constant;
	DeclarationNode->Declaration.ListVariable[1].ParamVariable.ExpressionLink = Constant2ExpressionNode;
	DeclarationNode->Declaration.ListVariable[1].X = 3;
	DeclarationNode->Declaration.ListVariable[1].Y = 2;

	/***************************************************************************************************************************/
	/***************************************************************************************************************************/

	struct AstNode *Body1Node = NULL;

	Body1Node = malloc(sizeof(struct ReturnStruct));
	Body1Node->Type = AstNodeBody;
	Body1Node->Body.CommandCount = 1;
	Body1Node->Body.ParentLink = RootNode;
	RootNode->Root.BodyLink = Body1Node;
	Body1Node->X = 1;
	Body1Node->X = 4;

	Body1Node->Body.CommandList = malloc(sizeof(struct AstNode) * 1);
	Body1Node->Body.CommandList[0].Type = AstNodeWrite;
	Body1Node->Body.CommandList[0].X = 2;
	Body1Node->Body.CommandList[0].Y = 5;
	Body1Node->Body.CommandList[0].Write.ArgCount = 3;

	/***************************************************************************************************************************/

	struct AstNode *Write1Expression1 = NULL;

	Write1Expression1 = malloc(sizeof(struct AstNode));
	Write1Expression1->Type = AstNodeVariable;
	strcpy(Write1Expression1->Variable.Name, "integer_const");
	Write1Expression1->X = 10;
	Write1Expression1->Y = 5;

	/***************************************************************************************************************************/

	struct AstNode *Write1Expression2 = NULL;

	Write1Expression2 = malloc(sizeof(struct AstNode));
	Write1Expression2->Type = AstNodeConstant;
	Write1Expression2->Constant.Type = String;
	strcpy(Write1Expression2->Constant.Value, "' '");
	Write1Expression2->X = 25;
	Write1Expression2->Y = 5;

	/***************************************************************************************************************************/

	struct AstNode *Write1Expression3 = NULL;

	Write1Expression3 = malloc(sizeof(struct AstNode));
	Write1Expression3->Type = AstNodeVariable;
	strcpy(Write1Expression3->Variable.Name, "string_const");
	Write1Expression3->X = 30;
	Write1Expression3->Y = 5;

	/***************************************************************************************************************************/
	Body1Node->Body.CommandList[0].Write.ExpressionList = malloc(sizeof(struct AstNode) * 3);
	Body1Node->Body.CommandList[0].Write.ExpressionList[0] = *Write1Expression1;
	Body1Node->Body.CommandList[0].Write.ExpressionList[1] = *Write1Expression2;
	Body1Node->Body.CommandList[0].Write.ExpressionList[2] = *Write1Expression3;

	/***************************************************************************************************************************/
	/***************************************************************************************************************************/

	TestAST = RootNode;

	return TestAST;
}

void test_Declaration_And_Print_Constant(void)
{
	struct AstNode *AST = NULL;
	struct AstNode *TestAst = NULL;

	AST = ParseProgram("test_program/ast_declaration_and_print_constant.pas");
	TestAst = CreateAST_Declaration_And_Print_Constant();

	TestAST(AST, TestAst);
}