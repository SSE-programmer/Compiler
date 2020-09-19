#include "identifier_table.h"

int HashFunction(char *Name)
{
	int h = 0;
	while (*Name)
		h += *Name++;
	return h % TABLE_SIZE;
}

HashNode *InsertNode(char *Name, struct IdentifierAttributes *INode, HashNode **Table)
{
	HashNode *P, *P0;
	int Bucket;

	Bucket = HashFunction(Name);

	if ((P = malloc(sizeof(HashNode))) == 0)
	{
		fprintf(stderr, "\033[01;31mЗакончилась память при добавлении узла в Hash-таблицу;\033[0m\n");
		exit(1);
	}

	P0 = Table[Bucket];
	Table[Bucket] = P;
	P->Next = P0;
	P->Data = INode;

	return P;
}

HashNode *FindNode(char *Name, HashNode **Table)
{
	HashNode *Node = NULL;

	Node = Table[HashFunction(Name)];

	while (Node)
	{
		if (strcmp(Node->Data->Name, Name) == 0)
			return Node;
		Node = Node->Next;
	}
	return NULL;
}

HashNode **CreateHashTable(struct AstNode *Root)
{
	struct AstNode *Point = NULL;

	HashNode **HashTable;

	HashTable = (HashNode **)malloc(TABLE_SIZE * sizeof(HashNode *));

	if (Root != NULL)
	{
		if (Root->Root.DeclarationLink != NULL)
		{
			Point = Root->Root.DeclarationLink;
		}
	}

	if (Point != NULL)
	{
		for (int i = 0; i < Point->Declaration.VariableCount; i++)
		{
			struct IdentifierAttributes *VariableUnit;

			if (FindNode(Point->Declaration.ListVariable[i].ParamVariable.Name, HashTable) != NULL)
			{
				printf("\033[01;31m<%d:%d> Ошибка объявления переменной/константы:\033[0m переменная с именем \"%s\" была объявлена ранее;\n",
					   Point->Declaration.ListVariable[i].Y, Point->Declaration.ListVariable[i].X, Point->Declaration.ListVariable[i].ParamVariable.Name);
				Root->Root.ErrorCount++;
				continue;
			}

			VariableUnit = (struct IdentifierAttributes *)malloc(sizeof(struct IdentifierAttributes));

			strcpy(VariableUnit->Name, Point->Declaration.ListVariable[i].ParamVariable.Name);
			if (Point->Declaration.ListVariable[i].ParamVariable.VarType != Constant)
			{
				VariableUnit->Volatile = 1;
			}
			else
			{
				VariableUnit->Volatile = 0;
				VariableUnit->Initialized = 1;
			}
			switch (Point->Declaration.ListVariable[i].ParamVariable.VarType)
			{
			case String:
				VariableUnit->Size_Bytes = 1;
				break;
			case Integer:
				VariableUnit->Size_Bytes = 4;
				break;
			default:
				VariableUnit->Size_Bytes = -1;
				break;
			}
			VariableUnit->Alignment_Bytes = 0;
			if (Point->Declaration.ListVariable[i].ParamVariable.ArrayFlag != 0)
			{
				VariableUnit->Type = _Array;
			}
			else
			{
				switch (Point->Declaration.ListVariable[i].ParamVariable.VarType)
				{
				case String:
					VariableUnit->Type = _String;
					break;

				case Integer:
					VariableUnit->Type = _Integer;

				default:
					break;
				}
			}
			switch (Point->Declaration.ListVariable[i].ParamVariable.VarType)
			{
			case String:
				VariableUnit->BaseType = _String;
				break;

			case Integer:
				VariableUnit->BaseType = _Integer;

			default:
				break;
			}
			if (VariableUnit->BaseType == _Integer)
			{
				VariableUnit->AmountOfElements = Point->Declaration.ListVariable[i].ParamVariable.Length;
			}
			else
			{
				VariableUnit->AmountOfElements =
					Point->Declaration.ListVariable[i].ParamVariable.Length * Point->Declaration.ListVariable[i].ParamVariable.StringSize;
			}
			
			VariableUnit->Bias = 0 - Point->Declaration.ListVariable[i].ParamVariable.First;

			VariableUnit->Initialized = 0;

			InsertNode(Point->Declaration.ListVariable[i].ParamVariable.Name, VariableUnit, HashTable);
		}
	}

	return HashTable;
}

void PrintIdentifierAttributes(struct IdentifierAttributes *Attributes)
{
	printf("\n");
	printf("Имя: %s\n", Attributes->Name);
	printf("Изменчивость: %s\n", Attributes->Volatile != 0 ? "Да" : "Нет");
	printf("Размер в байтах: %d\n", Attributes->Size_Bytes);
	printf("Выравнивание в байтах: %d\n", Attributes->Alignment_Bytes);
	switch (Attributes->Type)
	{
	case _Array:
		printf("Тип: Array\n");
		break;
	case _Integer:
		printf("Тип: Integer\n");
		break;
	case _String:
		printf("Тип: String\n");
		break;
	default:
		break;
	}
	switch (Attributes->BaseType)
	{
	case _Array:
		printf("Базовый тип: Array\n");
		break;
	case _Integer:
		printf("Базовый тип: Integer\n");
		break;
	case _String:
		printf("Базовый тип: String\n");
		break;
	default:
		break;
	}
	printf("Количество элементов: %d\n", Attributes->AmountOfElements);
}

void PrintHashTable(HashNode **Table)
{
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		if (Table[i] != NULL)
		{
			PrintIdentifierAttributes(Table[i]->Data);

			HashNode *Point = Table[i];

			while (Point->Next)
			{
				Point = Point->Next;
				PrintIdentifierAttributes(Point->Data);
			}
		}
	}
}