#include "code_generator.h"

int AsmRodataSection(FILE *FAsm)
{
	fprintf(FAsm, ".section .rodata\n");
	fprintf(FAsm, "decimal_format:\n");
	fprintf(FAsm, "\t.string \"%%d\"\n\n");
	fprintf(FAsm, "decimal_format_nl:\n");
	fprintf(FAsm, "\t.string \"%%d\\n\"\n\n");
	fprintf(FAsm, "string_format:\n");
	fprintf(FAsm, "\t.string \"%%s\"\n\n");
	fprintf(FAsm, "string_format_read:\n");
	fprintf(FAsm, "\t.string \"%%255[^\\r\\n]\"\n\n");
	fprintf(FAsm, "string_format_symbol:\n");
	fprintf(FAsm, "\t.string \"%%c\"\n\n");
	fprintf(FAsm, "string_format_symbol_nl:\n");
	fprintf(FAsm, "\t.string \"%%c\\n\"\n\n");
	fprintf(FAsm, "string_format_nl:\n");
	fprintf(FAsm, "\t.string \"%%s\\n\"\n\n");
	fprintf(FAsm, "format_nl:\n");
	fprintf(FAsm, "\t.string \"\\n\"\n\n");

	return 0;
}

int AsmCodeGenerator(struct AstNode *AstTree, HashNode **IdentifierTable, int FileOut)
{
	for (int i = 0; i < ASM_SYMBOL_COUNT; i++)
	{
		Counter[i] = 0;
	}

	FILE *FAsm;
	char FileName[] = "assembler/assembler.s";

	if (FileOut == 1)
	{
		if ((FAsm = fopen(FileName, "w")) == NULL)
		{
			printf("\033[01;31mОшибка:\033[0m Не удалось открыть файл для записи кода ассемблера;\n");
			return 0;
		}
	}
	else
	{
		FAsm = stdout;
	}

	fprintf(FAsm, ".code32\n\n");

	AsmVariableDeclaration(FAsm, AstTree, IdentifierTable);

	fprintf(FAsm, ".text\n");
	fprintf(FAsm, ".globl main\n");
	fprintf(FAsm, ".type main, @function\n");
	fprintf(FAsm, "main:\n");
	fprintf(FAsm, "\tpushl %%ebp\n");
	fprintf(FAsm, "\tmovl %%esp, %%ebp\n\n");

	AsmConstantDeclaration(FAsm, AstTree, IdentifierTable);

	AsmBodyCommands(FAsm, AstTree->Root.BodyLink, IdentifierTable);

	fprintf(FAsm, "\tmovl $0, %%eax\n");
	fprintf(FAsm, "\tleave\n");
	fprintf(FAsm, "\tret\n\n");

	AsmRodataSection(FAsm);

	fclose(FAsm);

	return 0;
}

int AsmConstantDeclaration(FILE *FAsm, struct AstNode *AstTree, HashNode **IdentifierTable)
{
	struct AstNode *Node = AstTree->Root.DeclarationLink;

	if (Node == NULL || Node->Declaration.VariableCount <= 0)
	{
		return 0;
	}

	for (int i = 0; i < Node->Declaration.VariableCount; i++)
	{
		HashNode *VarNode;
		VarNode = FindNode(Node->Declaration.ListVariable[i].ParamVariable.Name, IdentifierTable);

		if (VarNode->Data->Volatile != 1)
		{
			if (VarNode->Data->BaseType == _Integer)
			{
				EmitExpression(FAsm, Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink, IdentifierTable);

				fprintf(FAsm, "\tpopl  %%eax\n");
				fprintf(FAsm, "\tmovl  %%eax, %s\n", VarNode->Data->Name);
			}
			else
			{
				for (int j = 1; j < strlen(Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink->Constant.Value) - 1; j++)
				{
					fprintf(FAsm, "\tmovl  $%d, %%ebx\n", j - 1);
					fprintf(FAsm, "\tmovb  $%d, %s(,%%ebx,1)\n", Node->Declaration.ListVariable[i].ParamVariable.ExpressionLink->Constant.Value[j], Node->Declaration.ListVariable[i].ParamVariable.Name);
				}
			}
		}
	}

	return 0;
}

