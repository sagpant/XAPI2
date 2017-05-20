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


FieldInfo_STRUCT** CopyTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return nullptr;

	int count = GetCountTableHeader(ppHeader)+1;
	FieldInfo_STRUCT** ppNew = new FieldInfo_STRUCT*[count];
	ppNew[count - 1] = nullptr;
	
	for (int i = 0; i < count;++i)
	{
		ppNew[i] = new FieldInfo_STRUCT;
		memcpy(ppNew[i], ppHeader[i], sizeof(FieldInfo_STRUCT));
	}
	return ppNew;
}

void DeleteTableHeader(FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr)
		return;

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

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new GDLB_STRUCT();

		int j = 0;
		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
		while (pRow != 0)
		{
			char* t = ppTable[i * COL_EACH_ROW + j];
			switch (pRow->FieldID)
			{
			case FIELD_GDDM:
				strcpy_s(ppResults[i]->GDDM, t);
				break;
			case FIELD_GDMC:
				strcpy_s(ppResults[i]->GDMC, t);
				break;
			case FIELD_ZHLB:
				strcpy_s(ppResults[i]->ZHLB, t);
				break;
			case FIELD_ZJZH:
				strcpy_s(ppResults[i]->ZJZH, t);
				break;
			case FIELD_XWDM:
				strcpy_s(ppResults[i]->XWDM, t);
				break;
			case FIELD_RZRQBS:
				strcpy_s(ppResults[i]->RZRQBS, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			}

			++j;
			pRow = ppFieldInfos[j];
		}

		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);
		ppResults[i]->RZRQBS_ = atoi(ppResults[i]->RZRQBS);

		ppResults[i]->Client = Client;
	}
}

void DeleteStructs(void*** pppStructs)
{
	if (pppStructs == nullptr)
		return;

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

8
101313017766|0026087534|ZL|0|     304300|0|0|主股东|
101313017766|A511876656|ZL|1|     55005|0|0|主股东|
*/
// 发现不准，从登录信息中猜测式解析不准，需要换一种方式
void String2GDLB1(char* szString, GDLB_STRUCT*** pppResults, void* Client)
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

// 这种方式假定登录时的格式与直接查询的格式是完全一样的
void String2GDLB2(FieldInfo_STRUCT** ppFieldInfos, char* szString, GDLB_STRUCT*** pppResults, void* Client, char* ZJZH)
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

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new GDLB_STRUCT();

		int header_count = GetCountTableHeader(ppFieldInfos);

		char* pLast = vct[i];
		char* pCurr = strstr(vct[i], "|");

		for (int j = 0; j < header_count; ++j)
		{
			if (pCurr == nullptr)
				break;

			*pCurr = 0;
			char* t = pLast;

			FieldInfo_STRUCT* pRow = ppFieldInfos[j];
			switch (pRow->FieldID)
			{
			case FIELD_GDDM:
				strcpy_s(ppResults[i]->GDDM, t);
				break;
			case FIELD_GDMC:
				strcpy_s(ppResults[i]->GDMC, t);
				break;
			case FIELD_ZHLB:
				strcpy_s(ppResults[i]->ZHLB, t);
				break;
			case FIELD_ZJZH:
				strcpy_s(ppResults[i]->ZJZH, t);
				// 银河证券会出现资金账号为空的情况，只能从其它地方获取
				if (strlen(t) <= 0)
				{
					strcpy_s(ppResults[i]->ZJZH, ZJZH);
				}
				break;
			case FIELD_XWDM:
				strcpy_s(ppResults[i]->XWDM, t);
				break;
			case FIELD_RZRQBS:
				strcpy_s(ppResults[i]->RZRQBS, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			case FIELD_JB:
				strcpy_s(ppResults[i]->JB, t);
			default:
				break;
			}

			pLast = pCurr + 1;
			pCurr = strstr(pLast, "|");
		}

		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);
		ppResults[i]->RZRQBS_ = atoi(ppResults[i]->RZRQBS);

		ppResults[i]->Client = Client;
	}

	delete[] pBuf;
}

