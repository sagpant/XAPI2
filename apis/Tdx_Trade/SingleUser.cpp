#include "stdafx.h"
#include "SingleUser.h"

#include "../../common/Queue/MsgQueue.h"
#include "TypeConvert.h"
#include "TraderApi.h"

#define QUERY_TIME_MIN	(3)
#define QUERY_TIME_MAX	(60)

int Today(int day)
{
	time_t now = time(0);
	now += day * 86400;
	struct tm *ptmNow = localtime(&now);

	return (ptmNow->tm_year + 1900) * 10000
		+ (ptmNow->tm_mon + 1) * 100
		+ ptmNow->tm_mday;
}

void CreateID(char* pOut, char* pDate, char*pZh, char* wtbh)
{
	if (pDate == nullptr || strlen(pDate) == 0)
	{
		sprintf(pOut, "%d:%s:%s", Today(0), pZh, wtbh);
	}
	else
	{
		sprintf(pOut, "%s:%s:%s", pDate, pZh, wtbh);
	}
}

// 解决创建与删除不在同一dll的问题
void DeleteStructs(void*** pppStructs, CMsgQueue* pQueue)
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
		//pQueue->delete_block(ppStructs[i]);
		ppStructs[i] = nullptr;

		++i;
	}

	delete[] ppStructs;
	//pQueue->delete_block(ppStructs);
	*pppStructs = nullptr;
}

CSingleUser::CSingleUser(CTraderApi* pApi)
{
	m_pApi = pApi;
	m_pClient = nullptr;
	memset(m_UserID, 0, sizeof(m_UserID));

	m_ConnectionStatus = ConnectionStatus::ConnectionStatus_Uninitialized;
}


CSingleUser::~CSingleUser()
{
}

void CSingleUser::OutputQueryTime(time_t t, double db, const char* szSource)
{
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	sprintf(pField->Message, "UserID:%s,Source:%s,Add:%d,Time:%s", m_UserID, szSource, (int)db, ctime(&t));

	// 去了最后的回车
	int len = strlen(pField->Message);
	pField->Message[len - 1] = 0;

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
}

void CSingleUser::CheckThenQueryOrder(time_t _now)
{
	if (m_ConnectionStatus != ConnectionStatus::ConnectionStatus_Done)
		return;

	if (_now > m_QueryOrderTime)
	{
		double _queryTime = QUERY_TIME_MAX;
		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "QueryOrder");

		ReqQryOrder();
	}
}

void CSingleUser::CheckThenQueryTrade(time_t _now)
{
	if (m_ConnectionStatus != ConnectionStatus::ConnectionStatus_Done)
		return;

	if (_now > m_QueryTradeTime)
	{
		double _queryTime = QUERY_TIME_MAX;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "QueryTrade");

		ReqQryTrade();
	}
}

void CSingleUser::ReqQryOrder()
{
	// 进行查询
	ReqQueryData_STRUCT query = { 0 };
	query.Client = m_pClient;
	strcpy(query.KHH, m_UserID);
	query.requestType = REQUEST_DRWT;
	query.bAll = false;
	m_pApi->ReqQuery(&query);
}


void CSingleUser::ReqQryTrade()
{
	// 进行查询
	ReqQueryData_STRUCT query = { 0 };
	query.Client = m_pClient;
	strcpy(query.KHH, m_UserID);
	query.requestType = REQUEST_DRCJ;
	query.bAll = false;
	m_pApi->ReqQuery(&query);
}

