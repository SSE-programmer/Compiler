#include "semantic_analysis.h"

VariableType CheckingVariableType(struct AstNode *ExpressionNode, HashNode **Table, struct AstNode *RootLink)
{
	VariableType LeftValueType, RightValueType;

	switch (ExpressionNode->Type)
	{
	case AstNodeExpression:
		if (ExpressionNode->Expression.ResultType == Constant)
		{
			if (ExpressionNode->Expression.Operation == _UNARY_MINUS)
			{
				ExpressionNode->Expression.ResultType = Integer;
				return Integer;
			}
			else
			{
				LeftValueType = CheckingVariableType(ExpressionNode->Expression.LeftLink, Table, RootLink);
				RightValueType = CheckingVariableType(ExpressionNode->Expression.RightLink, Table, RootLink);

				if (LeftValueType == -1 || RightValueType == -1)
				{
					return -1;
				}

				if (LeftValueType == RightValueType)
				{
					if (LeftValueType == String)
					{

						if (ExpressionNode->Expression.LeftLink->Type == AstNodeExpression ||
							ExpressionNode->Expression.RightLink->Type == AstNodeExpression)
						{
							printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m Допускается только одиночное сравнение двух элементов строк;\n",
								   ExpressionNode->Y, ExpressionNode->X, ExpressionNode->Variable.Name);
							RootLink->Root.ErrorCount++;

							return -1;
						}

						/*****************************************************************************************/
						if (ExpressionNode->Expression.LeftLink->Type == AstNodeConstant)
						{
							if (strlen(ExpressionNode->Expression.LeftLink->Constant.Value) > 3)
							{
								printf("\033[01;31m<%d:%d> Семантическая ошибка:\033[0m могут сравниваться только одиночные символы;\n",
									   ExpressionNode->Expression.LeftLink->Y, ExpressionNode->Expression.LeftLink->X);
								RootLink->Root.ErrorCount++;

								return -1;
							}
						}
						else if (ExpressionNode->Expression.LeftLink->Type == AstNodeVariable)
						{
							if (ExpressionNode->Expression.LeftLink->Variable.OffsetExpression == NULL)
							{
								printf("\033[01;31m<%d:%d> Семантическая ошибка:\033[0m могут сравниваться только одиночные символы;\n",
									   ExpressionNode->Expression.LeftLink->Y, ExpressionNode->Expression.LeftLink->X);
								RootLink->Root.ErrorCount++;

								return -1;
							}
						}

						if (ExpressionNode->Expression.RightLink->Type == AstNodeConstant)
						{
							if (strlen(ExpressionNode->Expression.RightLink->Constant.Value) > 3)
							{
								printf("\033[01;31m<%d:%d> Семантическая ошибка:\033[0m могут сравниваться только одиночные символы;\n",
									   ExpressionNode->Expression.RightLink->Y, ExpressionNode->Expression.RightLink->X);
								RootLink->Root.ErrorCount++;

								return -1;
							}
						}
						else if (ExpressionNode->Expression.RightLink->Type == AstNodeVariable)
						{
							if (ExpressionNode->Expression.RightLink->Variable.OffsetExpression == NULL)
							{
								printf("\033[01;31m<%d:%d> Семантическая ошибка:\033[0m могут сравниваться только одиночные символы;\n",
									   ExpressionNode->Expression.RightLink->Y, ExpressionNode->Expression.RightLink->X);
								RootLink->Root.ErrorCount++;

								return -1;
							}
						}

						/*****************************************************************************************/

						if (ExpressionNode->Expression.Operation == _EQUAL)
						{
							ExpressionNode->Expression.ResultType = String;
							return LeftValueType;
						}
						else
						{
							printf("\033[01;31m<%d:%d> ", ExpressionNode->Y, ExpressionNode->X);

							switch (ExpressionNode->Expression.Operation)
							{
							case _AND:
								printf(" \"AND\" ");
								break;

							case _MULT:
								printf(" \"*\" ");
								break;

							case _DIV:
								printf(" \"DIV\" ");
								break;

							case _MOD:
								printf(" \"MOD\" ");
								break;

							case _OR:
								printf(" \"OR\" ");
								break;

							case _PLUS:
								printf(" \"+\" ");
								break;

							case _MINUS:
								printf(" \"-\" ");
								break;

							case _EQUAL:
								printf(" \"=\" ");
								break;

							case _NOT_EQUAL:
								printf(" \"<>\" ");
								break;

							case _LESS:
								printf(" \"<\" ");
								break;

							case _LARGER:
								printf(" \">\" ");
								break;

							case _LARGER_OR_EQUAL:
								printf(" \">=\" ");
								break;

							case _LESS_OR_EQUAL:
								printf(" \"<=\" ");
								break;

							default:
								break;
							}

							printf("Семантическая ошибка:\033[0m недопустимая операция для строк (возможно только сравнение двух элементов строк);\n");
							RootLink->Root.ErrorCount++;

							return -1;
						}
					}
					else
					{
						ExpressionNode->Expression.ResultType = Integer;
						return LeftValueType;
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d>", ExpressionNode->Y, ExpressionNode->X);
					switch (ExpressionNode->Expression.Operation)
					{
					case _AND:
						printf(" \"AND\" ");
						break;

					case _MULT:
						printf(" \"*\" ");
						break;

					case _DIV:
						printf(" \"DIV\" ");
						break;

					case _MOD:
						printf(" \"MOD\" ");
						break;

					case _OR:
						printf(" \"OR\" ");
						break;

					case _PLUS:
						printf(" \"+\" ");
						break;

					case _MINUS:
						printf(" \"-\" ");
						break;

					case _EQUAL:
						printf(" \"=\" ");
						break;

					case _NOT_EQUAL:
						printf(" \"<>\" ");
						break;

					case _LESS:
						printf(" \"<\" ");
						break;

					case _LARGER:
						printf(" \">\" ");
						break;

					case _LARGER_OR_EQUAL:
						printf(" \">=\" ");
						break;

					case _LESS_OR_EQUAL:
						printf(" \"<=\" ");
						break;

					default:
						break;
					}
					printf("Семантическая ошибка:\033[0m недопустимая операция для");
					switch (LeftValueType)
					{
					case Integer:
						printf(" INTEGER ");
						break;

					case String:
						printf(" STRING ");
						break;

					default:
						break;
					}
					printf("и");
					switch (RightValueType)
					{
					case Integer:
						printf(" INTEGER ");
						break;

					case String:
						printf(" STRING ");
						break;

					default:
						break;
					}
					printf("(несоответствие типов);\n");

					RootLink->Root.ErrorCount++;

					return -1;
				}
			}
		}
		else
		{
			return ExpressionNode->Expression.ResultType;
		}

		break;
	case AstNodeVariable:;
		HashNode *TempNode;
		TempNode = FindNode(ExpressionNode->Variable.Name, Table);
		if (TempNode != NULL)
		{
			if (TempNode->Data->Initialized == 0 && TempNode->Data->Volatile == 1)
			{
				printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m используется неинициализированная переменная;\n",
					   ExpressionNode->Y, ExpressionNode->X, ExpressionNode->Variable.Name);
				RootLink->Root.ErrorCount++;

				return -1;
			}

			switch (TempNode->Data->Type)
			{
			case _Integer:
				if (ExpressionNode->Variable.OffsetExpression != NULL)
				{
					printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m Указан индекс у целочисленной переменной;\n",
						   ExpressionNode->Y, ExpressionNode->X, ExpressionNode->Variable.Name);
					RootLink->Root.ErrorCount++;

					return -1;
				}

				return Integer;

			case _String:
			{
				VariableType IndexType;

				if (ExpressionNode->Variable.OffsetExpression != NULL)
				{
					IndexType = CheckingVariableType(ExpressionNode->Variable.OffsetExpression, Table, RootLink);

					if (IndexType != _Integer)
					{
						printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m индекс строки не целочисленный;\n",
							   ExpressionNode->Y, ExpressionNode->X, ExpressionNode->Variable.Name);
						RootLink->Root.ErrorCount++;

						return -1;
					}
				}
				else
				{
					if (ExpressionNode->Variable.ParentLink != NULL && ExpressionNode->Variable.ParentLink->Type == AstNodeExpression)
					{
						printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m не указан индекс строки;\n",
							   ExpressionNode->Y, ExpressionNode->X, ExpressionNode->Variable.Name);
						RootLink->Root.ErrorCount++;

						return -1;
					}
				}

				return String;
			}

			case _Array:
				if (ExpressionNode->Variable.OffsetExpression != NULL)
				{
					CheckingVariableType(ExpressionNode->Variable.OffsetExpression, Table, RootLink);

					switch (TempNode->Data->BaseType)
					{
					case _Integer:
						return Integer;

					case _String:
						return String;

					case _Array:
						return _Array;

					default:
						return -1;
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m не указан индекс массива;\n",
						   ExpressionNode->Y, ExpressionNode->X, ExpressionNode->Variable.Name);
					RootLink->Root.ErrorCount++;

					return -1;
				}
			}
		}
		else
		{
			printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m переменная не объявлена;\n",
				   ExpressionNode->Y, ExpressionNode->X, ExpressionNode->Variable.Name);
			RootLink->Root.ErrorCount++;

			return -1;
		}

		break;

	case AstNodeConstant:
		switch (ExpressionNode->Constant.Type)
		{
		case Integer:
			return Integer;

		case String:
			return String;

		default:
			return -1;
		}

		break;

	default:
		break;
	}

	return -1;
}

