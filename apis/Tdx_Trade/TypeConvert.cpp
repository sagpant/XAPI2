#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TypeConvert.h"
#include "../../include/Tdx/tdx_enum.h"
#include "../../include/Tdx/tdx_field.h"
#include "../../include/Tdx/tdx_function.h"

#include "../../include/toolkit.h"

#include "../../common/Queue/MsgQueue.h"

//////////////////////////////////////////////////////////////////////////
int ZTSM_str_2_int(char* pIn)
{
	char* pCheng = strstr(pIn, "成");
	char* pChe = strstr(pIn, "撤");
	char* pBu = strstr(pIn, "部");

	if (pChe)
	{
		if (pBu)
		{
			return ZTSM_PartiallyCancelled;
		}
		return ZTSM_AllCancelled;
	}
	else if (pCheng)
	{
		if (pBu)
		{
			return ZTSM_PartiallyFilled;
		}
		return ZTSM_AllFilled;
	}

	char* pF1 = strstr(pIn, "废");
	char* pF2 = strstr(pIn, "非");

	if (pF1 || pF2)
	{
		return ZTSM_Illegal;
	}

	char* pDeng = strstr(pIn, "等");
	if (pDeng)
	{
		return ZTSM_WaitingForReport;
	}

	//char* pQ = strstr(pIn, "全");
	//char* pY = strstr(pIn, "已");
	//char* pBao = strstr(pIn, "报");

	return ZTSM_New;
}

// 报价方式转，这是根据字符串的特点进行分类
// TODO:报价方式还有申购，等待后期处理
int BJFS_str_2_int(char* pIn)
{
	char* pX1 = strstr(pIn, "限");

	if (pX1 == pIn)
	{
		// 第一个字是限价
		return WTFS_Limit;
	}
	else
	{
		char* pMM = strstr(pIn, "买卖");
		if (pMM)
		{
			// 华泰返回买卖是表示限价
			return WTFS_Limit;
		}
		char* pC = strstr(pIn, "撤");
		if (pC)
		{
			char* p5 = strstr(pIn, "五");
			if (p5)
			{
				return WTFS_Five_IOC;
			}
			char* pQ = strstr(pIn, "全");
			if (pQ)
			{
				return WTFS_FOK;
			}
			return WTFS_IOC; // 剩
		}
		else
		{
			char* pZ = strstr(pIn, "转");
			if (pZ)
			{
				return WTFS_Five_Limit;
			}
			char* pD = strstr(pIn, "对");
			if (pD)
			{
				return WTFS_Best_Reverse;
			}
			char* pB = strstr(pIn, "本");
			if (pB)
			{
				return WTFS_Best_Forward;
			}
		}

		return WTFS_Limit;
	}
}


///发现不管你下的什么单，只要是特殊的单，买卖标志都是3，所以需要另行处理
int WTLB_str_2_int(char* pIn)
{
	char* pChe = strstr(pIn, "撤");
	char* pBuy = strstr(pIn, "买");
	if (pBuy)
	{
		if (pChe)
		{
			return WTLB_Buy_Cancel;
		}
		return WTLB_Buy;
	}
	char* pSell = strstr(pIn, "卖");
	if (pSell)
	{
		if (pChe)
		{
			return WTLB_Sell_Cancel;
		}
		return WTLB_Sell;
	}


	char* pETF = strstr(pIn, "ETF");
	char* pShen = strstr(pIn, "申");
	if (pShen)
	{
		if (pETF)
		{
			return WTLB_ETFCreation;
		}
		return WTLB_LOFCreation;
	}
	char* pShu = strstr(pIn, "赎");
	if (pShu)
	{
		if (pETF)
		{
			return WTLB_ETFRedemption;
		}
		return WTLB_LOFRedemption;
	}
	
	char* pHe = strstr(pIn, "合");
	if (pHe)
	{
		return WTLB_Merge;
	}
	char* pChai = strstr(pIn, "拆");
	if (pChai)
	{
		return WTLB_Split;
	}

	return WTLB_Buy;
}

//////////////////////////////////////////////////////////////////////////