int CSingleUser::OnResponse_ReqQryOrder(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	ReqQueryData_STRUCT* pQuery = (ReqQueryData_STRUCT*)pRespone->pContent;

	if (pRespone->pErr)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pField->Text, pRespone->pErr->ErrInfo);
		strcpy(pField->Source, "OnRespone_ReqQryOrder");

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}

	WTLB_STRUCT** ppRS = nullptr;
	CharTable2WTLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client, m_msgQueue);

	// 操作前清空，按说之前已经清空过一次了
	m_NewOrderList.clear();

	// 有未完成的，标记为true
	bool IsDone = true;
	// 有未申报的，标记为true
	bool IsNotSent = false;
	// 有更新的
	bool IsUpdated = false;

	if (ppRS)
	{
		int i = 0;
		while (ppRS[i])
		{
			// 将撤单委托过滤
			if (ppRS[i]->MMBZ_ != MMBZ_Cancel && ppRS[i]->MMBZ_ != MMBZ_Buy_Cancel && ppRS[i]->MMBZ_ != MMBZ_Sell_Cancel)
			{
				// 需要将它输入到一个地方用于计算，这个是临时的，需要删除
				OrderField* pField = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));

				WTLB_2_OrderField_0(ppRS[i], pField);
				CreateID(pField->ID, ppRS[i]->WTRQ, ppRS[i]->GDDM, ppRS[i]->WTBH);
				pField->Date = ppRS[i]->WTRQ_ == 0 ? Today(0) : ppRS[i]->WTRQ_;//日期为0，重新赋值

				m_NewOrderList.push_back(pField);

				if (!ZTSM_IsDone(ppRS[i]->ZTSM_))
				{
					IsDone = false;
				}
				if (ZTSM_IsNotSent(ppRS[i]->ZTSM_))
				{
					IsNotSent = true;
				}

				// 需要将其保存起来，是只保存一次，还是每次都更新呢？个人认为只保存一次即可，反正是用来撤单的
				unordered_map<string, WTLB_STRUCT*>::iterator it = m_pApi->m_id_api_order.find(pField->ID);
				if (it == m_pApi->m_id_api_order.end())
				{
					WTLB_STRUCT* pWTField = (WTLB_STRUCT*)m_msgQueue->new_block(sizeof(WTLB_STRUCT));
					memcpy(pWTField, ppRS[i], sizeof(WTLB_STRUCT));
					m_pApi->m_id_api_order.insert(pair<string, WTLB_STRUCT*>(pField->ID, pWTField));
				}
				else
				{
					// 如果已经存在，最好还是更新部分,要更新哪些地方呢？参考OnRespone_ReqOrderInsert部分，看哪些没有默认填写
					WTLB_STRUCT* pWTField = (WTLB_STRUCT*)it->second;
					if (strlen(pWTField->ZQMC) == 0)
					{
						memcpy(pWTField, ppRS[i], sizeof(WTLB_STRUCT));
					}
				}
			}
			++i;
		}
	}

	// 委托列表
	// 1.新增的都需要输出
	// 2.老的看是否有变化
	++m_OrderNotUpdateCount;

	int i = 0;
	list<OrderField*>::iterator it2 = m_OldOrderList.begin();
	for (list<OrderField*>::iterator it = m_NewOrderList.begin(); it != m_NewOrderList.end(); ++it)
	{
		OrderField* pField = *it;

		bool bUpdate = false;
		if (i >= m_OldOrderList.size())
		{
			bUpdate = true;
		}
		else
		{
			// 相同位置的部分
			OrderField* pOldField = *it2;
			if (pOldField->LeavesQty != pField->LeavesQty || pOldField->Status != pField->Status)
			{
				bUpdate = true;
			}
		}

		if (bUpdate)
		{
			IsUpdated = true;
			m_OrderNotUpdateCount = 0;

			// 如果能找到下单时的委托，就修改后发出来
			unordered_map<string, OrderField*>::iterator it = m_pApi->m_id_platform_order.find(pField->ID);
			if (it == m_pApi->m_id_platform_order.end())
			{
				// 因为上次生成的可能在后期删了，所以要复制一份
				OrderField* pField_ = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
				memcpy(pField_, pField, sizeof(OrderField));

				m_pApi->m_id_platform_order.insert(pair<string, OrderField*>(pField_->ID, pField_));
			}
			else
			{
				OrderField* pField_ = it->second;
				memcpy(pField_, pField, sizeof(OrderField));
			}

			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
		}

		// 前一个可能为空，移动到下一个时需要注意
		if (it2 != m_OldOrderList.end())
		{
			++it2;
		}

		++i;
	}

	if (pQuery->bAll)
	{
		int i = 0;
		int count = m_NewOrderList.size();
		for (list<OrderField*>::iterator it = m_NewOrderList.begin(); it != m_NewOrderList.end(); ++it)
		{
			OrderField* pField = *it;
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryOrder, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
			++i;
		}
	}

	// 将老数据清理，防止内存泄漏
	for (list<OrderField*>::iterator it = m_OldOrderList.begin(); it != m_OldOrderList.end(); ++it)
	{
		OrderField* pField = *it;
		m_msgQueue->delete_block(pField);
	}

	// 做交换
	m_OldOrderList.clear();
	m_OldOrderList = m_NewOrderList;
	m_NewOrderList.clear();

	double _queryTime = 0;
	if (!IsDone)
	{
		if (!IsUpdated)
		{
			// 没有更新，是否要慢点查
			_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		}

		// 有没有完成的，需要定时查询
		if (IsNotSent)
		{
			// 有没申报的，是否没在交易时间？慢点查
			_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		}
		else
		{
			// 可能是交易时间了，是否需要考虑
			_queryTime = 2 * QUERY_TIME_MIN;
			// 可能有些挂单一天都不会成交，挂在这一直导致查太多，加一下查询计数
			if (m_OrderNotUpdateCount >= 3)
			{
				_queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
			}
		}
	}
	else
	{
		// 全完成了，可以不查或慢查
		_queryTime = 5 * QUERY_TIME_MAX;
	}

	m_QueryOrderTime = time(nullptr) + _queryTime;
	OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_QueryOrder");

	// 决定成交查询间隔
	if (IsUpdated)
	{
		// 委托可能是撤单，也有可能是成交了，赶紧查一下
		_queryTime = 0;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "NextQueryTrade_QueryOrder");
	}
	else
	{
		// 委托没有变化，那成交就没有必要查那么快了
		_queryTime = 5 * QUERY_TIME_MAX;
		m_QueryTradeTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryTradeTime, _queryTime, "NextQueryTrade_QueryOrder");
	}

	DeleteStructs((void***)&ppRS, m_msgQueue);

	return 0;
}


