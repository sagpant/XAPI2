#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "XApiImpl.h"

#include "../../../include/QueueEnum.h"
#include "../../../include/XApiC.h"

void* __stdcall CXApiImpl::OnRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (pApi2 == nullptr)
		return nullptr;

	CXApiImpl* pThisClass = (CXApiImpl*)pApi2;
	return pThisClass->_OnRespone(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
}

void* CXApiImpl::_OnRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (nullptr == m_pSpi)
		return nullptr;

	ResponeType rt = (ResponeType)type;
	switch (rt)
	{
	case ResponeType_OnConnectionStatus:
		m_pSpi->OnConnectionStatus(this, (ConnectionStatus)(char)double1, (RspUserLoginField*)ptr1, size1);
		break;
	case ResponeType_OnRtnError:
		m_pSpi->OnRtnError(this, (ErrorField*)ptr1);
		break;
	case ResponeType_OnRtnDepthMarketData:
		m_pSpi->OnRtnDepthMarketDataN(this, (DepthMarketDataNField*)ptr1);
		break;
	case ResponeType_OnRspQryInstrument:
		m_pSpi->OnRspQryInstrument(this, (InstrumentField*)ptr1, size1, double1 != 0);
		break;
	case ResponeType_OnRspQryTradingAccount:
		m_pSpi->OnRspQryTradingAccount(this, (AccountField*)ptr1, size1, double1 != 0);
		break;
	case ResponeType_OnRspQryInvestorPosition:
		m_pSpi->OnRspQryInvestorPosition(this, (PositionField*)ptr1, size1, double1 != 0);
		break;
	case ResponeType_OnRspQrySettlementInfo:
		m_pSpi->OnRspQrySettlementInfo(this, (SettlementInfoField*)ptr1, size1, double1 != 0);
		break;

	case ResponeType_OnRtnOrder:
		m_pSpi->OnRtnOrder(this, (OrderField*)ptr1);
		break;
	case ResponeType_OnRtnTrade:
		m_pSpi->OnRtnTrade(this, (TradeField*)ptr1);
		break;

	case ResponeType_OnRtnQuote:
		m_pSpi->OnRtnQuote(this, (QuoteField*)ptr1);
		break;
	case ResponeType_OnRtnQuoteRequest:
		m_pSpi->OnRtnQuoteRequest(this, (QuoteRequestField*)ptr1);
		break;

	case ResponeType_OnRspQryHistoricalTicks:
		m_pSpi->OnRspQryHistoricalTicks(this, (TickField*)ptr1, size1, (HistoricalDataRequestField*)ptr2, size2, double1 != 0);
		break;
	case ResponeType_OnRspQryHistoricalBars:
		m_pSpi->OnRspQryHistoricalBars(this, (BarField*)ptr1, size1, (HistoricalDataRequestField*)ptr2, size2, double1 != 0);
		break;

	case ResponeType_OnRspQryInvestor:
		m_pSpi->OnRspQryInvestor(this, (InvestorField*)ptr1, size1, double1 != 0);
		break;
	case ResponeType_OnFilterSubscribe:
		return (void*)m_pSpi->OnFilterSubscribe(this, (ExchangeType)(char)double1, (int)size1, (int)size1, (int)size3, (char*)ptr1);
	default:
		break;
	}
	return nullptr;
}

CXApiImpl::CXApiImpl(const char* libPath) :CXApi()
{
	m_pLib = nullptr;
	m_pFun = nullptr;
	m_pApi = nullptr;
	m_pSpi = nullptr;

	strncpy(m_LibPath, libPath, sizeof(m_LibPath));
}


CXApiImpl::~CXApiImpl()
{
	Disconnect();
}

ApiType CXApiImpl::GetApiTypes()
{
	return X_GetApiTypes(m_pFun);
}

const char* CXApiImpl::GetApiVersion()
{
	return X_GetApiVersion(m_pFun);
}

const char* CXApiImpl::GetApiName()
{
	return X_GetApiName(m_pFun);
}

void CXApiImpl::RegisterSpi(CXSpi *pSpi)
{
	m_pSpi = pSpi;
}

bool CXApiImpl::Init()
{
	m_pLib = X_LoadLib(m_LibPath);
	if (m_pLib)
	{
		m_pFun = X_GetFunction(m_pLib);
		if (m_pFun)
		{
			return true;
		}
	}
	return false;
}

const char* CXApiImpl::GetLastError()
{
	return X_GetLastError();
}

void CXApiImpl::Connect(const char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count)
{
	m_pApi = X_Create(m_pFun);
	X_Register(m_pFun, m_pApi, (fnOnRespone)OnRespone, this);
	X_Connect(m_pFun, m_pApi, szPath, pServerInfo, pUserInfo, count);
}

void CXApiImpl::Disconnect()
{
	X_Disconnect(m_pFun, m_pApi);
	X_FreeLib(m_pLib);

	m_pLib = nullptr;
	m_pFun = nullptr;
	m_pApi = nullptr;
	m_pSpi = nullptr;
}

void CXApiImpl::Subscribe(const char* szInstrument, const char* szExchange)
{
	X_Subscribe(m_pFun, m_pApi, szInstrument, szExchange);
}

void CXApiImpl::Unsubscribe(const char* szInstrument, const char* szExchange)
{
	X_Unsubscribe(m_pFun, m_pApi, szInstrument, szExchange);
}

void CXApiImpl::ReqQuery(QueryType type, ReqQueryField* query)
{
	X_ReqQuery(m_pFun, m_pApi, type, query);
}

const char* CXApiImpl::SendOrder(OrderField* pOrder, int count, char* pOut)
{
	return X_SendOrder(m_pFun, m_pApi, pOrder, count, pOut);
}

const char* CXApiImpl::CancelOrder(OrderIDType* pIn, int count, char* pOut)
{
	return X_CancelOrder(m_pFun, m_pApi, pIn, count, pOut);
}

const char* CXApiImpl::SendQuote(QuoteField* pQuote, int count, char* pOut)
{
	return X_SendQuote(m_pFun, m_pApi, pQuote, count, pOut);
}

const char* CXApiImpl::CancelQuote(OrderIDType* pIn, int count, char* pOut)
{
	return X_CancelQuote(m_pFun, m_pApi, pIn, count, pOut);
}
