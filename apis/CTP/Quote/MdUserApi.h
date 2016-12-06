#pragma once

#include "../../include/ApiStruct.h"

// 由于这个Include.h需要使用每个项目自己的文件，所以需要在VC++ Directories->Include Directories中添加一个"./"
#include "Include.h"

#include <set>
#include <string>
#include <atomic>
#include <mutex>

using namespace std;

class CMsgQueue;

class CMdUserApi :
	public CThostFtdcMdSpi
{
	enum RequestType
	{
		E_Init,
		E_Disconnect,
		E_ReqUserLoginField,
	};

public:
	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void Register(void* pCallback, void* pClass);
	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo,
		int count);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID);

	void SubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID);
	void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	int _Init();
	void _Disconnect(bool IsInQueue);
	void _DisconnectInThread();

	//登录请求
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchangeID);
	void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchangeID);

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);

	//检查是否出错
	bool IsErrorRspInfo(const char* szSource, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//将出错消息送到消息队列
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//不送出错消息

private:
	//bool						m_delete;
	mutex						m_csMapInstrumentIDs;
	mutex						m_csMapQuoteInstrumentIDs;

	atomic<int>					m_lRequestID;			//请求ID，每次请求前自增

	set<string>					m_setInstrumentIDs;		//正在订阅的合约
	set<string>					m_setQuoteInstrumentIDs;		//正在订阅的合约
	CThostFtdcMdApi*			m_pApi;					//行情API

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

	CMsgQueue*					m_remoteQueue;

	int							m_TradingDay;
};