void CharTable2WTLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, WTLB_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	WTLB_STRUCT** ppResults = new WTLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	int col_147 = GetIndexByFieldID(ppFieldInfos, FIELD_ZTSM);

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new WTLB_STRUCT();

		int j = 0;
		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
		while (pRow != 0)
		{
			char* t = ppTable[i * COL_EACH_ROW + j];
			switch (pRow->FieldID)
			{
			case FIELD_WTRQ:
				strcpy_s(ppResults[i]->WTRQ, t);
				break;
			case FIELD_WTSJ:
				strcpy_s(ppResults[i]->WTSJ, t);
				break;
			case FIELD_GDDM:
				strcpy_s(ppResults[i]->GDDM, t);
				break;
			case FIELD_ZQDM:
				strcpy_s(ppResults[i]->ZQDM, t);
				break;
			case FIELD_ZQMC:
				strcpy_s(ppResults[i]->ZQMC, t);
				break;
			case FIELD_MMBZ:
				strcpy_s(ppResults[i]->MMBZ, t);
				break;
			case FIELD_WTLB:
				strcpy_s(ppResults[i]->WTLB, t);
				break;
			case FIELD_JYSDM:
				strcpy_s(ppResults[i]->JYSDM, t);
				break;
			case FIELD_WTJG:
				strcpy_s(ppResults[i]->WTJG, t);
				break;
			case FIELD_WTSL:
				strcpy_s(ppResults[i]->WTSL, t);
				break;
			case FIELD_CJJG:
				strcpy_s(ppResults[i]->CJJG, t);
				break;
			case FIELD_CJSL:
				strcpy_s(ppResults[i]->CJSL, t);
				break;
			case FIELD_CDSL:
				strcpy_s(ppResults[i]->CDSL, t);
				break;
			case FIELD_WTBH:
				strcpy_s(ppResults[i]->WTBH, t);
				break;
			case FIELD_BJFS:
				strcpy_s(ppResults[i]->BJFS, t);
				break;
			case FIELD_ZTSM:
				strcpy_s(ppResults[i]->ZTSM, t);
				break;
			case FIELD_DJZJ:
				strcpy_s(ppResults[i]->DJZJ, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			case FIELD_WTFS:
				strcpy_s(ppResults[i]->WTFS, t);
				break;
			case FIELD_ZJZH:
				strcpy_s(ppResults[i]->ZJZH, t);
				break;
			case FIELD_ZHLB:
				strcpy_s(ppResults[i]->ZHLB, t);
				break;
			}
			++j;
			pRow = ppFieldInfos[j];
		}

		ppResults[i]->WTRQ_ = atoi(ppResults[i]->WTRQ);
		ppResults[i]->MMBZ_ = atoi(ppResults[i]->MMBZ);
		ppResults[i]->JYSDM_ = atoi(ppResults[i]->JYSDM);
		ppResults[i]->WTJG_ = atof(ppResults[i]->WTJG);
		ppResults[i]->WTSL_ = atoi(ppResults[i]->WTSL);
		ppResults[i]->CJJG_ = atof(ppResults[i]->CJJG);
		ppResults[i]->CJSL_ = atoi(ppResults[i]->CJSL);
		ppResults[i]->DJZJ_ = atof(ppResults[i]->DJZJ);
		ppResults[i]->WTFS_ = atof(ppResults[i]->WTFS);
		ppResults[i]->ZHLB_ = atoi(ppResults[i]->ZHLB);

		// 可能没有，怎么办?那就只好不用它了
		ppResults[i]->CDSL_ = atoi(ppResults[i]->CDSL);

		if (strstr(ppResults[i]->WTSJ, ":"))
		{
			int HH = 0, mm = 0, ss = 0;
			GetUpdateTime_HH_mm_ss(ppResults[i]->WTSJ, &HH, &mm, &ss);
			ppResults[i]->WTSJ_ = HH * 10000 + mm * 100 + ss;
		}
		else
		{
			ppResults[i]->WTSJ_ = atoi(ppResults[i]->WTSJ);
		}


		if (col_147>=0)
		{
			// 第一个的字符，并转成数字，其实也可以全走文本比较的方式，但认为这样更快
			if (ppResults[i]->ZTSM[1] == '-')
			{
				ppResults[i]->ZTSM_ = ppResults[i]->ZTSM[0] - '0';
			}
			else
			{
				ppResults[i]->ZTSM_ = ZTSM_str_2_int(ppResults[i]->ZTSM);
			}
		}
		else
		{
			// 信达证券没有状态说明，需要模拟计算出来，这种情况下，撤单数量怎么都要有
			if (ppResults[i]->CDSL_>0)
			{
				ppResults[i]->ZTSM_ = ZTSM_AllCancelled;
			}
			else if (ppResults[i]->CJSL_ == ppResults[i]->WTSL_)
			{
				ppResults[i]->ZTSM_ = ZTSM_AllFilled;
			}
			else if (ppResults[i]->CJSL_ == 0)
			{
				ppResults[i]->ZTSM_ = ZTSM_New;
			}
			else
			{
				ppResults[i]->ZTSM_ = ZTSM_PartiallyFilled;
			}
		}
		

		ppResults[i]->BJFS_ = BJFS_str_2_int(ppResults[i]->BJFS);
		ppResults[i]->WTLB_ = WTLB_str_2_int(ppResults[i]->WTLB);

		ppResults[i]->Client = Client;
	}
}