void String_148(FieldInfo_STRUCT** ppFieldInfos, char* szString, REQ148_STRUCT*** pppResults, void* Client)
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

	REQ148_STRUCT** ppResults = new REQ148_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new REQ148_STRUCT();

		// 不用strtok是因为第1位，和空白位会丢失，导致数据处理错误
		int header_count = GetCountTableHeader(ppFieldInfos);

		char* pLast = vct[i];
		char* pCurr = strstr(vct[i], "|");

		for (int j = 0; j < header_count; ++j)
		{
			// 国金，取出来的一条记录没有4个，只有3个
			if (pCurr == nullptr)
				break;

			*pCurr = 0;
			char* t = pLast;

			FieldInfo_STRUCT* pRow = ppFieldInfos[j];
			switch (pRow->FieldID)
			{
			case FIELD_KHH:
				strcpy_s(ppResults[i]->CPZH, t);
				break;
			case FIELD_ZHLB:
				strcpy_s(ppResults[i]->ZHLB, t);
				break;
			case FIELD_FJYZHBZ:
				strcpy_s(ppResults[i]->FJYZHBZ, t);
				break;
			case FIELD_RZRQBS:
				strcpy_s(ppResults[i]->RZRQBS, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			default:
				break;
			}

			pLast = pCurr + 1;
			pCurr = strstr(pLast, "|");
		}

		ppResults[i]->Client = Client;
	}

	delete[] pBuf;
}

void CharTable2GDLB3(char* ppTable, GDLB_STRUCT*** pppResults, void* Client)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

}

