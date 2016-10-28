#include "stdafx.h"
#include "TraderApi.h"

#include "../../include/QueueEnum.h"
#include "../../include/QueueHeader.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/ApiProcess.h"
#include "../../include/toolkit.h"

#include "../../common/Queue/MsgQueue.h"

#include "TypeConvert.h"

#include "../../include/Tdx/tdx_function.h"

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

	//case QueryType::ReqQryOrder :
	//case QueryType::ReqQryTrade:
	//case QueryType::ReqQryInvestor:
	//case QueryType::ReqQryTradingAccount:
	//case QueryType::ReqQryInvestorPosition:
	case E_ReqQueryData_STRUCT:
		iRet = _ReqQuery(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
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

	for (unordered_map<void*, CSingleUser*>::iterator it = m_Client_User_Map.begin(); it != m_Client_User_Map.end(); ++it)
	{
		CSingleUser* pUser = it->second;
		pUser->CheckThenQueryOrder(_now);
		pUser->CheckThenQueryTrade(_now);
	}

	this_thread::sleep_for(chrono::milliseconds(1000));
	m_msgQueue_Test->Input_Copy(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
}

void CTraderApi::OnResponse(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	if (pRespone == nullptr)
		return;

	CSingleUser* pUser = (CSingleUser*)pRespone->pUserData_Public;
	if (pUser == nullptr)
	{
		// 这个地方主要是因为Login时立即发送了登录结果出来，这个地方如果改一下也许会好一些
		unordered_map<void*, CSingleUser*>::iterator it = m_Client_User_Map.find(pRespone->Client);
		if (it == m_Client_User_Map.end())
		{
			pUser = m_pDefaultUser;
		}
		else
		{
			pUser = it->second;
		}
		pRespone->pUserData_Public = pUser;
	}

	switch (pRespone->requestType)
	{
	case 0:
		pUser->OnResponse_ReqUserLogin(pApi, pRespone);
		break;
	case REQUEST_WT:
	case REQUEST_ETF_SGSH:
	case REQUEST_ZGHS:
	case REQUEST_WT_QQ:
		pUser->OnResponse_ReqOrderInsert(pApi, pRespone);
		break;
	case REQUEST_CD:
		pUser->OnResponse_ReqOrderAction(pApi, pRespone);
		break;
	case REQUEST_HQ:
		OnResponse_Subscribe(pApi, pRespone);
		break;
	case REQUEST_GDLB:
		pUser->OnResponse_ReqQryInvestor(pApi, pRespone);
		break;
	case REQUEST_DRWT:
		pUser->OnResponse_ReqQryOrder(pApi, pRespone);
		break;
	case REQUEST_DRCJ:
		pUser->OnResponse_ReqQryTrade(pApi, pRespone);
		break;
	case REQUEST_ZJYE:
		pUser->OnResponse_ReqQryTradingAccount(pApi, pRespone);
		break;
	case REQUEST_GFLB:
		pUser->OnResponse_ReqQryInvestorPosition(pApi, pRespone);
		break;
	default:
		break;
	}
}

int CTraderApi::_Init()
{
	if (m_pApi == nullptr)
	{
		m_pApi = CTdxApi::CreateApi();
		m_pApi->RegisterSpi(this);
	}

	Error_STRUCT* pErr = nullptr;

	m_pApi->LoadScript(m_ServerInfo.Address, true, false);
	m_pApi->Init(m_ServerInfo.ExtInfoChar128, &pErr);

	if (pErr)
	{
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

		pField->RawErrorID = pErr->ErrCode;
		strcpy(pField->Text, pErr->ErrInfo);

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
	}
	else
	{
		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Initialized, 0, nullptr, 0, nullptr, 0, nullptr, 0);

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
	Error_STRUCT* pErr = nullptr;

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logining, 0, nullptr, 0, nullptr, 0, nullptr, 0);

	CSingleUser* pUser = new CSingleUser(this);
	pUser->m_pClass = m_pClass;
	pUser->m_msgQueue = m_msgQueue;
	strcpy(pUser->m_UserID, m_UserInfo.UserID);
	m_UserID_User_Map.insert(pair<string, CSingleUser*>(m_UserInfo.UserID, pUser));

	if (m_pDefaultUser == nullptr)
	{
		m_pDefaultUser = pUser;
	}

	void* pClient = m_pApi->Login(m_UserInfo.UserID, m_UserInfo.Password, m_UserInfo.ExtInfoChar64, &pErr);
	pUser->m_pClient = pClient;
	m_Client_User_Map.insert(pair<void*, CSingleUser*>(pClient, pUser));

	if (pClient)
	{
		// 有授权信息要输出
		RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
		if (pErr)
		{
			pField->RawErrorID = pErr->ErrCode;
			strcpy(pField->Text, pErr->ErrInfo);
		}

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Logined, 0, pField, sizeof(pField), nullptr, 0, nullptr, 0);
		
		
		// 登录下一个账号
		//++m_UserInfo_Pos;
		//ReqUserLogin();
	}
	else
	{
		if (pErr)
		{
			RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));

			pField->RawErrorID = pErr->ErrCode;
			strcpy(pField->Text, pErr->ErrInfo);

			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
		}
	}

	DeleteError(pErr);

	return 0;
}