int CheckingConstants(struct AstNode *DeclarationNode, HashNode **Table, struct AstNode *RootLink)
{
	VariableType ConstantType;

	for (int i = 0; i < DeclarationNode->Declaration.VariableCount; i++)
	{
		if (DeclarationNode->Declaration.ListVariable[i].ParamVariable.VarType == Constant)
		{
			ConstantType = CheckingVariableType(DeclarationNode->Declaration.ListVariable[i].ParamVariable.ExpressionLink, Table, RootLink);

			if (ConstantType != -1)
			{
				HashNode *TempNode;
				TempNode = FindNode(DeclarationNode->Declaration.ListVariable[i].ParamVariable.Name, Table);

				TempNode->Data->Type = ConstantType;
				TempNode->Data->BaseType = ConstantType;

				switch (ConstantType)
				{
				case Integer:
					TempNode->Data->Size_Bytes = 4;
					break;

				case String:
					TempNode->Data->Size_Bytes = 1;
					break;

				default:
					break;
				}
			}
		}

		/***********************************************/
		/*Проверка массива (На то, что он не строковый)*/
		/***********************************************/

		if (DeclarationNode->Declaration.ListVariable[i].ParamVariable.VarType == String)
		{
			if (DeclarationNode->Declaration.ListVariable[i].ParamVariable.ArrayFlag == 1)
			{
				printf("\033[01;31mСемантическая ошибка:\033[0m переменная %s является массивом строк (допускаются только целочисленные массивы);\n",
					   DeclarationNode->Declaration.ListVariable[i].ParamVariable.Name);
				RootLink->Root.ErrorCount++;

				continue;
			}
		}
	}

	return 0;
}

