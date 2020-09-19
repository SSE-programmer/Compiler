#include "lexer.h" 

char TypeStringMass[N_TSM][LEN_TSM] = {
        {"ASSIGMENT"},
        {"BEGIN"},
        {"END"},
        {"IF"},
        {"THEN"},
        {"ELSE"},
        {"WHILE"},
        {"DO"},
        {"AND"},
        {"OR"},
        {"CONST"},
        {"VAR"},
        {"ARRAY"},
        {"OF"},
        {"PLUS"},
        {"MINUS"},
        {"MULT"},
        {"DIV"},
        {"MOD"},
        {"INTEGER"},
        {"STRING"},
        {"STRING_LITERAL"},
        {"L_PAREN"},
        {"R_PAREN"},
        {"COMMENT"},
        {"SEMI"},
        {"COLON"},
        {"POINT"},
        {"COMMA"},
        {"_EOF"},
        {"UNKNOWN"},
        {"EQUAL"},
        {"NOT_EQUAL"},
        {"LESS"},
        {"LARGER"},
        {"LESS_OR_EQUAL"},
        {"LARGER_OR_EQUAL"},
        {"NUM_2"},
        {"NUM_8"},
        {"NUM_10"},
        {"NUM_16"},
        {"IDENTIFIER"},
        {"L_BRACE"},
        {"R_BRACE"},
        {"ELLIPSIS"},
        {"READLN"},
        {"WRITELN"}
};

struct identificator{
    char Name[LEN_ID];
    TClass Class;
};

static struct identificator Identificator[N_ID] = {
        {"BEGIN", BEGIN},
        {"END", END},
        {"IF", IF},
        {"THEN", THEN},
        {"ELSE", ELSE},
        {"WHILE", WHILE},
        {"DO", DO},
        {"CONST", CONST},
        {"VAR", VAR},
        {"INTEGER", INTEGER},
        {"STRING", STRING},
        {"ARRAY", ARRAY},
        {"OF", OF},
        {"DIV", DIV},
        {"MOD", MOD},
        {"OR", OR},
        {"AND", AND},
        {"READLN", READLN},
        {"WRITELN", WRITELN}
};


static int fpeek(FILE *stream, struct stream_cursor_position *stream_cursor_position) {
    char ch = fgetc(stream);
    stream_cursor_position->X++;
    if (ch == '\n') {
        stream_cursor_position->X = 0;
        stream_cursor_position->Y++;
    }
    return ch;
}

static int funpeek(char ch, FILE *stream, struct stream_cursor_position *stream_cursor_position) {
    if (ch != '\n')
    {
        ungetc(ch, stream);
        stream_cursor_position->X--;
    }
    return 0;
}

FILE *open_program_file(const char *name, struct stream_cursor_position *stream_cursor_position) {
    FILE *f;
    if ((f = fopen(name, "rb")) == NULL) {
        printf("Ошибка при открытии файла\n");
        exit(201);
    }
    stream_cursor_position->X = 0, stream_cursor_position->Y = 1;
    return f;
}

int close_program_file(FILE *name) {
    fclose(name);

    return 0;
}

static int fill_struct(struct token * token, char *lexeme, TClass token_class)
{
    strcpy(token->Lexeme, lexeme);
    token->Class = token_class;

    return 0;
}

static int unvisible_ch(char ch) {
    if (ch == '\r' || ch == '\t' || ch == '\n' || ch == '\b' || ch == ' ')
        return 1;

    return 0;
}

static int keyword(char *word) {
    int k_w_index = -1;

    for (int i = 0; i < N_ID; i++)
        if (!strcasecmp(word, Identificator[i].Name))
                    k_w_index = Identificator[i].Class;

    return k_w_index;
}

