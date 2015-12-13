#pragma once

#include "../include/Tdx/TdxApi.h"
#include "../include/ApiStruct.h"

#include <list>
#include <map>
#include <unordered_map>

using namespace std;

class CMsgQueue;
class CTraderApi;

//////////////////////////////////////////////////////////////////////////
// 只是用来维护单账号的信息用
class CSingleUser
{
public:
	CSingleUser(CTraderApi* pApi);
	~CSingleUser();

	void CheckThenQueryOrder(time_t _now);
	void CheckThenQueryTrade(time_t _now);

	void OutputQueryTime(time_t t, double db, const char* szSource);

	void ReqQryOrder();
	void ReqQryTrade();

	int OnRespone_ReqQryOrder(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);
	int OnRespone_ReqQryTrade(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);

	int OnRespone_ReqUserLogin(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);
	int OnRespone_ReqQryInvestor(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);
	int OnRespone_ReqQryTradingAccount(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);
	int OnRespone_ReqQryInvestorPosition(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);

	int OnRespone_ReqOrderInsert(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);
	int OnRespone_ReqOrderAction(CTdxApi* pApi, RequestRespone_STRUCT* pRespone);

private:
	void CompareTradeMapAndEmit(unordered_map<string, TradeField*> &oldMap, unordered_map<string, TradeField*> &newMap);
	void CompareTradeListAndEmit(list<TradeField*> &oldList, list<TradeField*> &newList);

private:
	CTraderApi*					m_pApi;
	

	//unordered_map<string, OrderField*>				m_id_platform_order;
	//unordered_map<string, WTLB_STRUCT*>				m_id_api_order;

	list<TradeField*>			m_OldTradeList;
	list<TradeField*>			m_NewTradeList;
	unordered_map<string, TradeField*> m_NewTradeMap;
	unordered_map<string, TradeField*> m_OldTradeMap;


	list<OrderField*>			m_OldOrderList;
	list<OrderField*>			m_NewOrderList;

	int							m_OrderNotUpdateCount;
	bool						m_TradeListReverse;
	bool						m_LastIsMerge;

public:
	time_t						m_QueryTradeTime;
	time_t						m_QueryOrderTime;
	void*						m_pClient;
	void*						m_pClass;
	CMsgQueue*					m_msgQueue;
	char						m_UserID[64];
};