int CheckingCommandExpression(struct AstNode *BodyNode, HashNode **Table, struct AstNode *RootLink)
{
	VariableType ExpressionType;
	HashNode *TempNode;

	if (BodyNode == NULL)
	{
		return -1;
	}

	for (int i = 0; i < BodyNode->Body.CommandCount; i++)
	{
		switch (BodyNode->Body.CommandList[i].Type)
		{
		case AstNodeAssign:
		{
			VariableType IndexType;

			TempNode = FindNode(BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name, Table);

			if (TempNode == NULL)
			{
				printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m переменная не объявлена;\n",
					   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
				RootLink->Root.ErrorCount++;

				break;
			}

			if (TempNode->Data->Volatile == 0)
			{
				printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m недопускается изменение значения константы;\n",
					   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
				RootLink->Root.ErrorCount++;

				break;
			}

			if (TempNode->Data->Type == _Integer)
			{
				if (BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression != NULL)
				{
					printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m Указан индекс у целочисленной переменной;\n",
						   BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
					RootLink->Root.ErrorCount++;

					break;
				}
			}

			if (TempNode->Data->Type == _Array)
			{
				if (BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression != NULL)
				{
					IndexType = CheckingVariableType(BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression, Table, RootLink);

					if (IndexType != _Integer)
					{
						printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m индекс массива не целочисленный;\n",
							   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
						RootLink->Root.ErrorCount++;

						break;
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m не указан индекс массива;\n",
						   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
					RootLink->Root.ErrorCount++;

					break;
				}
			}

			if (TempNode->Data->Type == _String)
			{
				if (BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression != NULL)
				{
					IndexType = CheckingVariableType(BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression, Table, RootLink);

					if (IndexType != _Integer)
					{
						printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m индекс массива не целочисленный;\n",
							   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
						RootLink->Root.ErrorCount++;

						break;
					}
				}

				if (BodyNode->Body.CommandList[i].Assign.RightLink->Type == AstNodeConstant)
				{
					if (BodyNode->Body.CommandList[i].Assign.RightLink->Constant.Type != String)
					{
						printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m строковой переменной присваивается целочисленное значение;\n",
							   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
						RootLink->Root.ErrorCount++;

						break;
					}

					if (BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.OffsetExpression != NULL)
					{
						if (strlen(BodyNode->Body.CommandList[i].Assign.RightLink->Constant.Value) > 3)
						{
							printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m элементу строки может присваиваться только одиночный символ;\n",
								   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
							RootLink->Root.ErrorCount++;

							break;
						}
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m строковой переменной может присваиваться только строковая константа;\n",
						   BodyNode->Body.CommandList[i].Assign.VariableLink->Y, BodyNode->Body.CommandList[i].Assign.VariableLink->X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
					RootLink->Root.ErrorCount++;

					break;
				}
			}

			ExpressionType = CheckingVariableType(BodyNode->Body.CommandList[i].Assign.RightLink, Table, RootLink);

			if (ExpressionType == -1)
			{
				break;
			}

			int LeftType, RightType;

			switch (TempNode->Data->BaseType)
			{
			case _Integer:
				LeftType = 0;
				break;

			case _String:
				LeftType = 1;
				break;

			default:
				break;
			}

			switch (ExpressionType)
			{
			case Integer:
				RightType = 0;
				break;

			case String:
				RightType = 1;
				break;

			default:
				break;
			}

			if (LeftType != RightType)
			{
				printf("\033[01;31m<%d:%d> \"=\" ", BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X);

				printf("Семантическая ошибка:\033[0m переменной типа");
				switch (TempNode->Data->BaseType)
				{
				case _Integer:
					printf(" INTEGER ");
					break;

				case _String:
					printf(" STRING ");
					break;

				default:
					break;
				}
				printf("присваевается выражение, результат которого типа");
				switch (ExpressionType)
				{
				case Integer:
					printf(" INTEGER ");
					break;

				case String:
					printf(" STRING ");
					break;

				default:
					break;
				}
				printf("(несоответствие типов);\n");

				RootLink->Root.ErrorCount++;

				return -1;
			}

			TempNode->Data->Initialized = 1;

			break;
		}

		case AstNodeWhile:
		{
			VariableType ConditionType;

			ConditionType = CheckingVariableType(BodyNode->Body.CommandList[i].While.ConditionLink, Table, RootLink);

			if (ConditionType == -1)
			{
				break;
			}

			CheckingCommandExpression(BodyNode->Body.CommandList[i].While.BodyLink, Table, RootLink);

			break;
		}

		case AstNodeBranch:
		{
			VariableType ConditionType;

			ConditionType = CheckingVariableType(BodyNode->Body.CommandList[i].Branch.ConditionLink, Table, RootLink);

			if (ConditionType == -1)
			{
				break;
			}

			if (BodyNode->Body.CommandList[i].Branch.IfBodyLink != NULL)
			{
				CheckingCommandExpression(BodyNode->Body.CommandList[i].Branch.IfBodyLink, Table, RootLink);
			}

			if (BodyNode->Body.CommandList[i].Branch.ElseBodyLink != NULL)
			{
				CheckingCommandExpression(BodyNode->Body.CommandList[i].Branch.ElseBodyLink, Table, RootLink);
			}

			break;
		}

		case AstNodeWrite:
			for (int j = 0; j < BodyNode->Body.CommandList[i].Write.ArgCount; j++)
			{
				VariableType ArgumentType;

				ArgumentType = CheckingVariableType(&BodyNode->Body.CommandList[i].Write.ExpressionList[j], Table, RootLink);
			}

			break;

		case AstNodeRead:
			for (int j = 0; j < BodyNode->Body.CommandList[i].Read.VariableCount; j++)
			{
				HashNode *TempNode;

				TempNode = FindNode(BodyNode->Body.CommandList[i].Read.ListVariable[j].Name, Table);

				if (TempNode != NULL)
				{
					if (TempNode->Data->Volatile == 0)
					{
						printf("\033[01;31m<%d:%d> \"READLN\" Семантическая ошибка:\033[0m не допускается считывание данных в константу %s;\n",
							   BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X, BodyNode->Body.CommandList[i].Read.ListVariable[j].Name);
						RootLink->Root.ErrorCount++;

						continue;
					}

					if (TempNode->Data->Type == _Integer)
					{
						if (BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression != NULL)
						{
							printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m Указан индекс у целочисленной переменной;\n",
								   BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X, BodyNode->Body.CommandList[i].Read.ListVariable[j].Name);
							RootLink->Root.ErrorCount++;

							continue;
						}
					}

					TempNode->Data->Initialized = 1;

					if (TempNode->Data->Type == _Array)
					{
						if (BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression == NULL)
						{
							printf("\033[01;31m<%d:%d> \"READLN\" Семантическая ошибка:\033[0m не указан индекс массива %s;\n",
								   BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X, BodyNode->Body.CommandList[i].Read.ListVariable[j].Name);
							RootLink->Root.ErrorCount++;
						}
						else
						{
							VariableType IndexType;

							IndexType = CheckingVariableType(BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression, Table, RootLink);

							if (IndexType != _Integer)
							{
								printf("\033[01;31m<%d:%d> \"READLN\" Семантическая ошибка:\033[0m индекс массива/строки %s не целочисленный;\n",
									   BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X, BodyNode->Body.CommandList[i].Read.ListVariable[j].Name);
								RootLink->Root.ErrorCount++;

								continue;
							}
						}
					}

					if (TempNode->Data->Type == _String)
					{
						VariableType IndexType;

						if (BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression != NULL)
						{
							IndexType = CheckingVariableType(BodyNode->Body.CommandList[i].Read.ListVariable[j].OffsetExpression, Table, RootLink);

							if (IndexType != _Integer)
							{
								printf("\033[01;31m<%d:%d> \"%s\" Семантическая ошибка:\033[0m индекс массива не целочисленный;\n",
									   BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X, BodyNode->Body.CommandList[i].Assign.VariableLink->Variable.Name);
								RootLink->Root.ErrorCount++;

								break;
							}
						}
					}
				}
				else
				{
					printf("\033[01;31m<%d:%d> \"READLN\" Семантическая ошибка:\033[0m переменная %s не объявлена;\n",
						   BodyNode->Body.CommandList[i].Y, BodyNode->Body.CommandList[i].X, BodyNode->Body.CommandList[i].Read.ListVariable[j].Name);
					RootLink->Root.ErrorCount++;
				}
			}

			break;

		default:
			break;
		}
	}

	return 0;
}