double GetTradeListQty(list<TradeField*> &tradeList, int count)
{
	double Qty = 0;
	int i = 0;
	for (list<TradeField*>::iterator it = tradeList.begin(); it != tradeList.end(); ++it)
	{
		++i;
		if (i > count)
		{
			break;
		}

		TradeField* pField = *it;
		Qty += pField->Qty;
	}
	return Qty;
}

void TradeList2TradeMap(list<TradeField*> &tradeList, unordered_map<string, TradeField*> &tradeMap)
{
	// 只在这个函数中new和delete应当没有问题
	for (unordered_map<string, TradeField*>::iterator it = tradeMap.begin(); it != tradeMap.end(); ++it)
	{
		TradeField* pNewField = it->second;
		delete[] pNewField;
	}
	tradeMap.clear();

	// 将多个合约拼接成
	for (list<TradeField*>::iterator it = tradeList.begin(); it != tradeList.end(); ++it)
	{
		TradeField* pField = *it;
		unordered_map<string, TradeField*>::iterator it2 = tradeMap.find(pField->ID);
		if (it2 == tradeMap.end())
		{
			TradeField* pNewField = new TradeField;
			memcpy(pNewField, pField, sizeof(TradeField));
			tradeMap[pField->ID] = pNewField;
		}
		else
		{
			TradeField* pNewField = it2->second;
			pNewField->Price = pField->Price;
			pNewField->Qty += pField->Qty;
		}
	}
}