int AsmVariableDeclaration(FILE *FAsm, struct AstNode *AstTree, HashNode **IdentifierTable)
{
	struct AstNode *Node = AstTree->Root.DeclarationLink;

	if (Node == NULL)
	{
		return 0;
	}

	if (Node->Declaration.VariableCount > 0)
	{
		fprintf(FAsm, ".bss");
		fprintf(FAsm, "\n");
	}
	else
	{
		return 0;
	}

	for (int i = 0; i < Node->Declaration.VariableCount; i++)
	{
		HashNode *VariableHashNode;
		VariableHashNode = FindNode(Node->Declaration.ListVariable[i].ParamVariable.Name, IdentifierTable);

		if (VariableHashNode->Data->Volatile != 1)
		{
			fprintf(FAsm, "%s:\n", VariableHashNode->Data->Name);
			fprintf(FAsm, "\t\t.space ");
			if (VariableHashNode->Data->BaseType == _Integer)
			{
				fprintf(FAsm, "%d", VariableHashNode->Data->Size_Bytes);
			}
			else
			{
				fprintf(FAsm, "%d", VariableHashNode->Data->Size_Bytes * 255);
			}
			fprintf(FAsm, "\n");
			fprintf(FAsm, "\n");

			continue;
		}

		fprintf(FAsm, "%s:\n", VariableHashNode->Data->Name);
		fprintf(FAsm, "\t\t.space ");
		fprintf(FAsm, "%d", VariableHashNode->Data->Size_Bytes * VariableHashNode->Data->AmountOfElements);
		fprintf(FAsm, "\n");
		fprintf(FAsm, "\n");
	}

	return 0;
}

