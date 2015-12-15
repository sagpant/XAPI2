#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "tdx_function.h"
#include "tdx_enum.h"
#include "tdx_field.h"
#include "tdx_request.h"

using namespace std;

#ifdef TDXAPI_EXPORTS

/*
注意：以下代码只是几个数据表的读写方式，当做示例使用
此cpp请不要添加到项目中，而是只添加h文件
因为由哪里生成的内存块就由哪里删除，由dll生成的内存，由dll中的函数来删
如果把这个函数编译到exe中，会导致是由外部的exe中函数来删除了，会出错

*/

void PrintTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		char buf[512] = { 0 };
		printf("%d_%s|",//"%d,%s,%d,%d,%d,%d,%d",
			pRow->FieldID, pRow->FieldName, pRow->a, pRow->b, pRow->Len, pRow->d, pRow->e);

		++i;
		pRow = ppHeader[i];
	}
	printf("\n");
}

void OutputCSVTableHeader(FILE* pFile, FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr || pFile == nullptr)
		return;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		char buf[512] = { 0 };
		fprintf(pFile,"%d_%s,",//"%d,%s,%d,%d,%d,%d,%d",
			pRow->FieldID, pRow->FieldName, pRow->a, pRow->b, pRow->Len, pRow->d, pRow->e);

		++i;
		pRow = ppHeader[i];
	}
	fprintf(pFile, "\n");
}

FieldInfo_STRUCT** CopyTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return nullptr;

	int count = GetCountTableHeader(ppHeader)+1;
	FieldInfo_STRUCT** ppNew = new FieldInfo_STRUCT*[count];
	ppNew[count - 1] = nullptr;
	
	for (int i = 0; i < count;++i)
	{
		//ppNew[i] = ppHeader[i];
		ppNew[i] = new FieldInfo_STRUCT;
		memcpy(ppNew[i], ppHeader[i], sizeof(FieldInfo_STRUCT));
	}
	return ppNew;
}

void DeleteTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return;

	//原有创建方法是一个连续区域，使用时容易出错
	//delete[] ppHeader;

	int i = 0;
	while (ppHeader[i] != 0)
	{
		delete[] ppHeader[i];

		++i;
	}

	delete[] ppHeader;
}

int GetIndexByFieldName(FieldInfo_STRUCT** ppHeader, char* FieldName)
{
	if (ppHeader == nullptr)
		return -1;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		if (strcmp(pRow->FieldName,FieldName)==0)
		{
			return i;
		}

		++i;
		pRow = ppHeader[i];
	}
	return -1;
}

int GetIndexByFieldID(FieldInfo_STRUCT** ppHeader, int FieldID)
{
	if (ppHeader == nullptr)
		return -1;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		if (pRow->FieldID == FieldID)
		{
			return i;
		}

		++i;
		pRow = ppHeader[i];
	}
	return -1;
}

int GetCountTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return -1;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		++i;
		pRow = ppHeader[i];
	}
	return i;
}

void PrintTableBody(char** ppTable)
{
	if (ppTable == nullptr)
		return;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		printf("%d:",i);
		for (j = 0; j < COL_EACH_ROW; ++j)
		{
			p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				printf("%s|", p);
			}
			else
				break;
		}
		printf("\n");
		j = 0;
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	return;
}

void PrintTableBody(char** ppTable, int count)
{
	if (ppTable == nullptr)
		return;

	for (int i = 0; i < count;++i)
	{
		printf("%d:", i);
		for (int j = 0; j < COL_EACH_ROW; ++j)
		{
			char* p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				printf("%s|", p);
				
			}
			else
				break;
		}
		printf("\n");
	}

	return;
}

void OutputCSVTableBody(FILE* pFile, char** ppTable)
{
	if (ppTable == nullptr || pFile == nullptr)
		return;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		//printf("%d:", i);
		for (j = 0; j < COL_EACH_ROW; ++j)
		{
			p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				fprintf(pFile, "%s,", p);
			}
			else
				break;
		}
		fprintf(pFile, "\n");
		j = 0;
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	return;
}

void OutputCSVTableBody(FILE* pFile, char** ppTable, int count)
{
	if (ppTable == nullptr || pFile == nullptr)
		return;

	for (int i = 0; i < count; ++i)
	{
		//printf("%d:", i);
		for (int j = 0; j < COL_EACH_ROW; ++j)
		{
			char* p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				fprintf(pFile, "%s,", p);

			}
			else
				break;
		}
		fprintf(pFile, "\n");
	}

	return;
}

// 得到某一行某一列
char* GetAtTableBody(char** ppTable, int row, int col)
{
	if (ppTable == nullptr)
		return nullptr;

	if (col >= COL_EACH_ROW)
		return nullptr;

	if (row >= GetRowCountTableBody(ppTable))
	{
		return nullptr;
	}

	return ppTable[row * COL_EACH_ROW + col];
}