int CSingleUser::OnResponse_ReqQryTrade(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	ReqQueryData_STRUCT* pQuery = (ReqQueryData_STRUCT*)pRespone->pContent;

	if (pRespone->pErr)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pField->Text, pRespone->pErr->ErrInfo);
		strcpy(pField->Source, "OnRespone_ReqQryTrade");

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}

	CJLB_STRUCT** ppRS = nullptr;
	CharTable2CJLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client, m_msgQueue);

	// 操作前清空，按说之前已经清空过一次了
	m_NewTradeList.clear();

	if (ppRS)
	{
		// 利用成交编号的大小来判断正反序
		if (!m_TradeListReverse)
		{
			int count = GetCountStructs((void**)ppRS);
			if (count > 1)
			{
				// 转成数字的比较，是否会有非数字的情况出现？
				long CJBH0 = atol(ppRS[0]->CJBH);
				long CJBH1 = atol(ppRS[count - 1]->CJBH);
				if (CJBH0 > CJBH1)
				{
					m_TradeListReverse = true;
				}
			}
		}

		int i = 0;
		while (ppRS[i])
		{
			// 有部分券商，撤单也计成交了，真无语了，需要过滤
			if (ppRS[i]->CJSL_ <= 0)
			{
				++i;
				continue;
			}

			TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));

			CJLB_2_TradeField(ppRS[i], pField);
			CreateID(pField->ID, ppRS[i]->CJRQ, ppRS[i]->GDDM, ppRS[i]->WTBH);
			pField->Date = ppRS[i]->CJRQ_ == 0 ? Today(0) : ppRS[i]->CJRQ_;//日期为0，重新赋值

			if (m_TradeListReverse)
			{
				// 华泰查出来的表后生成的排第一，所以要处理一下
				m_NewTradeList.push_front(pField);
			}
			else
			{
				m_NewTradeList.push_back(pField);
			}

			++i;
		}
	}

	// 新查出来的反而少了，华泰有合并成交的情况，这种如何处理？
	// 对同ID的需要累加，有发现累加不对应的，应当处理
	// 同样条数的，也有可能其中的有变化，如何处理？
	bool bTryMerge = false;
	int OldTradeListCount = m_OldTradeList.size();
	int NewTradeListCount = m_NewTradeList.size();

	if (NewTradeListCount < OldTradeListCount)
	{
		// 行数变少了，应当是合并了
		bTryMerge = true;
	}
	else if (OldTradeListCount == 0)
	{
		// 如果上一次的为空，不管这次查出来的是合并还是没有合并，都没有关系，当成没合并处理即可
	}
	else if (NewTradeListCount == OldTradeListCount)
	{
		// 行数不变，但有可能是其中的一条部分成交的更新，所以检查一下

		double OldQty = GetTradeListQty(m_OldTradeList, m_OldTradeList.size());
		double NewQty = GetTradeListQty(m_NewTradeList, m_NewTradeList.size());
		if (NewQty != OldQty)
		{
			// 同样长度成交量发生了变化，可能是合并的列表其中一个新成交了
			bTryMerge = true;
		}
	}
	else
	{
		// 行数变多了，只要其中上次的部分有变化就需要检查一下
		double OldQty = GetTradeListQty(m_OldTradeList, m_OldTradeList.size());
		double NewQty = GetTradeListQty(m_NewTradeList, m_NewTradeList.size());
		if (NewQty != OldQty)
		{
			bTryMerge = true;
		}
	}

	if (bTryMerge)
	{
		// 合并列表的处理方法
		// 如果上次是合并，这次就没有必要再生成一次了
		if (m_OldTradeMap.size() == 0 || !m_LastIsMerge)
		{
			for (unordered_map<string, TradeField*>::iterator it = m_OldTradeMap.begin(); it != m_OldTradeMap.end(); ++it)
			{
				TradeField* pField = it->second;
				delete[] pField;
			}
			m_OldTradeMap.clear();

			TradeList2TradeMap(m_OldTradeList, m_OldTradeMap);
		}
		TradeList2TradeMap(m_NewTradeList, m_NewTradeMap);
		CompareTradeMapAndEmit(m_OldTradeMap, m_NewTradeMap);

		// 交换
		for (unordered_map<string, TradeField*>::iterator it = m_OldTradeMap.begin(); it != m_OldTradeMap.end(); ++it)
		{
			TradeField* pField = it->second;
			delete[] pField;
		}
		m_OldTradeMap.clear();
		m_OldTradeMap = m_NewTradeMap;
		m_NewTradeMap.clear();
		m_LastIsMerge = true;
	}
	else
	{
		// 普通的处理方法
		CompareTradeListAndEmit(m_OldTradeList, m_NewTradeList);
		m_LastIsMerge = false;
	}

	if (pQuery->bAll)
	{
		int i = 0;
		int count = m_NewTradeList.size();
		for (list<TradeField*>::iterator it = m_NewTradeList.begin(); it != m_NewTradeList.end(); ++it)
		{
			TradeField* pField = *it;
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRspQryTrade, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
			++i;
		}
	}

	// 将老数据清理，防止内存泄漏
	for (list<TradeField*>::iterator it = m_OldTradeList.begin(); it != m_OldTradeList.end(); ++it)
	{
		TradeField* pField = *it;
		m_msgQueue->delete_block(pField);
	}

	// 做交换
	m_OldTradeList.clear();
	m_OldTradeList = m_NewTradeList;
	m_NewTradeList.clear();

	DeleteStructs((void***)&ppRS, m_msgQueue);

	return 0;
}

void CSingleUser::CompareTradeMapAndEmit(unordered_map<string, TradeField*> &oldMap, unordered_map<string, TradeField*> &newMap)
{
	for (unordered_map<string, TradeField*>::iterator it = newMap.begin(); it != newMap.end(); ++it)
	{
		TradeField* pNewField = it->second;
		unordered_map<string, TradeField*>::iterator it2 = oldMap.find(pNewField->ID);
		if (it2 == oldMap.end())
		{
			// 没找到,是新单
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pNewField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		}
		else
		{
			TradeField* pOldField = it2->second;
			int Qty = pNewField->Qty - pOldField->Qty;
			if (Qty > 0)
			{
				// 有变化的单
				TradeField* pField = new TradeField;
				memcpy(pField, pNewField, sizeof(TradeField));
				pField->Qty = Qty;
				m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pNewField, sizeof(TradeField), nullptr, 0, nullptr, 0);
				delete[] pField;
			}
		}
	}
}