int AsmBodyCommands(FILE *FAsm, struct AstNode *BodyNode, HashNode **IdentifierTable)
{
	for (int i = 0; i < BodyNode->Body.CommandCount; i++)
	{
		switch (BodyNode->Body.CommandList[i].Type)
		{
		case AstNodeWrite:
		{
			for (int j = 0; j < BodyNode->Body.CommandList[i].Write.ArgCount; j++)
			{
				int Type;

				fprintf(FAsm, "\txorl %%eax, %%eax\t/*Начало программы вывода параметра writeln*/\n");

				Type = EmitExpression(FAsm, &BodyNode->Body.CommandList[i].Write.ExpressionList[j], IdentifierTable);

				if (Type == 1)
				{
					fprintf(FAsm, "\tpopl %%eax\n");

					fprintf(FAsm, "\tpushl %%eax\n");
					if (j + 1 != BodyNode->Body.CommandList[i].Write.ArgCount)
					{
						fprintf(FAsm, "\tpushl $decimal_format\n");
					}
					else
					{
						fprintf(FAsm, "\tpushl $decimal_format_nl\n");
					}
					fprintf(FAsm, "\tcall printf\n");
					fprintf(FAsm, "\taddl $8, %%esp\n");
				}
				else
				{
					if (BodyNode->Body.CommandList[i].Write.ExpressionList[j].Type == AstNodeConstant)
					{
						if (BodyNode->Body.CommandList[i].Write.ExpressionList[j].Constant.Type == String)
						{
							for (int k = 1; k < strlen(BodyNode->Body.CommandList[i].Write.ExpressionList[j].Constant.Value) - 1; k++)
							{
								fprintf(FAsm, "\tpushl $%d\n", BodyNode->Body.CommandList[i].Write.ExpressionList[j].Constant.Value[k]);
								if (k == strlen(BodyNode->Body.CommandList[i].Write.ExpressionList[j].Constant.Value) - 2 
									&& j == BodyNode->Body.CommandList[i].Write.ArgCount)
								{
									fprintf(FAsm, "\tpushl $string_format_symbol_nl\n");
								}
								else
								{
									fprintf(FAsm, "\tpushl $string_format_symbol\n");
								}
								fprintf(FAsm, "\tcall printf\n");
							}
						}
					}
					else if (BodyNode->Body.CommandList[i].Write.ExpressionList[j].Type == AstNodeVariable)
					{
						if (BodyNode->Body.CommandList[i].Write.ExpressionList[j].Variable.OffsetExpression == NULL)
						{
							fprintf(FAsm, "\tleal %s, %%eax\n", BodyNode->Body.CommandList[i].Write.ExpressionList[j].Variable.Name);
							fprintf(FAsm, "\tpushl %%eax\n");
							fprintf(FAsm, "\tpushl $string_format_nl\n");
							fprintf(FAsm, "\tcall printf\n");
						}
						else
						{
							EmitExpression(FAsm, BodyNode->Body.CommandList[i].Write.ExpressionList[j].Variable.OffsetExpression, IdentifierTable);
							fprintf(FAsm, "\tpopl  %%ebx\n");
							fprintf(FAsm, "\tleal -1(%%ebx), %%ebx\n");
							fprintf(FAsm, "\tmovl %s(,%%ebx,1), %%eax\n", BodyNode->Body.CommandList[i].Write.ExpressionList[j].Variable.Name);
							fprintf(FAsm, "\tpushl %%eax\n");
							fprintf(FAsm, "\tpushl $string_format_symbol_nl\n");
							fprintf(FAsm, "\tcall printf\n");
						}
					}
				}
			}
		}
		break;

		case AstNodeAssign:
		{
			HashNode *VarNode;

			VarNode = FindNode(BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name, IdentifierTable);

			if (VarNode->Data->BaseType == _Integer)
			{
				if (VarNode->Data->Type == _Integer)
				{
					EmitExpression(FAsm, BodyNode->Body.CommandList[i].Assign.RightLink, IdentifierTable);

					fprintf(FAsm, "\tpopl  %%eax\n");
					fprintf(FAsm, "\tmovl  %%eax, %s\n", VarNode->Data->Name);
				}
				else
				{
					EmitExpression(FAsm, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression, IdentifierTable);
					EmitExpression(FAsm, BodyNode->Body.CommandList[i].Assign.RightLink, IdentifierTable);
					fprintf(FAsm, "\tpopl  %%eax\n");
					fprintf(FAsm, "\tpopl  %%ebx\n");
					fprintf(FAsm, "\tleal %d(%%ebx), %%ebx\n", VarNode->Data->Bias);
					fprintf(FAsm, "\tmovl  %%eax, %s(,%%ebx,4)\n", VarNode->Data->Name);
				}
			}
			else
			{
				if (BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression == NULL)
				{
					for (int j = 1; j < strlen(BodyNode->Body.CommandList[i].Assign.RightLink->Constant.Value) - 1; j++)
					{
						fprintf(FAsm, "\tmovl  $%d, %%ebx\n", j - 1);
						fprintf(FAsm, "\tmovb  $%d, %s(,%%ebx,1)\n", BodyNode->Body.CommandList[i].Assign.RightLink->Constant.Value[j], BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
					}
				}
				else
				{
					EmitExpression(FAsm, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression, IdentifierTable);
					fprintf(FAsm, "\tpopl  %%ebx\n");
					fprintf(FAsm, "\tleal -1(%%ebx), %%ebx\n");
					fprintf(FAsm, "\tmovb  $%d, %s(,%%ebx,1)\n", BodyNode->Body.CommandList[i].Assign.RightLink->Constant.Value[1], BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
				}
			}
		}

		break;

		case AstNodeWhile:
		{
			int Count = Counter[ASM_LOOP];
			Counter[ASM_LOOP]++;

			fprintf(FAsm, "\tjmp check%d\n", Count);
			fprintf(FAsm, "loop_start%d:\n", Count);
			AsmBodyCommands(FAsm, BodyNode->Body.CommandList[i].While.BodyLink, IdentifierTable);
			fprintf(FAsm, "check%d:\n", Count);
			EmitExpression(FAsm, BodyNode->Body.CommandList[i].While.ConditionLink, IdentifierTable);
			fprintf(FAsm, "\tpopl %%eax\n");
			fprintf(FAsm, "\tcmpl $0, %%eax\n");
			fprintf(FAsm, "\tjg loop_start%d\n", Count);
		}

		break;

		case AstNodeBranch:
		{
			int Count = Counter[ASM_BRANCH];
			Counter[ASM_BRANCH]++;

			EmitExpression(FAsm, BodyNode->Body.CommandList[i].Branch.ConditionLink, IdentifierTable);
			fprintf(FAsm, "\tpopl %%eax\n");
			fprintf(FAsm, "\tcmpl $0, %%eax\n");
			fprintf(FAsm, "\tjg if_body%d\n", Count);
			if (BodyNode->Body.CommandList[i].Branch.ElseBodyLink != NULL)
			{
				AsmBodyCommands(FAsm, BodyNode->Body.CommandList[i].Branch.ElseBodyLink, IdentifierTable);
			}
			fprintf(FAsm, "\tjmp continue%d\n", Count);
			fprintf(FAsm, "if_body%d:\n", Count);
			AsmBodyCommands(FAsm, BodyNode->Body.CommandList[i].Branch.IfBodyLink, IdentifierTable);
			fprintf(FAsm, "continue%d:\n", Count);
		}

		break;

		case AstNodeRead:
		{
			HashNode *VarNode;

			for (int j = 0; j < BodyNode->Body.CommandList[i].Read.VariableCount; j++)
			{
				VarNode = FindNode(BodyNode->Body.CommandList[i].Read.ListVariable[j].Name, IdentifierTable);

				if (VarNode->Data->BaseType == _Integer)
				{
					if (VarNode->Data->Type == _Integer)
					{
						fprintf(FAsm, "\tleal %s, %%eax\n", VarNode->Data->Name);
						fprintf(FAsm, "\tpushl %%eax\n");
						fprintf(FAsm, "\tpushl $decimal_format\n");
						fprintf(FAsm, "\tcall scanf\n");
					}
					else
					{
						EmitExpression(FAsm, BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression, IdentifierTable);
						fprintf(FAsm, "\tpopl  %%ebx\n");
						fprintf(FAsm, "\tleal %d(%%ebx), %%ebx\n", VarNode->Data->Bias);
						fprintf(FAsm, "\tleal %s(,%%ebx,4), %%eax\n", VarNode->Data->Name);
						fprintf(FAsm, "\tpushl %%eax\n");
						fprintf(FAsm, "\tpushl $decimal_format\n");
						fprintf(FAsm, "\tcall scanf\n");
					}
				}
				else
				{
					if (BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression == NULL)
					{
						fprintf(FAsm, "\tleal %s, %%eax\n", VarNode->Data->Name);
						fprintf(FAsm, "\tpushl %%eax\n");
						fprintf(FAsm, "\tpushl $string_format_read\n");
						fprintf(FAsm, "\tcall scanf\n");
					}
					else
					{
						EmitExpression(FAsm, BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression, IdentifierTable);
						fprintf(FAsm, "\tpopl  %%ebx\n");
						fprintf(FAsm, "\tleal -1(%%ebx), %%ebx\n");
						fprintf(FAsm, "\tleal %s(,%%ebx,1), %%eax\n", VarNode->Data->Name);
						fprintf(FAsm, "\tpushl %%eax\n");
						fprintf(FAsm, "\tpushl $string_format_symbol\n");
						fprintf(FAsm, "\tcall scanf\n");
					}
				}
				fprintf(FAsm, "\tcall getchar\n");
			}
		}

		break;

		default:
			break;
		}
	}

	return 0;
}

int EmitExpression(FILE *FAsm, struct AstNode *ExpressionNode, HashNode **IdentifierTable)
{
	int Type = 0;

	switch (ExpressionNode->Type)
	{
	case AstNodeConstant:
		if (ExpressionNode->Constant.Type == Integer)
		{
			fprintf(FAsm, "\tpushl  $%s\n", ExpressionNode->Constant.Value);
			Type = 1;
		}
		else if (ExpressionNode->Constant.Type == String)
		{
			fprintf(FAsm, "/*Константу-строку пушим в стек*/\n");
			fprintf(FAsm, "\tpushl  $%d\n", ExpressionNode->Constant.Value[1]);
			Type = 2;
		}

		break;

	case AstNodeVariable:
	{
		HashNode *VarNode;

		VarNode = FindNode(ExpressionNode->Variable.Name, IdentifierTable);

		if (VarNode->Data->BaseType == _Integer)
		{
			if (VarNode->Data->Type == _Integer)
			{
				fprintf(FAsm, "\tpushl  %s\n", VarNode->Data->Name);
			}
			else
			{
				EmitExpression(FAsm, ExpressionNode->Variable.OffsetExpression, IdentifierTable);
				fprintf(FAsm, "\tpopl  %%ebx\n");
				fprintf(FAsm, "\tleal %d(%%ebx), %%ebx\n", VarNode->Data->Bias);
				fprintf(FAsm, "\tpushl %s(,%%ebx, 4)\n", VarNode->Data->Name);
			}

			Type = 1;
		}
		else
		{
			if (ExpressionNode->Variable.OffsetExpression != NULL)
			{
				EmitExpression(FAsm, ExpressionNode->Variable.OffsetExpression, IdentifierTable);
				fprintf(FAsm, "\tpopl  %%ebx\n");
				fprintf(FAsm, "\tleal -1(%%ebx), %%ebx\n");
				fprintf(FAsm, "\tpushl %s(,%%ebx, 1)\n", VarNode->Data->Name);
			}
		}
	}
	break;

	case AstNodeExpression:
	{
		if (ExpressionNode->Expression.ResultType == Integer)
		{
			Type = 1;

			if (ExpressionNode->Expression.LeftLink != NULL)
			{
				EmitExpression(FAsm, ExpressionNode->Expression.LeftLink, IdentifierTable);
			}

			if (ExpressionNode->Expression.RightLink != NULL && ExpressionNode->Expression.Operation != _UNARY_MINUS)
			{
				EmitExpression(FAsm, ExpressionNode->Expression.RightLink, IdentifierTable);
			}

			switch (ExpressionNode->Expression.Operation)
			{
			case _AND:
			{
				int Count = Counter[ASM_AND];
				Counter[ASM_AND]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp $0, %%eax\n");
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "\tjle AND%d_1_not_true\n", Count);
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "AND%d_1_not_true:\n", Count);

				fprintf(FAsm, "\tcmp $0, %%ebx\n");
				fprintf(FAsm, "\tmovl $0, %%ebx\n");
				fprintf(FAsm, "\tjle AND%d_2_not_true\n", Count);
				fprintf(FAsm, "\tmovl $1, %%ebx\n");
				fprintf(FAsm, "AND%d_2_not_true:\n", Count);

				fprintf(FAsm, "\tandl %%ebx, %%eax\n");

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _MULT:
			{
				fprintf(FAsm, "\tpopl %%eax\n");
				fprintf(FAsm, "\tpopl %%edx\n");

				fprintf(FAsm, "\timull %%edx\n");

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _DIV:
			{
				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");
				fprintf(FAsm, "\tmovl %%eax, %%edx\n");
				fprintf(FAsm, "\tshr $16, %%edx\n");

				fprintf(FAsm, "\tidiv %%ebx\n");
				fprintf(FAsm, "\tand $0xFF, %%eax\n");

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _MOD:
			{
				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");
				fprintf(FAsm, "\tmovl %%eax, %%edx\n");
				fprintf(FAsm, "\tshr $16, %%edx\n");

				fprintf(FAsm, "\tidiv %%ebx\n");
				fprintf(FAsm, "\tand $0xFF, %%edx\n");

				fprintf(FAsm, "\tpushl %%edx\n");
			}

			break;

			case _OR:
			{
				int Count = Counter[ASM_OR];
				Counter[ASM_OR]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp $0, %%eax\n");
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "\tjle OR%d_1_not_true\n", Count);
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "OR%d_1_not_true:\n", Count);

				fprintf(FAsm, "\tcmp $0, %%ebx\n");
				fprintf(FAsm, "\tmovl $0, %%ebx\n");
				fprintf(FAsm, "\tjle OR%d_2_not_true\n", Count);
				fprintf(FAsm, "\tmovl $1, %%ebx\n");
				fprintf(FAsm, "OR%d_2_not_true:\n", Count);

				fprintf(FAsm, "\torl %%ebx, %%eax\n");

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _PLUS:
			{
				fprintf(FAsm, "\tpopl %%edx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\taddl %%edx, %%eax\n");

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _MINUS:
			{
				fprintf(FAsm, "\tpopl %%edx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tsubl %%edx, %%eax\n");

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _UNARY_MINUS:
			{
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tneg %%eax\n");

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _EQUAL:
			{
				int Count = Counter[ASM_EQUAL];
				Counter[ASM_EQUAL]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp %%ebx, %%eax\n");
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "\tje EQUAL%d\n", Count);
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "EQUAL%d:\n", Count);

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _NOT_EQUAL:
			{
				int Count = Counter[ASM_NOT_EQUAL];
				Counter[ASM_NOT_EQUAL]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp %%ebx, %%eax\n");
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "\tjne NOT_EQUAL%d\n", Count);
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "NOT_EQUAL%d:\n", Count);

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _LESS:
			{
				int Count = Counter[ASM_LESS];
				Counter[ASM_LESS]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp %%ebx, %%eax\n");
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "\tjl LESS%d\n", Count);
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "LESS%d:\n", Count);

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _LARGER:
			{
				int Count = Counter[ASM_LARGER];
				Counter[ASM_LARGER]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp %%ebx, %%eax\n");
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "\tjg LARGER%d\n", Count);
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "LARGER%d:\n", Count);

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _LARGER_OR_EQUAL:
			{
				int Count = Counter[ASM_LARGER_OR_EQUAL];
				Counter[ASM_LARGER_OR_EQUAL]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp %%ebx, %%eax\n");
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "\tjge LARGER_OR_EQUAL%d\n", Count);
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "LARGER_OR_EQUAL%d:\n", Count);

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			case _LESS_OR_EQUAL:
			{
				int Count = Counter[ASM_LESS_OR_EQUAL];
				Counter[ASM_LESS_OR_EQUAL]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp %%ebx, %%eax\n");
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "\tjle LESS_OR_EQUAL%d\n", Count);
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "LESS_OR_EQUAL%d:\n", Count);

				fprintf(FAsm, "\tpushl %%eax\n");
			}

			break;

			default:
				break;
			}
		}
		else if (ExpressionNode->Expression.ResultType == String)
		{
			Type = 2;

			if (ExpressionNode->Expression.LeftLink != NULL)
			{
				EmitExpression(FAsm, ExpressionNode->Expression.LeftLink, IdentifierTable);
			}

			if (ExpressionNode->Expression.RightLink != NULL)
			{
				EmitExpression(FAsm, ExpressionNode->Expression.RightLink, IdentifierTable);
			}

			if (ExpressionNode->Expression.Operation == _EQUAL)
			{
				int Count = Counter[ASM_EQUAL];
				Counter[ASM_EQUAL]++;

				fprintf(FAsm, "\tpopl %%ebx\n");
				fprintf(FAsm, "\tpopl %%eax\n");

				fprintf(FAsm, "\tcmp %%bl, %%al\n");
				fprintf(FAsm, "\tmovl $1, %%eax\n");
				fprintf(FAsm, "\tje EQUAL%d\n", Count);
				fprintf(FAsm, "\tmovl $0, %%eax\n");
				fprintf(FAsm, "EQUAL%d:\n", Count);

				fprintf(FAsm, "\tpushl %%eax\n");
			}
			else
			{
				/**********************Магия какая-то**********************/
			}
		}
	}
	break;

	default:
		break;
	}

	return Type;
}