void CharTable2CJLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, CJLB_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	CJLB_STRUCT** ppResults = new CJLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new CJLB_STRUCT();

		int j = 0;
		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
		while (pRow != 0)
		{
			char* t = ppTable[i * COL_EACH_ROW + j];
			switch (pRow->FieldID)
			{
			case FIELD_CJRQ:
				strcpy_s(ppResults[i]->CJRQ, t);
				break;
			case FIELD_CJSJ:
				strcpy_s(ppResults[i]->CJSJ, t);
				break;
			case FIELD_GDDM:
				strcpy_s(ppResults[i]->GDDM, t);
				break;
			case FIELD_ZQDM:
				strcpy_s(ppResults[i]->ZQDM, t);
				break;
			case FIELD_ZQMC:
				strcpy_s(ppResults[i]->ZQMC, t);
				break;
			case FIELD_MMBZ:
				strcpy_s(ppResults[i]->MMBZ, t);
				break;
			case FIELD_WTLB:
				strcpy_s(ppResults[i]->WTLB, t);
				break;
			case FIELD_CJJG:
				strcpy_s(ppResults[i]->CJJG, t);
				break;
			case FIELD_CJSL:
				strcpy_s(ppResults[i]->CJSL, t);
				break;
			case FIELD_FSJE:
				strcpy_s(ppResults[i]->FSJE, t);
				break;
			case FIELD_SYJE:
				strcpy_s(ppResults[i]->SYJE, t);
				break;
			case FIELD_YJ:
				strcpy_s(ppResults[i]->YJ, t);
				break;
			case FIELD_YHS:
				strcpy_s(ppResults[i]->YHS, t);
				break;
			case FIELD_GHF:
				strcpy_s(ppResults[i]->GHF, t);
				break;
			case FIELD_CJF:
				strcpy_s(ppResults[i]->CJF, t);
				break;
			case FIELD_CJBH:
				strcpy_s(ppResults[i]->CJBH, t);
				break;
			case FIELD_CDBZ:
				strcpy_s(ppResults[i]->CDBZ, t);
				break;
			case FIELD_WTBH:
				strcpy_s(ppResults[i]->WTBH, t);
				break;
			}
			++j;
			pRow = ppFieldInfos[j];
		}

		ppResults[i]->CJRQ_ = atoi(ppResults[i]->CJRQ);
		ppResults[i]->MMBZ_ = atoi(ppResults[i]->MMBZ);
		ppResults[i]->CJJG_ = atof(ppResults[i]->CJJG);
		ppResults[i]->CJSL_ = atoi(ppResults[i]->CJSL);
		ppResults[i]->SYJE_ = atof(ppResults[i]->SYJE);
		ppResults[i]->YJ_ = atof(ppResults[i]->YJ);
		ppResults[i]->YHS_ = atof(ppResults[i]->YHS);
		ppResults[i]->GHF_ = atof(ppResults[i]->GHF);
		ppResults[i]->CJF_ = atof(ppResults[i]->CJF);
		ppResults[i]->CDBZ_ = atoi(ppResults[i]->CDBZ);
		ppResults[i]->FSJE_ = atof(ppResults[i]->FSJE);


		if (strstr(ppResults[i]->CJSJ, ":"))
		{
			int HH = 0, mm = 0, ss = 0;
			GetUpdateTime_HH_mm_ss(ppResults[i]->CJSJ, &HH, &mm, &ss);
			ppResults[i]->CJSJ_ = HH * 10000 + mm * 100 + ss;
		}
		else
		{
			ppResults[i]->CJSJ_ = atoi(ppResults[i]->CJSJ);
		}

		ppResults[i]->WTLB_ = WTLB_str_2_int(ppResults[i]->WTLB);

		ppResults[i]->Client = Client;
	}
}