void CSingleUser::CompareTradeListAndEmit(list<TradeField*> &oldList, list<TradeField*> &newList)
{
	int i = 0;
	list<TradeField*>::iterator it2 = oldList.begin();
	for (list<TradeField*>::iterator it = newList.begin(); it != newList.end(); ++it)
	{
		TradeField* pField = *it;

		bool bUpdate = false;
		if (i >= oldList.size())
		{
			bUpdate = true;
		}
		//else
		//{
		//	// 相同位置的部分
		//	TradeField* pOldField = *it2;
		//	if (pOldField->Qty != pField->Qty)
		//	{
		//		bUpdate = true;
		//	}
		//}

		if (bUpdate)
		{
			m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		}

		// 前一个可能为空，移动到下一个时需要注意
		if (it2 != oldList.end())
		{
			++it2;
		}

		++i;
	}
}


int CSingleUser::OnResponse_ReqUserLogin(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	if (pRespone->pErr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pField->Text, pRespone->pErr->ErrInfo);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		m_ConnectionStatus = ConnectionStatus::ConnectionStatus_Disconnected;
		m_pApi->RemoveUser((CSingleUser*)pRespone->pUserData_Public);
	}
	else
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		m_ConnectionStatus = ConnectionStatus::ConnectionStatus_Logined;
	}
	
	// 查询股东列表，华泰证券可能一开始查会返回非知请求[1122]
	GDLB_STRUCT** ppRS = nullptr;
	char CPZH[32] = { 0 };
	CharTable2Login(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client, nullptr, CPZH);

	int count = GetCountStructs((void**)ppRS);

	if (count > 0)
	{
		for (int i = 0; i < count; ++i)
		{
			InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

			GDLB_2_InvestorField(ppRS[i], pField);

			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
		}

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
		m_ConnectionStatus = ConnectionStatus::ConnectionStatus_Done;
	}

	// 这是由TdxApi.dll创建的
	DeleteStructs((void***)&ppRS);

	return 0;
}

int CSingleUser::OnResponse_ReqQryInvestor(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	if (pRespone->pErr)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pField->Text, pRespone->pErr->ErrInfo);
		strcpy(pField->Source, "OnRespone_ReqQryInvestor");

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}

	GDLB_STRUCT** ppRS = nullptr;
	CharTable2GDLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);

	if (count>0)
	{
		for (int i = 0; i < count; ++i)
		{
			InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

			GDLB_2_InvestorField(ppRS[i], pField);

			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
		}

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
	}

	DeleteStructs((void***)&ppRS);

	return 0;
}

int CSingleUser::OnResponse_ReqQryTradingAccount(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	if (pRespone->pErr)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pField->Text, pRespone->pErr->ErrInfo);
		strcpy(pField->Source, "OnRespone_ReqQryTradingAccount");

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}

	//if (pRespone->ppResults == nullptr)
	//{

	//	return;
	//}

	ZJYE_STRUCT** ppRS = nullptr;
	CharTable2ZJYE(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client, m_msgQueue);

	int count = GetCountStructs((void**)ppRS);
	for (int i = 0; i < count; ++i)
	{
		AccountField* pField = (AccountField*)m_msgQueue->new_block(sizeof(AccountField));

		ZJYE_2_AccountField(ppRS[i], pField);

		//// 可能资金账号查不出来，手工填上
		//if (strlen(pField->AccountID) <= 0)
		//{
		//	// 多账户会有问题
		//	strcpy(pField->AccountID, m_pApi->GetAccount());
		//}

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryTradingAccount, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
	}

	DeleteStructs((void***)&ppRS, m_msgQueue);

	return 0;
}