void CharTable2Login(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, GDLB_STRUCT*** pppResults, void* Client, FieldInfo_STRUCT** ppFieldInfos_148,char* CPZH)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	bool bFind = false;
	// 如果有数据，第一列就不为空
	int i = 0;
	int j = 0;
	char* p = ppTable[i * COL_EACH_ROW + j];

	// 产品账号，即资金账号
	//char CPZH[32] = { 0 };
	while (p != nullptr)
	{
		int requstid = atoi(p);
		// 银河证券没有资金账号，只有句柄，得想法转成资金账号
		if ((requstid == REQUEST_148 + 1) && (ppFieldInfos_148 != nullptr))
		{	
			// 这里假定句柄关系可以提前取到
			REQ148_STRUCT** ppRS = nullptr;
			String_148(ppFieldInfos_148, ppTable[i * COL_EACH_ROW + 2], &ppRS, Client);
			// 复制，然后清理
			int count = GetCountStructs((void**)ppRS);
			if (count > 0)
			{
				strcpy(CPZH, ppRS[0]->CPZH);
			}

			DeleteStructs((void***)&ppRS);
		}

		if (requstid == REQUEST_GDLB + 1)
		{
			String2GDLB2(ppFieldInfos, ppTable[i * COL_EACH_ROW + 2], pppResults, Client, CPZH);
			bFind = true;
		}

		++i;
		p = ppTable[i * COL_EACH_ROW + j];
	}

	// 香港账号可能没有开头的一些数字，只能自己识别
	// 光证国际，登录时的资金帐号，查询股东列表，两个数据完全不一样，再查一次会覆盖
	if (!bFind)
	{
		int count = 1;
		GDLB_STRUCT** ppResults = new GDLB_STRUCT*[count + 1]();
		ppResults[count] = nullptr;
		*pppResults = ppResults;


		int i = 0;
		int j = 0;
		char* p = ppTable[i * COL_EACH_ROW + j];
		int pos = 0;

		while (p != nullptr)
		{
			char * flag = ppTable[i * COL_EACH_ROW + 10];
			if (flag)
			{
				if (strcmp(flag, "资金帐号") == 0)
				{
					ppResults[pos] = new GDLB_STRUCT();

					strcpy_s(ppResults[pos]->ZJZH, ppTable[i * COL_EACH_ROW + 0]);
					strcpy_s(ppResults[pos]->GDMC, ppTable[i * COL_EACH_ROW + 1]);
					++pos;
				}
			}

			++i;
			p = ppTable[i * COL_EACH_ROW + j];
		}
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

void DeleteRequestRespone(RequestResponse_STRUCT* pRespone)
{
	if (pRespone == nullptr)
		return;

	DeleteTableBody(pRespone->ppResults);
	DeleteError(pRespone->pErr);
	// 会不会是由它导致的每次内存没有释放干净？
	//delete[] pRespone;
}

void CharTable2XGSGEDCX(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, XGSGEDCX_STRUCT*** pppResults, void* Client)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	XGSGEDCX_STRUCT** ppResults = new XGSGEDCX_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new XGSGEDCX_STRUCT();

		int j = 0;
		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
		while (pRow != 0)
		{
			char* t = ppTable[i * COL_EACH_ROW + j];
			switch (pRow->FieldID)
			{
			case FIELD_ZJZH:
				strcpy_s(ppResults[i]->ZJZH, t);
				break;
			case FIELD_CS:
				strcpy_s(ppResults[i]->CS, t);
				break;
			case FIELD_ZHLB:
				strcpy_s(ppResults[i]->ZHLB, t);
				break;
			case FIELD_GDDM:
				strcpy_s(ppResults[i]->GDDM, t);
				break;
			case FIELD_JYSMC:
				strcpy_s(ppResults[i]->JYSMC, t);
				break;
			case FIELD_XGSGED:
				strcpy_s(ppResults[i]->XGSGED, t);
				break;
			case FIELD_KSGED:
				strcpy_s(ppResults[i]->KSGED, t);
				break;
			case FIELD_CZSJ:
				strcpy_s(ppResults[i]->CZSJ, t);
				break;
			case FIELD_JB:
				strcpy_s(ppResults[i]->JB, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			case FIELD_SGED_961:
				strcpy_s(ppResults[i]->SGED_961, t);
				break;
			case FIELD_XGSGED_5564:
				strcpy_s(ppResults[i]->XGSGED_5564, t);
				break;
			case FIELD_ZHED:
				strcpy_s(ppResults[i]->ZHED, t);
				break;
			}
			++j;
			pRow = ppFieldInfos[j];
		}

		
		ppResults[i]->CS_ = atof(ppResults[i]->CS);
		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);
		ppResults[i]->XGSGED_ = atof(ppResults[i]->XGSGED);
		ppResults[i]->SGED_961_ = atof(ppResults[i]->SGED_961);
		ppResults[i]->KSGED_ = atof(ppResults[i]->KSGED);
		ppResults[i]->XGSGED_5564_ = atof(ppResults[i]->XGSGED_5564);
		ppResults[i]->ZHED_ = atof(ppResults[i]->ZHED);

		ppResults[i]->XGSGED_ = max(ppResults[i]->XGSGED_, ppResults[i]->SGED_961_);
		ppResults[i]->XGSGED_ = max(ppResults[i]->XGSGED_, ppResults[i]->KSGED_);
		ppResults[i]->XGSGED_ = max(ppResults[i]->XGSGED_, ppResults[i]->XGSGED_5564_);
		ppResults[i]->XGSGED_ = max(ppResults[i]->XGSGED_, ppResults[i]->ZHED_);


		ppResults[i]->Client = Client;
	}
}