void CharTable2GFLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, GFLB_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	GFLB_STRUCT** ppResults = new GFLB_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new GFLB_STRUCT();

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
			case FIELD_ZQSL:
				strcpy_s(ppResults[i]->ZQSL, t);
				break;
			case FIELD_KMSL:
				strcpy_s(ppResults[i]->KMSL, t);
				break;
			case FIELD_TBCBJ:
				strcpy_s(ppResults[i]->TBCBJ, t);
				break;
			case FIELD_DQJ:
				strcpy_s(ppResults[i]->DQJ, t);
				break;
			case FIELD_ZXSZ:
				strcpy_s(ppResults[i]->ZXSZ, t);
				break;
			case FIELD_TBFDYK:
				strcpy_s(ppResults[i]->TBFDYK, t);
				break;
			case FIELD_SXYK:
				strcpy_s(ppResults[i]->SXYK, t);
				break;
			case FIELD_CKYKBL:
				strcpy_s(ppResults[i]->CKYKBL, t);
				break;
			case FIELD_DJSL:
				strcpy_s(ppResults[i]->DJSL, t);
				break;
			case FIELD_GDDM:
				strcpy_s(ppResults[i]->GDDM, t);
				break;
			case FIELD_JYSDM:
				strcpy_s(ppResults[i]->JYSDM, t);
				break;
			case FIELD_JYSMC:
				strcpy_s(ppResults[i]->JYSMC, t);
				break;
			case FIELD_BLXX:
				strcpy_s(ppResults[i]->BLXX, t);
				break;
			}
			++j;
			pRow = ppFieldInfos[j];
		}

		ppResults[i]->ZQSL_ = atoi(ppResults[i]->ZQSL);
		ppResults[i]->KMSL_ = atoi(ppResults[i]->KMSL);
		ppResults[i]->TBCBJ_ = atof(ppResults[i]->TBCBJ);
		ppResults[i]->DQJ_ = atof(ppResults[i]->DQJ);
		ppResults[i]->ZXSZ_ = atof(ppResults[i]->ZXSZ);
		ppResults[i]->DJSL_ = atof(ppResults[i]->DJSL);

		ppResults[i]->Client = Client;
	}
}

void CharTable2ZJYE(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, ZJYE_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue)
{
	*pppResults = nullptr;
	if (ppTable == nullptr)
		return;

	int count = GetRowCountTableBody(ppTable);
	if (count <= 0)
		return;

	ZJYE_STRUCT** ppResults = new ZJYE_STRUCT*[count + 1]();
	ppResults[count] = nullptr;
	*pppResults = ppResults;

	for (int i = 0; i < count; ++i)
	{
		ppResults[i] = new ZJYE_STRUCT();

		int j = 0;
		FieldInfo_STRUCT* pRow = ppFieldInfos[j];
		while (pRow != 0)
		{
			char* t = ppTable[i * COL_EACH_ROW + j];
			switch (pRow->FieldID)
			{
			case FIELD_BZ:
				strcpy_s(ppResults[i]->BZ, t);
				break;
			case FIELD_ZJYE:
				strcpy_s(ppResults[i]->ZJYE, t);
				break;
			case FIELD_KYZJ:
				strcpy_s(ppResults[i]->KYZJ, t);
				break;
			case FIELD_ZZC_310:
				strcpy_s(ppResults[i]->ZZC, t);
				break;
			case FIELD_KQZJ:
				strcpy_s(ppResults[i]->KQZJ, t);
				break;
			case FIELD_ZJZH:
				strcpy_s(ppResults[i]->ZJZH, t);
				break;
			case FIELD_ZXSZ:
				strcpy_s(ppResults[i]->ZXSZ, t);
				break;
			}
			++j;
			pRow = ppFieldInfos[j];
		}

		ppResults[i]->ZJYE_ = atof(ppResults[i]->ZJYE);
		ppResults[i]->KYZJ_ = atof(ppResults[i]->KYZJ);
		ppResults[i]->ZZC_ = atof(ppResults[i]->ZZC);
		ppResults[i]->KQZJ_ = atof(ppResults[i]->KQZJ);
		ppResults[i]->ZXSZ_ = atof(ppResults[i]->ZXSZ);

		ppResults[i]->Client = Client;
	}
}