static void alpha(FILE *source_code, char ch, struct token *pointer_token, struct stream_cursor_position *stream_cursor_position) {
    int i = 0;
    short token_class;
    char word[256];

    while (i < 255 && (isalpha(ch) || isdigit(ch) || ch == '_')) {       
	word[i] = ch;
        ch = fpeek(source_code, stream_cursor_position);
        i++;
    }
    word[i] = '\0';

    funpeek(ch, source_code, stream_cursor_position);

    if ((token_class = keyword(word)) >= 0) {
        fill_struct(pointer_token, word, token_class);
    } else {
        for(int i=0; i < sizeof(word)-1; i++)
        {
            if(word[i]>'A' && word[i]<'Z') word[i]+='z'-'Z';
        }
        fill_struct(pointer_token, word, IDENTIFIER);
    }
    return;
}

static int digit(FILE *source_code, char ch, struct token *pointer_token, struct stream_cursor_position *stream_cursor_position) {
    int i = 0;
    short token_class;
    char word[256];
    short flag = 0;

    if (isdigit(ch)) {
        while (i < 255 && (isdigit(ch) || isalpha(ch) || ch == '_')) {
            if (isalpha(ch) || ch == '_')
                flag = 1;
            word[i] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            i++;
        }

        if (flag == 0)
            token_class = NUM_10;
        else
            token_class = UNKNOWN;

        if (i == 1 && !(ch == '\'' || ch == EOF || unvisible_ch(ch) || ch == ';' || ch == '.' || ch == '=' || ch == '<' ||
                           ch == '>' || ch == '+' || ch == '-' || ch == '*' || ch == ':' || ch == '(' || 
                           ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '\'' || ch == ',' ))
                           {
                               funpeek(ch, source_code, stream_cursor_position);
                           }

    } else if (ch == '$') {
        word[i] = ch;
        ch = fpeek(source_code, stream_cursor_position);
        i++;
        while (i < 255 && (isdigit(ch) || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C'
                           || ch == 'd' || ch == 'D' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F' ||
                           isalpha(ch) || ch == '_')) {
            if ((isalpha(ch) && !(ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C'
                                 || ch == 'd' || ch == 'D' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F')) ||
                                    ch == '_')
                flag = 1;
            word[i] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            i++;
        }
        if (flag == 0)
            token_class = NUM_16;
        else
            token_class = UNKNOWN;
    } else if (ch == '&') {
        word[i] = ch;
        ch = fpeek(source_code, stream_cursor_position);
        i++;
        while (i < 255 && (isdigit(ch) || isalpha(ch) || ch == '_')) {
            if (ch != '0' && ch != '1' && ch != '2' && ch != '3' && ch != '4' && ch != '5' && ch != '6'
                && ch != '7')
                flag = 1;
            word[i] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            i++;
        }
        if (flag == 0)
            token_class = NUM_8;
        else
            token_class = UNKNOWN;
    } else if (ch == '%') {
        word[i] = ch;
        ch = fpeek(source_code, stream_cursor_position);
        i++;
        while (i < 255 && (isdigit(ch) || isalpha(ch) || ch == '_')) {
            if (isalpha(ch) || ch == '_' || !(ch == '0' || ch == '1'))
                flag = 1;
            word[i] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            i++;
        }
        if (flag == 0)
            token_class = NUM_2;
        else
            token_class = UNKNOWN;
    }
    word[i] = '\0';
    
    if (i != 1)
        funpeek(ch, source_code, stream_cursor_position);
    else if (ch == '\'' || ch == EOF || unvisible_ch(ch) || ch == ';' || ch == '.' || ch == '=' || ch == '<' ||
                           ch == '>' || ch == '+' || ch == '-' || ch == '*' || ch == ':' || ch == '(' || 
                           ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '\'' || ch == ',')
                           {
                               funpeek(ch, source_code, stream_cursor_position);
                           }

    if (!isdigit(word[0]) && word[1] == '\0') {
        int j = 1;
        while (j < 255 && (ch != '\'' && ch != EOF && !unvisible_ch(ch) && !isalpha(ch) && !isdigit(ch) &&
                           ch != ';' && ch != '.' && ch != '=' && ch != '<' &&
                           ch != '>' && ch != '+' && ch != '-' && ch != '*' && ch != ':' && ch != '(' &&
                           ch != ')' && ch != '[' && ch != ']' && ch != '{' && ch != '\'' && ch != ',')) {
            word[j] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            j++;
        }
        if (word[j - 1] == '%' && (ch == '0' || ch == '1'))
        {
            funpeek(ch, source_code, stream_cursor_position);
            funpeek(word[j - 1], source_code, stream_cursor_position);
        }
        else  if (word[j - 1] == '$' && (isdigit(ch) || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C'
                           || ch == 'd' || ch == 'D' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F'))
        {
            funpeek(ch, source_code, stream_cursor_position);
            funpeek(word[j - 1], source_code, stream_cursor_position);
        }
        else if(word[j - 1] == '&' && (ch >= '0' && ch <= '7'))
        {
            funpeek(ch, source_code, stream_cursor_position);
            funpeek(word[j - 1], source_code, stream_cursor_position);
        }
        else 
        {
            funpeek(ch, source_code, stream_cursor_position);
        }
        word[j] = '\0';
        fill_struct(pointer_token, word, UNKNOWN);
    } else {
        fill_struct(pointer_token, word, token_class);
    }

    return 0;
}

struct token next_token(FILE *source_code, struct stream_cursor_position *stream_cursor_position) {
    char ch;
    struct token token;

    ch = fpeek(source_code, stream_cursor_position);
    
    while (unvisible_ch(ch)) {
        ch = fpeek(source_code, stream_cursor_position);
    }

    token.X = stream_cursor_position->X;
    token.Y = stream_cursor_position->Y;

    if (isalpha(ch) || ch == '_') {
        alpha(source_code, ch, &token, stream_cursor_position);
    } else if (isdigit(ch) || ch == '$' || ch == '&' || ch == '%') {
        digit(source_code, ch, &token, stream_cursor_position);
    } else if (ch == ';') {
        fill_struct(&token, ";", SEMI);
    } else if (ch == '.') {
        ch = fpeek(source_code, stream_cursor_position);
        if (ch == '.') {
            fill_struct(&token, "..", ELLIPSIS);
        } else {
            fill_struct(&token, ".", POINT);
            funpeek(ch, source_code, stream_cursor_position);
        }
    } else if (ch == '=') {
        fill_struct(&token, "=", EQUAL);
    } else if (ch == '>') {
        ch = fpeek(source_code, stream_cursor_position);
        if (ch == '=') {
            fill_struct(&token, ">=", LARGER_OR_EQUAL);
        } else {
            fill_struct(&token, ">", LARGER);
            funpeek(ch, source_code, stream_cursor_position);
        }
    } else if (ch == '<') {
        ch = fpeek(source_code, stream_cursor_position);
        if (ch == '=') {
            fill_struct(&token, "<=", LESS_OR_EQUAL);
        } else if (ch == '>') {
            fill_struct(&token, "<>", NOT_EQUAL);
        } else {
            fill_struct(&token, "<", LESS);
            funpeek(ch, source_code, stream_cursor_position);
        }
    } else if (ch == '+') {
        fill_struct(&token, "+", PLUS);
    } else if (ch == '-') {
        fill_struct(&token, "-", MINUS);
    } else if (ch == '*') {
        fill_struct(&token, "*", MULT);
    } else if (ch == ':') {
        ch = fpeek(source_code, stream_cursor_position);
        if (ch == '=') {
            fill_struct(&token, ":=", ASSIGMENT);
        } else {
            fill_struct(&token, ":", COLON);
            funpeek(ch, source_code, stream_cursor_position);
        }
    } else if (ch == '(') {
        fill_struct(&token, "(", L_PAREN);
    } else if (ch == ')') {
        fill_struct(&token, ")", R_PAREN);
    } else if (ch == '[') {
        fill_struct(&token, "[", L_BRACE);
    } else if (ch == ']') {
        fill_struct(&token, "]", R_BRACE);
    } else if (ch == '{') {
        int i = 0;
        char word[256];
        while (i < 255 && ch != '}' && ch != EOF) {
            word[i] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            i++;
        }
        if (ch == EOF) {
            word[i] = '\0';
            fill_struct(&token, word, UNKNOWN);
            funpeek(ch, source_code, stream_cursor_position);
        } else {
            word[i] = '}';
            word[i + 1] = '\0';
            fill_struct(&token, word, COMMENT);
        }
    } else if (ch == '\'') {
        int i = 0;
        char word[256];
        word[i] = ch;
        ch = fpeek(source_code, stream_cursor_position);
        i++;
        while (i < 255 && ch != '\'' && ch != EOF && ch != '\n') {
            word[i] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            i++;
        }
        if (ch == EOF) {
            word[i] = '\0';
            fill_struct(&token, word, UNKNOWN);
            funpeek(ch, source_code, stream_cursor_position);
        } else if (ch == '\n') {
            word[i] = '\0';
            fill_struct(&token, word, UNKNOWN);
        } else {
            word[i] = '\'';
            word[i + 1] = '\0';
            fill_struct(&token, word, STRING_LITERAL);
        }

    } else if (ch == ',') {
        fill_struct(&token, ",", COMMA);
    } else if (ch == EOF) {
        fill_struct(&token, "EOF", _EOF);
    } else {
        int i = 0;
        char word[256];
        while (i < 255 && (ch != '\'' && ch != EOF && !unvisible_ch(ch) && !isalpha(ch) && !isdigit(ch) &&
                           ch != ';' && ch != '.' && ch != '=' && ch != '<' &&
                           ch != '>' && ch != '+' && ch != '-' && ch != '*' && ch != ':' && ch != '(' &&
                           ch != ')' && ch != '[' && ch != ']' && ch != '{' && ch != '\'' && ch != ',' && ch != EOF)) {
            word[i] = ch;
            ch = fpeek(source_code, stream_cursor_position);
            i++;
        }
        if (word[i - 1] == '%' && (ch == '0' || ch == '1'))
        {
            funpeek(ch, source_code, stream_cursor_position);
            funpeek(word[i - 1], source_code, stream_cursor_position);
        }
        else  if (word[i - 1] == '$' && (isdigit(ch) || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C'
                           || ch == 'd' || ch == 'D' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F'))
        {
            funpeek(ch, source_code, stream_cursor_position);
            funpeek(word[i - 1], source_code, stream_cursor_position);
        }
        else if(word[i - 1] == '&' && (ch >= '0' && ch <= '7'))
        {
            funpeek(ch, source_code, stream_cursor_position);
            funpeek(word[i - 1], source_code, stream_cursor_position);
        }
        else 
        {
            funpeek(ch, source_code, stream_cursor_position);
        }
        word[i] = '\0';

        fill_struct(&token, word, UNKNOWN);
    }
    return token;
}

struct token NextToken(FILE *source_code, struct stream_cursor_position *stream_cursor_position)
{
    struct token TempToken;
    
    do {
        TempToken = next_token(source_code, stream_cursor_position);
    } while (TempToken.Class == COMMENT);

    return TempToken;
}

int print_token(struct token token) {
    if (token.Class == UNKNOWN)
        printf("\033[01;31m");
    printf("<%d:%d>\t%s", token.Y, token.X, TypeStringMass[token.Class]);
    for (int j = 0; j < LEN_TSM - strlen(TypeStringMass[token.Class]); j++)
        printf(" ");
    printf("\t%s\n", token.Lexeme);
    printf("\033[0m");
    return 0;
}
