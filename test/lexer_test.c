
#include "../source/lexer.h"
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
		{1, 2, IDENTIFIER, "FUNCTION"},
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