void CharTable2HQ(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, HQ_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue)
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

//////////////////////////////////////////////////////////////////////////

OrderType WTFS_2_OrderType(int In)
{
	switch (In)
	{
	case WTFS_Limit:
		return OrderType::OrderType_Limit;
	default:
		return OrderType::OrderType_Market;
	}
}

TimeInForce WTFS_2_TimeInForce(int In)
{
	switch (In)
	{
	case WTFS_Limit:
	case WTFS_Best_Reverse:
	case WTFS_Best_Forward:
		return TimeInForce::TimeInForce_Day;
	case WTFS_IOC:
	case WTFS_Five_IOC:
		return TimeInForce::TimeInForce_IOC;
	case WTFS_FOK:
		return TimeInForce::TimeInForce_FOK;
	case WTFS_Five_Limit:
		return TimeInForce::TimeInForce_Day;
	default:
		return TimeInForce::TimeInForce_Day;
	}
}

OrderStatus ZTSM_2_OrderStatus(int In)
{
	switch (In)
	{
	case ZTSM_NotSent:
		return OrderStatus::OrderStatus_PendingNew;
	case ZTSM_New:
		return OrderStatus::OrderStatus_New;
	case ZTSM_Illegal:
		return OrderStatus::OrderStatus_Rejected;
	case ZTSM_AllFilled:
		return OrderStatus::OrderStatus_Filled;
	case ZTSM_AllCancelled:
		return OrderStatus::OrderStatus_Cancelled;
	case ZTSM_PartiallyFilled:
			return OrderStatus::OrderStatus_PartiallyFilled;
	case ZTSM_WaitingForReport:
		return OrderStatus::OrderStatus_New;
	default:
		return OrderStatus::OrderStatus_NotSent;
	}
}

ExecType ZTSM_2_ExecType(int In)
{
	switch (In)
	{
	case ZTSM_NotSent:
		return ExecType::ExecType_New;
	case ZTSM_New:
		return ExecType::ExecType_New;
	case ZTSM_Illegal:
		return ExecType::ExecType_Rejected;
	case ZTSM_AllFilled:
	case ZTSM_PartiallyFilled:
		return ExecType::ExecType_Trade;
	case ZTSM_AllCancelled:
	case ZTSM_PartiallyCancelled:
		return ExecType::ExecType_Cancelled;
	case ZTSM_WaitingForReport:
		return ExecType::ExecType_New;
	default:
		return ExecType::ExecType_New;
	}
}

bool ZTSM_IsDone(int In)
{
	switch (In)
	{
	case ZTSM_Illegal:
	case ZTSM_AllFilled:
	case ZTSM_AllCancelled:
	case ZTSM_PartiallyCancelled:
		return true;
	}
	return false;
}

bool ZTSM_IsNotSent(int In)
{
	switch (In)
	{
	case ZTSM_NotSent:
		return true;
	}
	return false;
}