void CharTable2KSGXGCX(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, KSGXGCX_STRUCT*** pppResults, void* Client)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	KSGXGCX_STRUCT** ppResults = new KSGXGCX_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new KSGXGCX_STRUCT();

		int j = 0;
		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
		while (pRow != 0)
		{
			char* t = ppTable[i * COL_EACH_ROW + j];
			switch (pRow->FieldID)
			{
			case FIELD_ZQDM:
				strcpy_s(ppResults[i]->ZQDM, t);
				break;
			case FIELD_ZQMC:
				strcpy_s(ppResults[i]->ZQMC, t);
				break;
			case FIELD_FXJG:
				strcpy_s(ppResults[i]->FXJG, t);
				break;
			case FIELD_ZDSL:
				strcpy_s(ppResults[i]->ZDSL, t);
				break;
			case FIELD_ZGSL:
				strcpy_s(ppResults[i]->ZGSL, t);
				break;
			case FIELD_WTSL:
				strcpy_s(ppResults[i]->WTSL, t);
				break;
			case FIELD_WTRQ:
				strcpy_s(ppResults[i]->WTRQ, t);
				break;
			case FIELD_JYSDM:
				strcpy_s(ppResults[i]->JYSDM, t);
				break;
			case FIELD_JYSMC:
				strcpy_s(ppResults[i]->JYSMC, t);
				break;
			case FIELD_CZSJ:
				strcpy_s(ppResults[i]->CZSJ, t);
				break;
			case FIELD_JB:
				strcpy_s(ppResults[i]->JB, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			case FIELD_SGSX:
				strcpy_s(ppResults[i]->SGSX, t);
				break;
			case FIELD_WTJG:
				strcpy_s(ppResults[i]->WTJG, t);
				break;
			case FIELD_ZHLB:
				strcpy_s(ppResults[i]->ZHLB, t);
				break;
			}
			++j;
			pRow = ppFieldInfos[j];
		}

		ppResults[i]->FXJG_ = atof(ppResults[i]->FXJG);
		ppResults[i]->ZDSL_ = atof(ppResults[i]->ZDSL);
		ppResults[i]->ZGSL_ = atof(ppResults[i]->ZGSL);
		ppResults[i]->WTSL_ = atof(ppResults[i]->WTSL);
		ppResults[i]->WTRQ_ = atoi(ppResults[i]->WTRQ);
		ppResults[i]->JYSDM_ = atoi(ppResults[i]->JYSDM);
		ppResults[i]->SGSX_ = atof(ppResults[i]->SGSX);
		ppResults[i]->WTJG_ = atof(ppResults[i]->WTJG);
		ppResults[i]->ZHLB_ = atof(ppResults[i]->ZHLB);

		ppResults[i]->ZGSL_ = max(ppResults[i]->ZGSL_, ppResults[i]->SGSX_);
		ppResults[i]->FXJG_ = max(ppResults[i]->FXJG_, ppResults[i]->WTJG_);
		ppResults[i]->JYSDM_ = max(ppResults[i]->JYSDM_, ppResults[i]->ZHLB_);

		ppResults[i]->Client = Client;
	}
}

