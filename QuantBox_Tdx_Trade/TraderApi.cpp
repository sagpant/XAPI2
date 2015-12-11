#include "stdafx.h"
#include "TraderApi.h"

#include "../include/QueueEnum.h"
#include "../include/QueueHeader.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

#include "../include/ApiProcess.h"
#include "../include/toolkit.h"

#include "../QuantBox_Queue/MsgQueue.h"

#include "TypeConvert.h"

#include "../include/Tdx/tdx_function.h"

#include <cstring>
#include <assert.h>


void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CTraderApi* pApi = (CTraderApi*)pApi2;
	pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CTraderApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	int iRet = 0;
	switch (type)
	{
	case E_Init:
		iRet = _Init();
		break;
	case E_ReqUserLoginField:
		iRet = _ReqUserLogin(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_InputOrderField:
		iRet = _ReqOrderInsert(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_InputOrderActionField:
		iRet = _ReqOrderAction(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case E_QryDepthMarketDataField:
		iRet = _Subscribe(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;

	case QueryType::ReqQryOrder :
		iRet = _ReqQryOrder(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case QueryType::ReqQryTrade:
		iRet = _ReqQryTrade(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case QueryType::ReqQryInvestor:
		iRet = _ReqQryInvestor(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case QueryType::ReqQryTradingAccount:
		iRet = _ReqQryTradingAccount(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	case QueryType::ReqQryInvestorPosition:
		iRet = _ReqQryInvestorPosition(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	
	default:
		break;
	}

	if (0 == iRet)
	{
		//返回成功，填加到已发送池
		m_nSleep = 1;
	}
	else
	{
		m_msgQueue_Query->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		//失败，按4的幂进行延时，但不超过1s
		m_nSleep *= 4;
		m_nSleep %= 1023;
	}
	this_thread::sleep_for(chrono::milliseconds(m_nSleep));
}

void* __stdcall Test(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	// 由内部调用，不用检查是否为空
	CTraderApi* pApi = (CTraderApi*)pApi2;
	pApi->TestInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
	return nullptr;
}

void CTraderApi::TestInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	time_t _now = time(nullptr);

	for (unordered_map<void*, CSingleUser*>::iterator it = m_Client_SingleUser.begin(); it != m_Client_SingleUser.end(); ++it)
	{
		CSingleUser* pUser = it->second;
		if (_now > pUser->m_QueryOrderTime)
		{
			//double _queryTime = QUERY_TIME_MAX;
			//m_QueryOrderTime = time(nullptr) + _queryTime;
			//OutputQueryTime(m_QueryOrderTime, _queryTime, "QueryOrder");

			pUser->ReqQryOrder();
		}

		if (_now > pUser->m_QueryTradeTime)
		{
			//double _queryTime = QUERY_TIME_MAX;
			//m_QueryTradeTime = time(nullptr) + _queryTime;
			//OutputQueryTime(m_QueryTradeTime, _queryTime, "QueryTrade");

			pUser->ReqQryTrade();
		}
	}

	this_thread::sleep_for(chrono::milliseconds(1000));
	m_msgQueue_Test->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
}

void CTraderApi::OutputQueryTime(time_t t, double db, const char* szSource)
{
	LogField* pField = (LogField*)m_msgQueue->new_block(sizeof(LogField));

	sprintf(pField->Message, "Source:%s,Add:%d,Time:%s", szSource,(int)db, ctime(&t));

	// 去了最后的回车
	int len = strlen(pField->Message);
	pField->Message[len - 1] = 0;
	
	m_msgQueue->Input_NoCopy(ResponeType::OnLog, m_msgQueue, m_pClass, true, 0, pField, sizeof(LogField), nullptr, 0, nullptr, 0);
}

void CTraderApi::OnRespone(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	if (pRespone == nullptr)
		return;

	switch (pRespone->requestType)
	{
	case REQUEST_WT:
		break;
	case REQUEST_HQ:
		OnRespone_Subscribe(pApi, pRespone);
		break;
	case REQUEST_GDLB:
		OnRespone_ReqQryInvestor(pApi, pRespone);
		break;
	case REQUEST_DRWT:
		OnRespone_ReqQryOrder(pApi, pRespone);
		break;
	case REQUEST_DRCJ:
		OnRespone_ReqQryTrade(pApi, pRespone);
		break;
	case REQUEST_ZJYE:
		OnRespone_ReqQryTradingAccount(pApi, pRespone);
		break;
	case REQUEST_GFLB:
		OnRespone_ReqQryInvestorPosition(pApi, pRespone);
		break;
	default:
		break;
	}
}

int CTraderApi::_Init()
{
	if (m_pApi == nullptr)
	{
		m_pApi = CTdxApi::CreateApi(m_ServerInfo.ExtInfoChar128);
	}

	Error_STRUCT* pErr = nullptr;

	m_pApi->LoadScript(m_ServerInfo.Address, true, false);
	m_pApi->Init(m_ServerInfo.ExtInfoChar128, &pErr);

	if (pErr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->RawErrorID = pErr->ErrCode;
		strcpy(pField->Text, pErr->ErrInfo);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	else
	{
		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		ReqUserLogin();
	}

	DeleteError(pErr);

	return 0;
}

void CTraderApi::ReqUserLogin()
{
	if (m_UserInfo_Pos >= m_UserInfo_Count)
		return;

	m_msgQueue_Query->Input_NoCopy(RequestType::E_ReqUserLoginField, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestRespone_STRUCT LoginRespone = { 0 };

	m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	m_pClient = m_pApi->Login(m_UserInfo.UserID, m_UserInfo.Password, m_UserInfo.ExtInfoChar64, &LoginRespone);

	if (m_pClient)
	{
		// 有授权信息要输出
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		if (LoginRespone.pErr)
		{
			pField->RawErrorID = LoginRespone.pErr->ErrCode;
			strcpy(pField->Text, LoginRespone.pErr->ErrInfo);
		}

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, pField, sizeof(pField), nullptr, 0, nullptr, 0);


		// 查询股东列表，华泰证券可能一开始查会返回非知请求[1122]
		GDLB_STRUCT** ppRS = nullptr;
		CharTable2Login(LoginRespone.ppResults, &ppRS, m_pClient);

		int count = GetCountStructs((void**)ppRS);

		if (count>0)
		{
			for (int i = 0; i < count; ++i)
			{
				InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

				GDLB_2_InvestorField(ppRS[i], pField);

				m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
			}
		}
		
		// 启动定时查询功能使用
		m_msgQueue_Test->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue_Test, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);

		m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);


		CSingleUser* pUser = new CSingleUser();
		m_Client_SingleUser.insert(pair<void*, CSingleUser*>(m_pClient, pUser));
		m_UserID_Client.insert(pair<string, void*>(m_UserInfo.UserID, m_pClient));

		// 登录下一个账号
		//++m_UserInfo_Pos;
		//ReqUserLogin();
	}
	else
	{
		if (LoginRespone.pErr)
		{
			RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

			pField->RawErrorID = LoginRespone.pErr->ErrCode;
			strcpy(pField->Text, LoginRespone.pErr->ErrInfo);

			m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		}
	}

	DeleteRequestRespone(&LoginRespone);

	return 0;
}

void CTraderApi::ReqQuery(QueryType type, ReqQueryField* pQuery)
{
	m_msgQueue_Query->Input_Copy(type, m_msgQueue_Query, this, 0, 0,
		pQuery, sizeof(ReqQueryField), nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryInvestor(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	ReqQueryData_STRUCT query = { 0 };
	//query.Client = pClient;
	strcpy(query.KHH, pQuery->ClientID);
	strcpy(query.GDDM, pQuery->AccountID);
	m_pApi->SendRequest(m_pApi->MakeQueryData(REQUEST_GDLB, &query));

	return 0;
}

int CTraderApi::OnRespone_ReqQryInvestor(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	GDLB_STRUCT** ppRS = nullptr;
	CharTable2GDLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);

	for (int i = 0; i < count; ++i)
	{
		InvestorField* pField = (InvestorField*)m_msgQueue->new_block(sizeof(InvestorField));

		GDLB_2_InvestorField(ppRS[i], pField);

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestor, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(InvestorField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

CTraderApi::CTraderApi(void)
{
	m_pIDGenerator = nullptr;
	m_pApi = nullptr;
	m_pClient = nullptr;
	m_lRequestID = 0;
	m_nSleep = 1;

	// 自己维护两个消息队列
	m_msgQueue = new CMsgQueue();
	m_msgQueue_Query = new CMsgQueue();
	m_msgQueue_Test = new CMsgQueue();

	m_msgQueue_Query->Register(Query,this);
	m_msgQueue_Query->StartThread();

	m_msgQueue_Test->Register(Test, this);
	m_msgQueue_Test->StartThread();
}


CTraderApi::~CTraderApi(void)
{
	Disconnect();
}

void CTraderApi::Register(void* pCallback, void* pClass)
{
	m_pClass = pClass;
	if (m_msgQueue == nullptr)
		return;

	m_msgQueue_Query->Register(Query,this);
	m_msgQueue_Test->Register(Test, this);
	m_msgQueue->Register(pCallback,this);
	if (pCallback)
	{
		m_msgQueue_Query->StartThread();
		m_msgQueue->StartThread();
		m_msgQueue_Test->StartThread();
	}
	else
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue->StopThread();
		m_msgQueue_Test->StopThread();
	}
}

bool CTraderApi::IsErrorRspInfo(const char* szSource, Error_STRUCT *pRspInfo)
{
	bool bRet = ((pRspInfo) && (pRspInfo->ErrType != 0));
	if (bRet)
	{
		ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));

		pField->RawErrorID = pRspInfo->ErrCode;
		strcpy(pField->Text, pRspInfo->ErrInfo);
		strcpy(pField->Source, szSource);

		m_msgQueue->Input_NoCopy(ResponeType::OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
	}
	return bRet;
}

void CTraderApi::Connect(const string& szPath,
	ServerInfoField* pServerInfo,
	UserInfoField* pUserInfo,
	int count)
{
	m_szPath = szPath;
	memcpy(&m_ServerInfo, pServerInfo, sizeof(ServerInfoField));
	memcpy(&m_UserInfo, pUserInfo, sizeof(UserInfoField));

	m_pUserInfos = (UserInfoField*)(new char[sizeof(UserInfoField)*count]);
	memcpy(m_pUserInfos, pUserInfo, sizeof(UserInfoField)*count);

	m_UserInfo_Pos = 0;
	m_UserInfo_Count = count;

	m_pIDGenerator = new CIDGenerator();
	m_pIDGenerator->SetPrefix(m_UserInfo.UserID);

	m_msgQueue_Query->Input_NoCopy(E_Init, m_msgQueue_Query, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CTraderApi::Disconnect()
{
	if (m_msgQueue_Query)
	{
		m_msgQueue_Query->StopThread();
		m_msgQueue_Query->Register(nullptr,nullptr);
		m_msgQueue_Query->Clear();
		delete m_msgQueue_Query;
		m_msgQueue_Query = nullptr;
	}

	if (m_msgQueue_Test)
	{
		m_msgQueue_Test->StopThread();
		m_msgQueue_Test->Register(nullptr, nullptr);
		m_msgQueue_Test->Clear();
		delete m_msgQueue_Test;
		m_msgQueue_Test = nullptr;
	}

	if(m_pApi)
	{
		// 还没有登出
		m_pApi->Logout(m_pClient);
		m_pClient = nullptr;
		m_pApi->Exit();
		
		m_pApi->Release();
		m_pApi = nullptr;

		// 全清理，只留最后一个
		if (m_msgQueue)
		{
			m_msgQueue->Clear();
			m_msgQueue->Input_NoCopy(ResponeType::OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
			// 主动触发
			m_msgQueue->Process();
		}
	}

	if (m_msgQueue)
	{
		m_msgQueue->StopThread();
		m_msgQueue->Register(nullptr,nullptr);
		m_msgQueue->Clear();
		delete m_msgQueue;
		m_msgQueue = nullptr;
	}

	if (m_pIDGenerator)
	{
		delete m_pIDGenerator;
		m_pIDGenerator = nullptr;
	}
}

char* CTraderApi::ReqOrderInsert(
	OrderField* pOrder,
	int count,
	char* pszLocalIDBuf)
{
	memset(pszLocalIDBuf, 0, sizeof(OrderIDType)*count);

	if (count < 1)
		return 0;	

	OrderField** ppOrders = new OrderField*[count];
	
	// 生成本地ID，供上层进行定位使用
	for (int i = 0; i < count; ++i)
	{
		OrderField* pNewOrder = (OrderField*)m_msgQueue->new_block(sizeof(OrderField));
		memcpy(pNewOrder, &pOrder[i], sizeof(OrderField));

		strcpy(pNewOrder->LocalID, m_pIDGenerator->GetIDString());
		strcpy(pNewOrder->ID, pNewOrder->LocalID);
		strcat(pszLocalIDBuf, pNewOrder->LocalID);
		
		if (i <count-1)
		{
			strcat(pszLocalIDBuf, ";");
		}

		ppOrders[i] = pNewOrder;

		// 注意这里保存了最开始发单的结构体的备份
		m_id_platform_order.insert(pair<string, OrderField*>(pNewOrder->LocalID, pNewOrder));
	}

	m_msgQueue_Query->Input_Copy(RequestType::E_InputOrderField, m_msgQueue_Query, this, 0, 0,
		ppOrders, sizeof(OrderField*)*count, nullptr, 0, nullptr, 0);

	delete[] ppOrders;
	
	return pszLocalIDBuf;
}

int CTraderApi::_ReqOrderInsert(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	// 得到报单指针列表
	OrderField** ppOrders = (OrderField**)ptr1;
	int count = (int)size1 / sizeof(OrderField*);

	for (int i = 0; i < count; ++i)
	{
		RequestRespone_STRUCT* pRequest = (RequestRespone_STRUCT*)m_msgQueue->new_block(sizeof(RequestRespone_STRUCT));
		OrderField_2_Order_STRUCT(ppOrders[i], (Order_STRUCT*)pRequest->pContent);
		// 这地方需要对一些值进行修正

		m_pApi->SendRequest(m_pApi->MakeOrder(pRequest->requestType, (Order_STRUCT*)pRequest->pContent));
	}

	return 0;
}

char* CTraderApi::ReqOrderAction(OrderIDType* szId, int count, char* pzsRtn)
{
	memset(pzsRtn, 0, sizeof(OrderIDType)*count);

	OrderField** ppOrders = new OrderField*[count];
	WTLB_STRUCT** ppTdxOrders = new WTLB_STRUCT*[count];

	for (int i = 0; i < count; ++i)
	{
		ppOrders[i] = nullptr;
		ppTdxOrders[i] = nullptr;

		{
			unordered_map<string, OrderField*>::iterator it = m_id_platform_order.find(szId[i]);
			if (it != m_id_platform_order.end())
				ppOrders[i] = it->second;
			else
			{
				OrderField* pField = new OrderField();
				ppOrders[i] = pField;
				strcpy(pField->ID, szId[i]);
				strcpy(pField->Symbol, "*.*");
				strcpy(pField->InstrumentID, "*");
				strcpy(pField->ExchangeID, "*");
			}
		}
		
		{
			unordered_map<string, WTLB_STRUCT*>::iterator it = m_id_api_order.find(szId[i]);
			if (it != m_id_api_order.end())
				ppTdxOrders[i] = it->second;
		}

		if (i < count - 1)
		{
			strcat(pzsRtn, ";");
		}
	}

	m_msgQueue_Query->Input_Copy(RequestType::E_InputOrderActionField, m_msgQueue_Query, this, 0, 0,
		ppOrders, sizeof(OrderField*)*count, ppTdxOrders, sizeof(WTLB_STRUCT*)*count, nullptr, 0);

	delete[] ppOrders;
	delete[] ppTdxOrders;

	return pzsRtn;
}

int CTraderApi::_ReqOrderAction(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	//int count = (int)size1 / sizeof(OrderField*);
	//// 通过ID找到原始结构，用于撤单
	//// 通过ID找到通用结构，用于接收回报
	//// 这里传过来的的是已经被复制过的内容
	//OrderField** ppOrders = (OrderField**)ptr1;
	//WTLB_STRUCT** ppTdxOrders = (WTLB_STRUCT**)ptr2;

	//FieldInfo_STRUCT** ppFieldInfos = nullptr;
	//char** ppResults = nullptr;
	//Error_STRUCT** ppErrs = nullptr;

	//int n = m_pApi->CancelMultiOrders(ppTdxOrders, count, &ppFieldInfos, &ppResults, &ppErrs);

	//bool bSuccess = false;
	//// 将结果立即取出来
	//for (int i = 0; i < count; ++i)
	//{
	//	if (ppErrs)
	//	{
	//		if (ppErrs[i])
	//		{
	//			ppOrders[i]->RawErrorID = ppErrs[i]->ErrCode;
	//			strcpy(ppOrders[i]->Text, ppErrs[i]->ErrInfo);

	//			ppOrders[i]->ExecType = ExecType::ExecType_CancelReject;
	//			// 注意报单状态问题，交给报单查询来处理
	//		}
	//		else
	//		{
	//			unordered_map<void*, CSingleUser*>::iterator it = m_Client_SingleUser.find(ppTdxOrders[i]->Client);
	//			if (it == m_Client_SingleUser.end())
	//			{
	//				assert(false);
	//			}
	//			else
	//			{
	//				// 有挂单的，需要进行查询了
	//				CSingleUser* pUser = it->second;

	//				double  _queryTime = QUERY_TIME_MIN;
	//				double _queryOrderTime = time(nullptr) + _queryTime;
	//				pUser->UpdateQueryOrderTime(_queryOrderTime, _queryTime, "NextQueryOrder_Cancel");
	//			}

	//			// 会不会出现撤单时，当时不知道是否成功撤单，查询才得知没有撤成功？
	//			//ppOrders[i]->ExecType = ExecType::ExecCancelled;
	//			//ppOrders[i]->Status = OrderStatus::Cancelled;
	//			continue;
	//		}
	//	}
	//	// 撤单成功时，返回的东西还是null,所以这里使用错误信息来进行区分
	//	
	//	//if (ppResults)
	//	//{	
	//	//	if (ppResults[i*COL_EACH_ROW + 0])
	//	//	{
	//	//	}
	//	//}

	//	m_msgQueue->Input_Copy(ResponeType::OnRtnOrder, m_msgQueue, m_pClass, 0, 0, ppOrders[i], sizeof(OrderField), nullptr, 0, nullptr, 0);
	//}

	//DeleteTableBody(ppResults, count);
	//DeleteErrors(ppErrs, count);

	return 0;
}

void CTraderApi::ReqQryOrder()
{
	m_msgQueue_Query->Input_NoCopy(QueryType::ReqQryOrder, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	ReqQueryData_STRUCT query = { 0 };
	//query.Client = pClient;
	strcpy(query.KHH, pQuery->ClientID);
	strcpy(query.GDDM, pQuery->AccountID);
	m_pApi->SendRequest(m_pApi->MakeQueryData(REQUEST_DRWT, &query));

	return 0;

	//FieldInfo_STRUCT** ppFieldInfos = nullptr;
	//char** ppResults = nullptr;
	//Error_STRUCT* pErr = nullptr;

	//ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	//ReqQueryData_STRUCT query = { 0 };
	//strcpy(query.KHH, pQuery->ClientID);
	////strcpy(query.GDDM, pQuery->AccountID);

	//m_pApi->ReqQueryData(REQUEST_DRWT, &ppFieldInfos, &ppResults, &pErr, &query);

	//if (IsErrorRspInfo("ReqQryOrder", pErr))
	//{
	//	double _queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
	//	m_QueryOrderTime = time(nullptr) + _queryTime;

	//	DeleteTableBody(ppResults);
	//	DeleteError(pErr);

	//	return 0;
	//}

	//
}

int CTraderApi::OnRespone_ReqQryOrder(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	WTLB_STRUCT** ppRS = nullptr;
	CharTable2WTLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	CSingleUser* pUser = (CSingleUser*)pRespone->pUserData_Public;

	pUser->OnRespone_ReqQryOrder(pApi, pRespone, ppRS);

	return 0;
}

void CTraderApi::ReqQryTrade()
{
	m_msgQueue_Query->Input_NoCopy(QueryType::ReqQryTrade, m_msgQueue_Query, this, 0, 0,
		nullptr, 0, nullptr, 0, nullptr, 0);
}

int CTraderApi::_ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	ReqQueryData_STRUCT query = { 0 };
	//query.Client = pClient;
	strcpy(query.KHH, pQuery->ClientID);
	strcpy(query.GDDM, pQuery->AccountID);
	m_pApi->SendRequest(m_pApi->MakeQueryData(REQUEST_DRCJ, &query));

	return 0;

	/*if (IsErrorRspInfo("ReqQryTrade", pErr))
	{
		double _queryTime = 0.5 * QUERY_TIME_MAX + QUERY_TIME_MIN;
		m_QueryTradeTime = time(nullptr) + _queryTime;

		DeleteTableBody(ppResults);
		DeleteError(pErr);

		return 0;
	}

	*/
}

int CTraderApi::OnRespone_ReqQryTrade(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	CJLB_STRUCT** ppRS = nullptr;
	CharTable2CJLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	CSingleUser* pUser = (CSingleUser*)pRespone->pUserData_Public;
	
	pUser->OnRespone_ReqQryTrade(pApi, pRespone, ppRS);	

	return 0;
}

int CTraderApi::_ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	ReqQueryData_STRUCT query = { 0 };
	//query.Client = pClient;
	strcpy(query.KHH, pQuery->ClientID);
	strcpy(query.GDDM, pQuery->AccountID);
	m_pApi->SendRequest(m_pApi->MakeQueryData(REQUEST_ZJYE, &query));

	return 0;
}

int CTraderApi::OnRespone_ReqQryTradingAccount(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	ZJYE_STRUCT** ppRS = nullptr;
	CharTable2ZJYE(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

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

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryTradingAccount, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(AccountField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

int CTraderApi::_ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	ReqQueryField* pQuery = (ReqQueryField*)ptr1;

	ReqQueryData_STRUCT query = { 0 };
	//query.Client = pClient;
	strcpy(query.KHH, pQuery->ClientID);
	strcpy(query.GDDM, pQuery->AccountID);
	m_pApi->SendRequest(m_pApi->MakeQueryData(REQUEST_GFLB, &query));

	return 0;
}

int CTraderApi::OnRespone_ReqQryInvestorPosition(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	GFLB_STRUCT** ppRS = nullptr;
	CharTable2GFLB(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);
	for (int i = 0; i < count; ++i)
	{
		PositionField* pField = (PositionField*)m_msgQueue->new_block(sizeof(PositionField));

		// 应当处理一下，可能一个账号对应的有多个，如信用账户
		GFLB_2_PositionField(ppRS[i], pField);

		m_msgQueue->Input_NoCopy(ResponeType::OnRspQryInvestorPosition, m_msgQueue, m_pClass, i == count - 1, 0, pField, sizeof(PositionField), nullptr, 0, nullptr, 0);
	}

	return 0;
}

void CTraderApi::Subscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
	m_msgQueue_Query->Input_Copy(RequestType::E_QryDepthMarketDataField, m_msgQueue_Query, this, 0, 0,
		(void*)szInstrumentIDs.c_str(), szInstrumentIDs.length() + 1, (void*)szExchangeID.c_str(), szExchangeID.length() + 1, nullptr, 0);
}

int CTraderApi::_Subscribe(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	ReqQueryData_STRUCT query = { 0 };
	//query.Client = pClient;
	strcpy(query.KSRQ, "20151109");
	strcpy(query.ZZRQ, "20151209");
	strcpy(query.ZQDM, (char*)ptr1);
	m_pApi->SendRequest(m_pApi->MakeQueryData(REQUEST_HQ, &query));

	return 0;
}

int CTraderApi::OnRespone_Subscribe(CTdxApi* pApi, RequestRespone_STRUCT* pRespone)
{
	HQ_STRUCT** ppRS = nullptr;
	CharTable2HQ(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client);

	int count = GetCountStructs((void**)ppRS);

	for (int i = 0; i < count; ++i)
	{
		DepthMarketDataNField* pField = (DepthMarketDataNField*)m_msgQueue->new_block(sizeof(DepthMarketDataNField)+sizeof(DepthField)* 10);

		HQ_STRUCT* pDepthMarketData = ppRS[i];

		strcpy(pField->InstrumentID, pDepthMarketData->ZQDM);
		//pField->Exchange = JYSDM_2_ExchangeType(pDepthMarketData->JYSDM);

		sprintf(pField->Symbol, "%s.%s", pField->InstrumentID, "");

		// 交易时间
		GetExchangeTime(time(nullptr), &pField->TradingDay, &pField->ActionDay, &pField->UpdateTime);


		pField->LastPrice = pDepthMarketData->DQJ_;
		//pField->Volume = 0;
		//pField->Turnover = pDepthMarketData->Turnover;
		//pField->OpenInterest = pDepthMarketData->OpenInterest;
		//pField->AveragePrice = pDepthMarketData->AveragePrice;
		pField->UpperLimitPrice = pDepthMarketData->ZTJG_;
		pField->LowerLimitPrice = pDepthMarketData->DTJG_;

		pField->PreClosePrice = pDepthMarketData->ZSJ_;
		pField->OpenPrice = pDepthMarketData->JKJ_;

		InitBidAsk(pField);

		do
		{
			if (pDepthMarketData->BidSize1_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice1_, pDepthMarketData->BidSize1_, 0);

			if (pDepthMarketData->BidSize2_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice2_, pDepthMarketData->BidSize2_, 0);

			if (pDepthMarketData->BidSize3_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice3_, pDepthMarketData->BidSize3_, 0);

			if (pDepthMarketData->BidSize4_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice4_, pDepthMarketData->BidSize4_, 0);

			if (pDepthMarketData->BidSize5_ == 0)
				break;
			AddBid(pField, pDepthMarketData->BidPrice5_, pDepthMarketData->BidSize5_, 0);
		} while (false);

		do
		{
			if (pDepthMarketData->AskSize1_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice1_, pDepthMarketData->AskSize1_, 0);

			if (pDepthMarketData->AskSize2_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice2_, pDepthMarketData->AskSize2_, 0);

			if (pDepthMarketData->AskSize3_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice3_, pDepthMarketData->AskSize3_, 0);

			if (pDepthMarketData->AskSize4_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice4_, pDepthMarketData->AskSize4_, 0);

			if (pDepthMarketData->AskSize5_ == 0)
				break;
			AddAsk(pField, pDepthMarketData->AskPrice5_, pDepthMarketData->AskSize5_, 0);
		} while (false);

		m_msgQueue->Input_NoCopy(ResponeType::OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
	}

	return 0;
}