void CTraderApi::StartQueryThread()
{
	// 启动定时查询功能使用
	m_msgQueue_Test->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue_Test, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CTraderApi::RemoveUser(CSingleUser* pUser)
{
	m_UserID_User_Map.erase(pUser->m_UserID);
	m_Client_User_Map.erase(pUser->m_pClient);
}

void CTraderApi::ReqQuery(QueryType type, ReqQueryField* pQuery)
{
	ReqQueryData_STRUCT query = { 0 };
	strcpy(query.KHH, pQuery->ClientID);
	strcpy(query.ZJZH, pQuery->ClientID);//通达信模拟上这样写是否会出问题
	strcpy(query.GDDM, pQuery->AccountID);
	strcpy(query.ZQDM, pQuery->InstrumentID);
	sprintf_s(query.KSRQ,"%d",pQuery->DateStart);
	sprintf_s(query.ZZRQ, "%d", pQuery->DateEnd);
	query.bAll = pQuery->Int32ID == -1;

	//printf("===%d,%d", pQuery->Int32ID, query.bAll);

	switch (type)
	{
	case QueryType_ReqQryInstrument:
		break;
	case QueryType_ReqQryTradingAccount:
		query.requestType = REQUEST_ZJYE;
		break;
	case QueryType_ReqQryInvestorPosition:
		query.requestType = REQUEST_GFLB;
		break;
	case QueryType::QueryType_ReqQryOrder:
		query.requestType = REQUEST_DRWT;
		break;
	case QueryType::QueryType_ReqQryTrade:
		query.requestType = REQUEST_DRCJ;
		break;
	case QueryType_ReqQryQuote:
		break;
	case QueryType_ReqQryInstrumentCommissionRate:
		break;
	case QueryType_ReqQryInstrumentMarginRate:
		break;
	case QueryType_ReqQrySettlementInfo:
		break;
	case QueryType_ReqQryInvestor:
		query.requestType = REQUEST_GDLB;
		break;
	case QueryType_ReqQryHistoricalTicks:
		break;
	case QueryType_ReqQryHistoricalBars:
		break;
	default:
		break;
	}

	ReqQuery(&query);

	return;
}

void CTraderApi::ReqQuery(ReqQueryData_STRUCT* pQuery)
{
	// 直接转发请求，不进行处理
	m_msgQueue_Query->Input_Copy(E_ReqQueryData_STRUCT, m_msgQueue_Query, this, 0, 0,
		pQuery, sizeof(ReqQueryData_STRUCT), nullptr, 0, nullptr, 0);

	return;
}


int CTraderApi::_ReqQuery(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	if (m_pApi == nullptr)
		return 0;

	ReqQueryData_STRUCT* pQuery = (ReqQueryData_STRUCT*)ptr1;

	CSingleUser* pUser = Fill_UserID_Client(pQuery->KHH, &pQuery->Client);
	RequestResponse_STRUCT* pRequest = m_pApi->MakeQueryData(pQuery);
	pRequest->Client = pQuery->Client;
	pRequest->pUserData_Public = pUser;
	strcpy(pRequest->khh,pQuery->KHH);

	m_pApi->SendRequest(pRequest);

	return 0;
}



CTraderApi::CTraderApi(void)
{
	// 弹出这个只是为了在Python这类的平台中调试，以后可能要关闭
	//AllocConsole();
	//freopen("CON", "r", stdin);
	//freopen("CON", "w", stdout);
	//freopen("CON", "w", stderr);
	//::DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
	//printf("===**********************===");

	m_pIDGenerator = nullptr;
	m_pApi = nullptr;
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

	m_pDefaultUser = nullptr;
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

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnError, m_msgQueue, m_pClass, true, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
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
		m_pApi->Exit();
		
		m_pApi->Release();
		m_pApi = nullptr;

		// 全清理，只留最后一个
		if (m_msgQueue)
		{
			m_msgQueue->Clear();
			m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
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
		Order_STRUCT order = { 0 };
		OrderField_2_Order_STRUCT(ppOrders[i], &order);

		// 主要是账号定位问题
		CSingleUser* pUser = Fill_UserID_Client(order.KHH, &order.Client);

		RequestResponse_STRUCT* pRequest = m_pApi->MakeOrder(&order);
		pRequest->pUserData_Public = pUser;
		pRequest->pUserData_Public2 = ppOrders[i];
		m_pApi->SendRequest(pRequest);
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
			else
			{
				// 找不到对应的ID，需要处理地，不然会报错
			}
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
	if (m_pApi == nullptr)
		return 0;

	int count = (int)size1 / sizeof(OrderField*);
	// 通过ID找到原始结构，用于撤单
	// 通过ID找到通用结构，用于接收回报
	// 这里传过来的的是已经被复制过的内容
	OrderField** ppOrders = (OrderField**)ptr1;
	WTLB_STRUCT** ppTdxOrders = (WTLB_STRUCT**)ptr2;

	for (int i = 0; i < count; ++i)
	{
		if (ppTdxOrders[i] == nullptr)
			continue;

		CSingleUser* pUser = Fill_UserID_Client(ppTdxOrders[i]->KHH, &ppTdxOrders[i]->Client);

		RequestResponse_STRUCT* pRequest = m_pApi->MakeCancelOrder(ppTdxOrders[i]);
		pRequest->pUserData_Public = pUser;
		pRequest->pUserData_Public2 = ppOrders[i];
		m_pApi->SendRequest(pRequest);
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

	strcpy(query.ZQDM, (char*)ptr1);
	query.requestType = REQUEST_HQ;
	CSingleUser* pUser = Fill_UserID_Client(query.KHH, &query.Client);
	RequestResponse_STRUCT* pRequest = m_pApi->MakeQueryData(&query);
	pRequest->pUserData_Public = pUser;
	m_pApi->SendRequest(pRequest);

	return 0;
}

int CTraderApi::OnResponse_Subscribe(CTdxApi* pApi, RequestResponse_STRUCT* pRespone)
{
	HQ_STRUCT** ppRS = nullptr;
	CharTable2HQ(pRespone->ppFieldInfo, pRespone->ppResults, &ppRS, pRespone->Client, m_msgQueue);

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

		m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnDepthMarketData, m_msgQueue, m_pClass, 0, 0, pField, pField->Size, nullptr, 0, nullptr, 0);
	}

	return 0;
}


CSingleUser* CTraderApi::Fill_UserID_Client(char* khh, void** Client)
{
	CSingleUser* pUser = nullptr;
	if (*Client)
	{
		unordered_map<void*, CSingleUser*>::iterator it = m_Client_User_Map.find(*Client);
		if (it == m_Client_User_Map.end())
		{
			pUser = m_pDefaultUser;
		}
		else
		{
			pUser = it->second;
		}
		strcpy(khh, pUser->m_UserID);
	}
	else
	{
		unordered_map<string, CSingleUser*>::iterator it = m_UserID_User_Map.find(khh);
		if (it == m_UserID_User_Map.end())
		{
			pUser = m_pDefaultUser;
		}
		else
		{
			pUser = it->second;
		}
		*Client = pUser->m_pClient;
	}

	return pUser;
}