int CSingleUser::OnResponse_ReqQryInvestorPosition(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	if (pRespone->pErr)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pField->Text, pRespone->pErr->ErrInfo);
		strcpy(pField->Source, "OnRespone_ReqQryInvestorPosition");

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}

	GFLB_STRUCT** ppRS = nullptr;
	CharTable2GFLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client, m_msgQueue);

	int count = GetCountStructs((void**)ppRS);
	for (int i = 0; i < count; ++i)
	{
		PositionField* pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));

		// 应当处理一下，可能一个账号对应的有多个，如信用账户
		GFLB_2_PositionField(ppRS[i], pField);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRspQryInvestorPosition, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
	}

	DeleteStructs((void***)&ppRS, m_msgQueue);

	return 0;
}

int CSingleUser::OnResponse_ReqOrderInsert(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	Order_STRUCT* pTdxOrder = (Order_STRUCT*)pRespone->pContent;
	OrderField* pOrder = (OrderField*)pRespone->pUserData_Public2;

	WTLB_STRUCT* pWTOrders = (WTLB_STRUCT*)m_msgQueue->new_block(sizeof(WTLB_STRUCT));
	strcpy(pWTOrders->ZJZH, pTdxOrder->ZJZH);
	strcpy(pWTOrders->ZHLB, pTdxOrder->ZHLB_);
	strcpy(pWTOrders->ZQDM, pTdxOrder->ZQDM);
	strcpy(pWTOrders->GDDM, pTdxOrder->GDDM);
	strcpy(pWTOrders->WTBH, pTdxOrder->WTBH);
	strcpy(pWTOrders->JYSDM, pTdxOrder->ZHLB_);// FIXME:交易所代码有什么办法搞到吗？

	pWTOrders->Client = m_pClient;

	m_pApi->m_id_api_order.insert(pair<string, WTLB_STRUCT*>(pOrder->LocalID, pWTOrders));
	// 处理错误
	if (pRespone->pErr)
	{
		pOrder->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pOrder->Text, pRespone->pErr->ErrInfo);
	}

	// 处理结果
	if (pRespone->ppResults&&pRespone->ppResults[0 * COL_EACH_ROW + 0])
	{
		// 写上柜台的ID，以后将基于此进行定位
		strcpy(pOrder->OrderID, pRespone->ppResults[0 * COL_EACH_ROW + 0]);//订单号
		strcpy(pWTOrders->WTBH, pOrder->OrderID);
		CreateID(pOrder->ID, nullptr, pTdxOrder->GDDM, pOrder->OrderID);
		

		m_pApi->m_id_api_order.erase(pOrder->LocalID);
		m_pApi->m_id_api_order.insert(pair<string, WTLB_STRUCT*>(pOrder->ID, pWTOrders));

		m_pApi->m_id_platform_order.erase(pOrder->LocalID);
		m_pApi->m_id_platform_order.insert(pair<string, OrderField*>(pOrder->ID, pOrder));
		
		// 有挂单的，需要进行查询了

		double  _queryTime = QUERY_TIME_MIN;
		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_Send");
	}

	// 现在有两个结构体，需要进行操作了
	// 1.通知下单的结果
	// 2.记录下单

	OrderField* pField = pOrder;
	if (pField->RawErrorID != 0)
	{
		pField->ExecType = ExecType::ExecType_Rejected;
		pField->Status = OrderStatus::OrderStatus_Rejected;
	}
	else
	{
		pField->ExecType = ExecType::ExecType_New;
		pField->Status = OrderStatus::OrderStatus_New;
	}

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

	return 0;
}

int CSingleUser::OnResponse_ReqOrderAction(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	WTLB_STRUCT* pTdxOrder = (WTLB_STRUCT*)pRespone->pContent;
	OrderField* pOrder = (OrderField*)pRespone->pUserData_Public2;

	if (pRespone->pErr)
	{
		pOrder->RawErrorID = pRespone->pErr->ErrCode;
		strcpy(pOrder->Text, pRespone->pErr->ErrInfo);

		pOrder->ExecType = ExecType::ExecType_CancelReject;
		// 注意报单状态问题，交给报单查询来处理
	}
	else
	{
		double  _queryTime = QUERY_TIME_MIN;
		m_QueryOrderTime = time(nullptr) + _queryTime;
		OutputQueryTime(m_QueryOrderTime, _queryTime, "NextQueryOrder_Cancel");

		// 会不会出现撤单时，当时不知道是否成功撤单，查询才得知没有撤成功？
		//ppOrders[i]->ExecType = ExecType::ExecCancelled;
		//ppOrders[i]->Status = OrderStatus::Cancelled;
	}

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pOrder, sizeof(OrderField), nullptr, 0, nullptr, 0);

	return 0;
}