void CharTable2HQ(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, HQ_STRUCT*** pppResults, void* Client)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	HQ_STRUCT** ppResults = new HQ_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new HQ_STRUCT();

		int j = 0;
		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
		while (pRow != 0)
		{
			char* t = ppTable[i * COL_EACH_ROW + j];
			switch (pRow->FieldID)
			{
			case FIELD_ZQDM:
				strcpy_s(ppResults[i]->ZQDM, t);
				break;
			case FIELD_ZQMC:
				strcpy_s(ppResults[i]->ZQMC, t);
				break;
			case FIELD_ZSJ:
				strcpy_s(ppResults[i]->ZSJ, t);
				break;
			case FIELD_JKJ:
				strcpy_s(ppResults[i]->JKJ, t);
				break;
			case FIELD_GZLX:
				strcpy_s(ppResults[i]->GZLX, t);
				break;
			case FIELD_DQJ:
				strcpy_s(ppResults[i]->DQJ, t);
				break;
			case FIELD_BID_PRICE_1:
				strcpy_s(ppResults[i]->BidPrice1, t);
				break;
			case FIELD_BID_PRICE_2:
				strcpy_s(ppResults[i]->BidPrice2, t);
				break;
			case FIELD_BID_PRICE_3:
				strcpy_s(ppResults[i]->BidPrice3, t);
				break;
			case FIELD_BID_PRICE_4:
				strcpy_s(ppResults[i]->BidPrice4, t);
				break;
			case FIELD_BID_PRICE_5:
				strcpy_s(ppResults[i]->BidPrice5, t);
				break;
			case FIELD_BID_SIZE_1:
				strcpy_s(ppResults[i]->BidSize1, t);
				break;
			case FIELD_BID_SIZE_2:
				strcpy_s(ppResults[i]->BidSize2, t);
				break;
			case FIELD_BID_SIZE_3:
				strcpy_s(ppResults[i]->BidSize3, t);
				break;
			case FIELD_BID_SIZE_4:
				strcpy_s(ppResults[i]->BidSize4, t);
				break;
			case FIELD_BID_SIZE_5:
				strcpy_s(ppResults[i]->BidSize5, t);
				break;
			case FIELD_ASK_PRICE_1:
				strcpy_s(ppResults[i]->AskPrice1, t);
				break;
			case FIELD_ASK_PRICE_2:
				strcpy_s(ppResults[i]->AskPrice2, t);
				break;
			case FIELD_ASK_PRICE_3:
				strcpy_s(ppResults[i]->AskPrice3, t);
				break;
			case FIELD_ASK_PRICE_4:
				strcpy_s(ppResults[i]->AskPrice4, t);
				break;
			case FIELD_ASK_PRICE_5:
				strcpy_s(ppResults[i]->AskPrice5, t);
				break;
			case FIELD_ASK_SIZE_1:
				strcpy_s(ppResults[i]->AskSize1, t);
				break;
			case FIELD_ASK_SIZE_2:
				strcpy_s(ppResults[i]->AskSize2, t);
				break;
			case FIELD_ASK_SIZE_3:
				strcpy_s(ppResults[i]->AskSize3, t);
				break;
			case FIELD_ASK_SIZE_4:
				strcpy_s(ppResults[i]->AskSize4, t);
				break;
			case FIELD_ASK_SIZE_5:
				strcpy_s(ppResults[i]->AskSize5, t);
				break;
			case FIELD_JYSDM:
				strcpy_s(ppResults[i]->JYSDM, t);
				break;
			case FIELD_ZXJYGS:
				strcpy_s(ppResults[i]->ZXJYGS, t);
				break;
			case FIELD_ZXMRBDJW:
				strcpy_s(ppResults[i]->ZXMRBDJW, t);
				break;
			case FIELD_ZXMCBDJW:
				strcpy_s(ppResults[i]->ZXMCBDJW, t);
				break;
			case FIELD_ZHLB:
				strcpy_s(ppResults[i]->ZHLB, t);
				break;
			case FIELD_BZ:
				strcpy_s(ppResults[i]->BZ, t);
				break;
			case FIELD_GZBS:
				strcpy_s(ppResults[i]->GZBS, t);
				break;
			case FIELD_ZTJG:
				strcpy_s(ppResults[i]->ZTJG, t);
				break;
			case FIELD_DTJG:
				strcpy_s(ppResults[i]->DTJG, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			}
			++j;
			pRow = ppFieldInfos[j];
		}

		ppResults[i]->ZSJ_ = atof(ppResults[i]->ZSJ);
		ppResults[i]->JKJ_ = atof(ppResults[i]->JKJ);
		ppResults[i]->GZLX_ = atof(ppResults[i]->GZLX);
		ppResults[i]->DQJ_ = atof(ppResults[i]->DQJ);
		ppResults[i]->BidPrice1_ = atof(ppResults[i]->BidPrice1);
		ppResults[i]->BidPrice2_ = atof(ppResults[i]->BidPrice2);
		ppResults[i]->BidPrice3_ = atof(ppResults[i]->BidPrice3);
		ppResults[i]->BidPrice4_ = atof(ppResults[i]->BidPrice4);
		ppResults[i]->BidPrice5_ = atof(ppResults[i]->BidPrice5);
		ppResults[i]->AskPrice1_ = atof(ppResults[i]->AskPrice1);
		ppResults[i]->AskPrice2_ = atof(ppResults[i]->AskPrice2);
		ppResults[i]->AskPrice3_ = atof(ppResults[i]->AskPrice3);
		ppResults[i]->AskPrice4_ = atof(ppResults[i]->AskPrice4);
		ppResults[i]->AskPrice5_ = atof(ppResults[i]->AskPrice5);
		ppResults[i]->BidSize1_ = atoi(ppResults[i]->BidSize1);
		ppResults[i]->BidSize2_ = atoi(ppResults[i]->BidSize2);
		ppResults[i]->BidSize3_ = atoi(ppResults[i]->BidSize3);
		ppResults[i]->BidSize4_ = atoi(ppResults[i]->BidSize4);
		ppResults[i]->BidSize5_ = atoi(ppResults[i]->BidSize5);
		ppResults[i]->AskSize1_ = atoi(ppResults[i]->AskSize1);
		ppResults[i]->AskSize2_ = atoi(ppResults[i]->AskSize2);
		ppResults[i]->AskSize3_ = atoi(ppResults[i]->AskSize3);
		ppResults[i]->AskSize4_ = atoi(ppResults[i]->AskSize4);
		ppResults[i]->AskSize5_ = atoi(ppResults[i]->AskSize5);

		ppResults[i]->ZXJYGS_ = atoi(ppResults[i]->ZXJYGS);
		ppResults[i]->ZXMRBDJW_ = atof(ppResults[i]->ZXMRBDJW);
		ppResults[i]->ZXMCBDJW_ = atof(ppResults[i]->ZXMCBDJW);
		ppResults[i]->ZTJG_ = atof(ppResults[i]->ZTJG);
		ppResults[i]->DTJG_ = atof(ppResults[i]->DTJG);

		ppResults[i]->Client = Client;
	}
}

