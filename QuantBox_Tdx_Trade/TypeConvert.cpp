#include "stdafx.h"
#include "TypeConvert.h"
#include "../include/Tdx/tdx_enum.h"

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
	
		return ExecType::ExecType_Trade;
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
	switch (In)
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
		return OrderSide::OrderSide_Creation;
	case MMBZ_Redemption:
		return OrderSide::OrderSide_Redemption;
	case MMBZ_Merge:
		return OrderSide::OrderSide_Merge;
	case MMBZ_Split:
		return OrderSide::OrderSide_Split;
	default:
		break;
	}
	return OrderSide::OrderSide_Buy;
}

// 将买卖方式转成开平
OpenCloseType MMBZ_2_OpenCloseType(int In)
{
	switch (In)
	{
	case MMBZ_Buy_Limit:
	case MMBZ_Buy_Market:
	case MMBZ_Creation:
	case MMBZ_Merge:
		return OpenCloseType::OpenCloseType_Open;
	case MMBZ_Sell_Limit:
	case MMBZ_Sell_Market:
	case MMBZ_Redemption:
	case MMBZ_Split:
		return OpenCloseType::OpenCloseType_Close;
	default:
		break;
	}
	return OpenCloseType::OpenCloseType_Open;
}

void CJLB_2_TradeField(CJLB_STRUCT* pIn, TradeField* pOut)
{
	strcpy(pOut->ID, pIn->WTBH);
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	pOut->Price = pIn->CJJG_;
	pOut->Qty = pIn->CJSL_;
	pOut->Date = pIn->CJRQ_;
	pOut->Time = pIn->CJSJ_;
	pOut->Side = MMBZ_2_OrderSide(pIn->MMBZ_);

	strcpy(pOut->TradeID, pIn->CJBH);

	pOut->Commission = pIn->YJ_ + pIn->YHS_ + pIn->GHF_ + pIn->CJF_;

	pOut->OpenClose = pOut->Side == OrderSide::OrderSide_Buy ? OpenCloseType::OpenCloseType_Open : OpenCloseType::OpenCloseType_Close;
	pOut->HedgeFlag = HedgeFlagType::HedgeFlagType_Speculation;
	
}

void WTLB_2_OrderField_0(WTLB_STRUCT* pIn, OrderField* pOut)
{
	strcpy(pOut->ID, pIn->WTBH);
	strcpy(pOut->InstrumentID, pIn->ZQDM);
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

	pOut->OpenClose = MMBZ_2_OpenCloseType(pIn->WTLB_);
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
	strcpy(pOut->ZQDM, pIn->InstrumentID);
	pOut->Price = pIn->Price;
	pOut->Qty = pIn->Qty;
	pOut->WTFS = OrderType_2_WTFS(pIn->Type);
	pOut->RZRQBS = RZRQBS_NO;

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
	case OrderSide::OrderSide_Creation:
		pOut->MMBZ = MMBZ_Creation;
		break;
	case OrderSide::OrderSide_Redemption:
		pOut->MMBZ = MMBZ_Redemption;
		break;
	case OrderSide::OrderSide_Merge:
		pOut->MMBZ = MMBZ_Merge;
		break;
	case OrderSide::OrderSide_Split:
		pOut->MMBZ = MMBZ_Split;
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
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	strcpy(pOut->Symbol, pIn->ZQDM);

	// 还有一些信息没有
}