// 将买卖方式转成买卖方向
OrderSide MMBZ_2_OrderSide(int In)
{
	switch (In / 100)
	{
	case REQUEST_ETF_SGSH:
		switch (In % 100)
		{
		case JYLX_Creation:
			return OrderSide::OrderSide_ETFCreation;
		case JYLX_Redemption:
			return OrderSide::OrderSide_ETFRedemption;
		default:
			break;
		}
		break;
	case REQUEST_WT:
		switch (In % 100)
		{
		case MMBZ_Buy_Limit:
			return OrderSide::OrderSide_Buy;
		case MMBZ_Sell_Limit:
			return OrderSide::OrderSide_Sell;
		case MMBZ_Buy_Market:
			return OrderSide::OrderSide_Buy;
		case MMBZ_Sell_Market:
			return OrderSide::OrderSide_Sell;
		case MMBZ_Creation:
			return OrderSide::OrderSide_LOFCreation;
		case MMBZ_Redemption:
			return OrderSide::OrderSide_LOFRedemption;
		case MMBZ_Merge:
			return OrderSide::OrderSide_Merge;
		case MMBZ_Split:
			return OrderSide::OrderSide_Split;
		default:
			break;
		}
		break;
	case REQUEST_ZGHS:
		switch (In % 100)
		{
		case MMBZ_CB_Conv:
			return OrderSide::OrderSide_CBConvert;
		case MMBZ_CB_Red:
			return OrderSide::OrderSide_CBRedemption;
		default:
			break;
		}
		break;
	default:
		break;
	}
	
	return OrderSide::OrderSide_Unknown;
}

void CJLB_2_TradeField(CJLB_STRUCT* pIn, TradeField* pOut)
{
	//strcpy(pOut->ID, pIn->WTBH);将会被修正
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	strcpy(pOut->InstrumentName, pIn->ZQMC);
	strcpy(pOut->AccountID, pIn->GDDM);
	pOut->Price = pIn->CJJG_;
	pOut->Qty = pIn->CJSL_;
	pOut->Date = pIn->CJRQ_;
	pOut->Time = pIn->CJSJ_;
	pOut->Side = MMBZ_2_OrderSide(pIn->WTLB_);

	strcpy(pOut->TradeID, pIn->CJBH);

	pOut->Commission = pIn->YJ_ + pIn->YHS_ + pIn->GHF_ + pIn->CJF_;

	pOut->OpenClose = pOut->Side % 2 == 0 ? OpenCloseType::OpenCloseType_Open : OpenCloseType::OpenCloseType_Close;
	pOut->HedgeFlag = HedgeFlagType::HedgeFlagType_Speculation;
	
}

void WTLB_2_OrderField_0(WTLB_STRUCT* pIn, OrderField* pOut)
{
	//strcpy(pOut->ID, pIn->WTBH);//将会被修正
	strcpy(pOut->OrderID, pIn->WTBH);
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	strcpy(pOut->InstrumentName, pIn->ZQMC);
	strcpy(pOut->AccountID, pIn->GDDM);
	pOut->Price = pIn->WTJG_;
	pOut->Qty = pIn->WTSL_;
	pOut->Date = pIn->WTRQ_;
	pOut->Time = pIn->WTSJ_;
	// 这个地方怎么会这样
	pOut->Side = MMBZ_2_OrderSide(pIn->WTLB_);

	pOut->Type = WTFS_2_OrderType(pIn->BJFS_);
	pOut->TimeInForce = WTFS_2_TimeInForce(pIn->BJFS_);

	pOut->Status = ZTSM_2_OrderStatus(pIn->ZTSM_);
	pOut->ExecType = ZTSM_2_ExecType(pIn->ZTSM_);

	pOut->OpenClose = pOut->Side % 2 == 0 ? OpenCloseType::OpenCloseType_Open : OpenCloseType::OpenCloseType_Close;
	pOut->HedgeFlag = HedgeFlagType::HedgeFlagType_Speculation;

	strcpy(pOut->AccountID, pIn->GDDM);

	pOut->AvgPx = pIn->CJJG_;
	pOut->CumQty = pIn->CJSL_;
	pOut->LeavesQty = pIn->WTSL_ - pIn->CJSL_ - pIn->CDSL_;

	strcpy(pOut->Text, pIn->ZTSM);
}

