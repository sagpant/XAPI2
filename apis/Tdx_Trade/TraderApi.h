#pragma once

#include "../../include/Tdx/TdxApi.h"
#include "../../include/ApiStruct.h"
#include "../../include/IDGenerator.h"
#include "../../include/QueueEnum.h"

#ifdef _WIN64
	#pragma comment(lib, "../../include/Tdx/win64/TdxApi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x64d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x64.lib")
	#endif
#else
	#pragma comment(lib, "../../include/Tdx/win32/TdxApi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x86d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x86.lib")
	#endif
#endif

#include "SingleUser.h"

#include <set>
#include <list>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>



using namespace std;

class CMsgQueue;

/*
多账号支持的问题
1.由于不同账号查询的时间间隔需要不一样，所以这个交给子对象处理
2.撤单时只根据一个撤单的ID，所以要在主对象中维护可撤表
3.股东、股份、资金由于只是回报，并不存储，所以可以写在主对象中，也可以写在子对象中，但有可能以后要维护，还是写在子对象中吧
4.行情查询，建议直接都通过主连接又快又好

*/
class CTraderApi:public CTdxSpi
{
	//请求数据包类型
	enum RequestType
	{
		E_Init = 100,
		E_ReqUserLoginField,
		E_QryInvestorField,
		E_InputOrderField,
		E_InputOrderActionField,
		E_QryOrderField,
		E_QryTradeField,
		E_QryTradingAccountField,
		E_QryInvestorPositionField,

		E_QryDepthMarketDataField,


		E_SettlementInfoConfirmField,
		E_QryInstrumentField,
		E_InputQuoteField,
		E_InputQuoteActionField,
		E_ParkedOrderField,
		E_QryInvestorPositionDetailField,
		E_QryInstrumentCommissionRateField,
		E_QryInstrumentMarginRateField,
		E_QrySettlementInfoField,
		E_QryQuoteField,

		E_ReqQueryData_STRUCT,
	};

public:
	//static CTraderApi * pThis;

	CTraderApi(void);
	virtual ~CTraderApi(void);

	void Register(void* pCallback, void* pClass);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo,
		int count);
	void Disconnect();

	char* ReqOrderInsert(
		OrderField* pOrder,
		int count,
		char* pszLocalIDBuf);

	char* ReqOrderAction(OrderIDType* szId, int count, char* pzsRtn);


	void ReqQuery(QueryType type, ReqQueryField* pQuery);
	void ReqQuery(ReqQueryData_STRUCT* pQuery);

	//void ReqQryOrder();
	//void ReqQryTrade();

	void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);

	void StartQueryThread();
	void RemoveUser(CSingleUser* pUser);

private:
	virtual void OnResponse(CTdxApi* pApi, RequestResponse_STRUCT* pRespone);

	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	virtual void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	friend void* __stdcall Test(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	virtual void TestInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _Init();
	int _ReqQuery(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);


	//登录请求
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _ReqQryInvestor(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	/*int _ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);*/
	
	int _ReqOrderInsert(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqOrderAction(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	/*int _ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);*/
	
	int _Subscribe(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int OnResponse_Subscribe(CTdxApi* pApi, RequestResponse_STRUCT* pRespone);


	//检查是否出错
	//bool IsErrorRspInfo(Error_STRUCT *pRspInfo, int nRequestID, bool bIsLast);//向消息队列输出信息
	bool IsErrorRspInfo(const char* szSource, Error_STRUCT *pRspInfo);//不输出信息
	void OutputQueryTime(time_t t, double db,const char* szSource);

	CSingleUser* Fill_UserID_Client(char* khh, void** Client);

public:
	unordered_map<string, OrderField*>				m_id_platform_order;
	unordered_map<string, WTLB_STRUCT*>				m_id_api_order;

private:
	atomic<int>					m_lRequestID;			//请求ID,得保持自增

	OrderIDType					m_orderInsert_Id;

	mutex						m_csOrderRef;
	int							m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	CTdxApi*					m_pApi;					//交易API


	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;

	int							m_nSleep;




	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;		//发送消息队列指针
	CMsgQueue*					m_msgQueue_Order;		//报单消息队列指针
	CMsgQueue*					m_msgQueue_Test;		//测试用

	UserInfoField*				m_pUserInfos;
	int							m_UserInfo_Pos;
	int							m_UserInfo_Count;
	
	void*						m_pClass;

	CIDGenerator*				m_pIDGenerator;

	unordered_map<void*, CSingleUser*> m_Client_User_Map;
	unordered_map<string, CSingleUser*> m_UserID_User_Map;

	CSingleUser*				m_pDefaultUser;
};