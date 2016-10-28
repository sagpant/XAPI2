#pragma once

#include "../../include/ApiStruct.h"

#include "../../include/Tdx/tdx_struct.h"

// 两种情况：
// 1.完全重建
// 2.部分重建

// 隐藏大Bug，谁建谁删
class CMsgQueue;

void CharTable2WTLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, WTLB_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue);

void CharTable2CJLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, CJLB_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue);

void CharTable2GFLB(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, GFLB_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue);

void CharTable2ZJYE(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, ZJYE_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue);

void CharTable2HQ(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, HQ_STRUCT*** pppResults, void* Client, CMsgQueue* pQueue);

//////////////////////////////////////////////////////////////////////////


bool ZTSM_IsDone(int In);
bool ZTSM_IsNotSent(int In);

void GDLB_2_InvestorField(GDLB_STRUCT* pIn, InvestorField* pOut);
void ZJYE_2_AccountField(ZJYE_STRUCT* pIn, AccountField* pOut);
void GFLB_2_PositionField(GFLB_STRUCT* pIn, PositionField* pOut);

void CJLB_2_TradeField(CJLB_STRUCT* pIn, TradeField* pOut);

// 
void WTLB_2_OrderField_0(WTLB_STRUCT* pIn, OrderField* pOut);
//void WTLB_2_OrderField_1(WTLB_STRUCT* pIn, OrderField* pOut);

void OrderField_2_Order_STRUCT(OrderField* pIn, Order_STRUCT* pOut);