int GetRowCountTableBody(char** ppTable)
{
	if (ppTable == nullptr)
		return -1;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	return i;
}

void DeleteTableBody(char** ppTable)
{
	// 要注意的是什么时候停止删除
	if (ppTable == nullptr)
		return;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		for (j = 0; j < COL_EACH_ROW; ++j)
		{
			delete[] ppTable[i * COL_EACH_ROW + j];
		}
		j = 0;
		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	delete[] ppTable;

	return;
}

void DeleteTableBody(char** ppTable, int count)
{
	if (ppTable == nullptr)
		return;

	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < COL_EACH_ROW; ++j)
		{
			char* p = ppTable[i * COL_EACH_ROW + j];
			if (p)
			{
				delete[] ppTable[i * COL_EACH_ROW + j];
			}
			else
				break;
		}
	}

	delete[] ppTable;
}

void PrintError(Error_STRUCT* pErr)
{
	if (pErr == nullptr)
	{
		return;
	}

	printf("%d,%d,%s\n", pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);
}

void PrintErrors(Error_STRUCT** pErrs)
{
	if (pErrs == nullptr)
		return;

	int i = 0;
	Error_STRUCT* pErr = pErrs[i];
	while (pErr != nullptr)
	{
		printf("%d:%d,%d,%s\n",i, pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);

		++i;
		pErr = pErrs[i];
	}
}

void PrintErrors(Error_STRUCT** pErrs, int count)
{
	if (pErrs == nullptr)
	{
		return;
	}

	for (int i = 0; i < count;++i)
	{
		Error_STRUCT* pErr = pErrs[i];
		if (pErr)
		{
			printf("%d:%d,%d,%s\n", i, pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);
		}
	}
}

void OutputCSVError(FILE* pFile, Error_STRUCT* pErr)
{
	if (pErr == nullptr)
	{
		return;
	}

	fprintf(pFile, "%d,%d,%s\n", pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);
}

void OutputCSVErrors(FILE* pFile, Error_STRUCT** pErrs)
{
	if (pErrs == nullptr)
		return;

	int i = 0;
	Error_STRUCT* pErr = pErrs[i];
	while (pErr != nullptr)
	{
		fprintf(pFile, "%d:%d,%d,%s\n", i, pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);

		++i;
		pErr = pErrs[i];
	}
}

void OutputCSVErrors(FILE* pFile, Error_STRUCT** pErrs, int count)
{
	if (pErrs == nullptr)
	{
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		Error_STRUCT* pErr = pErrs[i];
		if (pErr)
		{
			fprintf(pFile, "%d:%d,%d,%s\n", i, pErr->ErrType, pErr->ErrCode, pErr->ErrInfo);
		}
		else
			fprintf(pFile, "\n");
	}
}

void DeleteError(Error_STRUCT* pErr)
{
	delete[] pErr;
}

void DeleteErrors(Error_STRUCT** pErrs)
{
	if (pErrs == nullptr)
		return;

	int i = 0;
	while (pErrs[i] != 0)
	{
		delete[] pErrs[i];

		++i;
	}

	delete[] pErrs;
}

void DeleteErrors(Error_STRUCT** pErrs, int count)
{
	if (pErrs == nullptr)
		return;

	for (int i = 0; i < count; ++i)
	{
		delete[] pErrs[i];
	}

	delete[] pErrs;
}

int GetCountErrors(Error_STRUCT** pErrs)
{
	if (pErrs == nullptr)
		return -1;

	int i = 0;
	Error_STRUCT* pRow = pErrs[i];
	while (pRow != 0)
	{
		++i;
		pRow = pErrs[i];
	}
	return i;
}

//////////////////////////////////////////////////////////////////////////

void CharTable2GDLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, GDLB_STRUCT*** pppResults, void* Client)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	GDLB_STRUCT** ppResults = new GDLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_123 = GetIndexByFieldID(ppFieldInfos, FIELD_GDDM);
	int col_124 = GetIndexByFieldID(ppFieldInfos, FIELD_GDMC);
	int col_125 = GetIndexByFieldID(ppFieldInfos, FIELD_ZHLB);
	int col_121 = GetIndexByFieldID(ppFieldInfos, FIELD_ZJZH);
	int col_173 = GetIndexByFieldID(ppFieldInfos, FIELD_XWDM);
	int col_281 = GetIndexByFieldID(ppFieldInfos, FIELD_RZRQBS);
	int col_1213 = GetIndexByFieldID(ppFieldInfos, FIELD_BLXX);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new GDLB_STRUCT();

		//if (col_123 >= 0)
		strcpy_s(ppResults[i]->GDDM, ppTable[i * COL_EACH_ROW + col_123]);
		//if (col_124 >= 0)
		strcpy_s(ppResults[i]->GDMC, ppTable[i * COL_EACH_ROW + col_124]);
		if (col_121 >= 0)
			strcpy_s(ppResults[i]->ZJZH, ppTable[i * COL_EACH_ROW + col_121]);
		//if (col_125 >= 0)
		strcpy_s(ppResults[i]->ZHLB, ppTable[i * COL_EACH_ROW + col_125]);
		if (col_173 >= 0)
			strcpy_s(ppResults[i]->XWDM, ppTable[i * COL_EACH_ROW + col_173]);
		if (col_281 >= 0)
			strcpy_s(ppResults[i]->RZRQBS, ppTable[i * COL_EACH_ROW + col_281]);
		if (col_1213 >= 0)
			strcpy_s(ppResults[i]->BLXX, ppTable[i * COL_EACH_ROW + col_1213]);

		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);
		ppResults[i]->RZRQBS_ = atoi(ppResults[i]->RZRQBS);

		ppResults[i]->Client = Client;
	}
}