//void CharTable2XGPHCX(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, XGPHCX_STRUCT*** pppResults, void* Client)
//{
//	*pppResults = nullptr;
//	if (ppTable == nullptr)
//		return;
//
//	int count = GetRowCountTableBody(ppTable);
//	if (count <= 0)
//		return;
//
//	XGPHCX_STRUCT** ppResults = new XGPHCX_STRUCT*[count + 1]();
//	ppResults[count] = nullptr;
//	*pppResults = ppResults;
//
//	for (int i = 0; i < count; ++i)
//	{
//		ppResults[i] = new XGPHCX_STRUCT();
//
//		int j = 0;
//		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
//		while (pRow != 0)
//		{
//			char* t = ppTable[i * COL_EACH_ROW + j];
//			switch (pRow->FieldID)
//			{
//			case FIELD_PHRQ:
//				strcpy_s(ppResults[i]->PHRQ, t);
//				break;
//			case FIELD_ZQDM:
//				strcpy_s(ppResults[i]->ZQDM, t);
//				break;
//			case FIELD_ZQMC:
//				strcpy_s(ppResults[i]->ZQMC, t);
//				break;
//			case FIELD_QSPH:
//				strcpy_s(ppResults[i]->QSPH, t);
//				break;
//			case FIELD_PHSL:
//				strcpy_s(ppResults[i]->PHSL, t);
//				break;
//			case FIELD_ZHLB:
//				strcpy_s(ppResults[i]->ZHLB, t);
//				break;
//			case FIELD_BLXX:
//				strcpy_s(ppResults[i]->BLXX, t);
//				break;
//			}
//			++j;
//			pRow = ppFieldInfos[j];
//		}
//
//		ppResults[i]->PHRQ_ = atoi(ppResults[i]->PHRQ);
//		ppResults[i]->QSPH_ = atoi(ppResults[i]->QSPH);
//		ppResults[i]->PHSL_ = atoi(ppResults[i]->PHSL);
//		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);
//
//		ppResults[i]->Client = Client;
//	}
//}

#else


void OutputCSVTableHeader(FILE* pFile, FieldInfo_STRUCT** ppHeader)
{
	if (ppHeader == nullptr || pFile == nullptr)
		return;

	int i = 0;
	FieldInfo_STRUCT* pRow = ppHeader[i];
	while (pRow != 0)
	{
		char buf[512] = { 0 };
		fprintf(pFile, "%d_%s,",//"%d,%s,%d,%d,%d,%d,%d",
			pRow->FieldID, pRow->FieldName, pRow->a, pRow->b, pRow->Len, pRow->d, pRow->e);

		++i;
		pRow = ppHeader[i];
	}
	fprintf(pFile, "\n");
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




#endif