int OrderType_2_WTFS(OrderType In)
{
	switch (In)
	{
	case OrderType_Market:
	case OrderType_Stop:
	case OrderType_MarketOnClose:
	case OrderType_TrailingStop:
		return WTFS_Five_IOC;// 只推荐使用五档模拟市价
	case OrderType_Limit:
	case OrderType_StopLimit:
	case OrderType_TrailingStopLimit:
		return WTFS_Limit;
	case OrderType_Pegged:
	default:
		return WTFS_Limit;
	}
}


void OrderField_2_Order_STRUCT(OrderField* pIn, Order_STRUCT* pOut)
{
	strcpy(pOut->KHH, pIn->ClientID);
	strcpy(pOut->ZJZH, pIn->ClientID);//通达信模拟上是否会出错？
	strcpy(pOut->GDDM, pIn->AccountID);

	strcpy(pOut->ZQDM, pIn->InstrumentID);
	pOut->Price = pIn->Price;
	pOut->Qty = pIn->Qty;
	pOut->WTFS = OrderType_2_WTFS(pIn->Type);
	pOut->RZRQBS = RZRQBS_NO;
	pOut->requestType = REQUEST_WT;

	// 这个地方后期要再改，因为没有处理基金等情况
	switch (pIn->Side)
	{
	case OrderSide::OrderSide_Buy:
		if (pIn->Type == OrderType::OrderType_Market)
			pOut->MMBZ = MMBZ_Buy_Market;
		else
			pOut->MMBZ = MMBZ_Buy_Limit;
		break;
	case OrderSide::OrderSide_Sell:
		if (pIn->Type == OrderType::OrderType_Market)
			pOut->MMBZ = MMBZ_Sell_Market;
		else
			pOut->MMBZ = MMBZ_Sell_Limit;
		break;
	case OrderSide::OrderSide_LOFCreation:
		pOut->MMBZ = MMBZ_Creation;
		break;
	case OrderSide::OrderSide_LOFRedemption:
		pOut->MMBZ = MMBZ_Redemption;
		break;
	case OrderSide::OrderSide_ETFCreation:
		pOut->MMBZ = JYLX_Creation;
		pOut->requestType = REQUEST_ETF_SGSH;
		break;
	case OrderSide::OrderSide_ETFRedemption:
		pOut->MMBZ = JYLX_Redemption;
		pOut->requestType = REQUEST_ETF_SGSH;
		break;
	case OrderSide::OrderSide_Merge:
		pOut->MMBZ = MMBZ_Merge;
		break;
	case OrderSide::OrderSide_Split:
		pOut->MMBZ = MMBZ_Split;
		break;
	case OrderSide::OrderSide_CBConvert:
		pOut->MMBZ = MMBZ_CB_Conv;
		pOut->requestType = REQUEST_ZGHS;
		break;
	case OrderSide::OrderSide_CBRedemption:
		pOut->MMBZ = MMBZ_CB_Red;
		pOut->requestType = REQUEST_ZGHS;
		break;
	}
}

void GDLB_2_InvestorField(GDLB_STRUCT* pIn, InvestorField* pOut)
{
	strcpy(pOut->InvestorID, pIn->GDDM);
	strcpy(pOut->InvestorName, pIn->GDMC);
}

void ZJYE_2_AccountField(ZJYE_STRUCT* pIn, AccountField* pOut)
{
	strcpy(pOut->AccountID, pIn->ZJZH);
	//strcpy(pOut->ClientID, pIn->ZJZH);
	
	pOut->Available = pIn->KYZJ_;

	// 还有很多不知道如何对应，有可能需要扩展XAPI部分
	pOut->Balance = pIn->ZZC_;
	
}

void GFLB_2_PositionField(GFLB_STRUCT* pIn, PositionField* pOut)
{
	pOut->Side = PositionSide::PositionSide_Long;
	pOut->Position = pIn->ZQSL_;
	pOut->TodayPosition = pIn->DJSL_;
	pOut->HistoryPosition = pIn->KMSL_;
	pOut->HedgeFlag = HedgeFlagType::HedgeFlagType_Speculation;
	strcpy(pOut->InstrumentName, pIn->ZQMC);
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	strcpy(pOut->Symbol, pIn->ZQDM);
	strcpy(pOut->AccountID, pIn->GDDM);
	strcpy(pOut->ExchangeID, pIn->JYSDM);

	// 还有一些信息没有
}