void DeleteStructs(void*** pppStructs)
{
	if (*pppStructs == nullptr)
		return;

	void** ppStructs = *pppStructs;

	int i = 0;
	while (ppStructs[i] != 0)
	{
		delete[] ppStructs[i];
		ppStructs[i] = nullptr;

		++i;
	}

	delete[] ppStructs;
	*pppStructs = nullptr;
}


/*
6
A74168961|WK  |05000000100801|1|       0|      |
013086113|WK  |05000000100801|0|       0|      |

7
A12121217|ZHT|1|      40690000|28356| 0|主股东|
015597938|ZHT|0|      40690000|394232|0|主股东|

5
A341083000|LK|1|                         |主股东|
0189181400|LK|0|                         |主股东|

8
E015976151|HT|1|88500918|		12982|1|	|主股东|
0601605823|HT|0|88500918|		354000|1|	|主股东|
*/
void String2GDLB(char* szString, GDLB_STRUCT*** pppResults, void* Client)
{
	*pppResults = nullptr;
	if (szString == nullptr)
		return;

	char* pBuf = new char[strlen(szString) + 1];
	strcpy(pBuf, szString);

	vector<char*> vct;

	// 分好多少列
	char* token = strtok(pBuf, "\r\n");
	int i = 0;
	while (token)
	{
		if (i>0)
		{
			vct.push_back(token);
		}
		token = strtok(nullptr, "\r\n");
		++i;
	}

	int count = vct.size();

	GDLB_STRUCT** ppResults = new GDLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count;++i)
	{
		ppResults[i] = new GDLB_STRUCT();

		vector<char*> v1;
		vector<char*> v2;
		vector<char*> v3;
		char* t = strtok(vct[i], "|");
		int j = 0;
		while (t)
		{
			int len = strlen(t);
			if (len == 1)
			{
				v2.push_back(t);
			}
			else if (len>0)
			{
				if (t[0]>127||t[0]<0)
				{
					v3.push_back(t);
				}
				else
				{
					v1.push_back(t);
				}
			}
			t = strtok(nullptr, "|");
		}

		strcpy_s(ppResults[i]->GDDM, v1[0]);
		if (v1.size() > 1)
			strcpy_s(ppResults[i]->ZJZH, v1[1]);
		if (v1.size() > 2)
			strcpy_s(ppResults[i]->XWDM, v1[2]);

		strcpy_s(ppResults[i]->ZHLB, v2[0]);
		if (v2.size() > 1)
			strcpy_s(ppResults[i]->RZRQBS, v2[1]);

		if (v3.size()>0)
			strcpy_s(ppResults[i]->GDMC, v3[0]);
		if (v3.size()>1)
			strcpy_s(ppResults[i]->BLXX, v3[1]);

		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);
		ppResults[i]->RZRQBS_ = atoi(ppResults[i]->RZRQBS);

		ppResults[i]->Client = Client;
	}

	delete[] pBuf;
}

void CharTable2Login(char** ppTable, GDLB_STRUCT*** pppResults, void* Client)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];
	while (p != nullptr)
	{
		int requstid = atoi(p);
		if (requstid == REQUEST_GDLB + 1)
		{
			String2GDLB(ppTable[i * COL_EACH_ROW + 2], pppResults, Client);
		}

		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	return;
}

int GetCountStructs(void** ppResults)
{
	if (ppResults == nullptr)
		return -1;

	int i = 0;
	void* pRow = ppResults[i];
	while (pRow != 0)
	{
		++i;
		pRow = ppResults[i];
	}
	return i;
}

void DeleteRequestRespone(RequestRespone_STRUCT* pRespone)
{
	if (pRespone == nullptr)
		return;

	DeleteTableBody(pRespone->ppResults);
	DeleteError(pRespone->pErr);
}

#else
#endif