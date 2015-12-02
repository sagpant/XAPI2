#pragma once
#include "../include/XApiCpp.h"

#include "../include/QueueHeader.h"

class CXApiImpl :
	public CXApi
{
public:
	CXApiImpl(char* libPath);
	~CXApiImpl();

	virtual ApiType GetApiType();
	virtual char* GetApiVersion();
	virtual char* GetApiName();

	virtual bool Init();
	virtual char* GetLastError();
	virtual void RegisterSpi(CXSpi *pSpi);

	virtual void Connect(char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count);
	virtual void Disconnect();

	virtual void Subscribe(char* szInstrument, char* szExchange);
	virtual void Unsubscribe(char* szInstrument, char* szExchange);

	virtual void ReqQuery(QueryType type, ReqQueryField* query);

	virtual char* SendOrder(OrderField* pOrder, int count, char* pOut);
	virtual char* CancelOrder(OrderIDType* pIn, int count, char* pOut);

	virtual char* SendQuote(QuoteField* pQuote, int count, char* pOut);
	virtual char* CancelQuote(OrderIDType* pIn, int count, char* pOut);
public:
	static  void* __stdcall OnRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
private:
	void* _OnRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void* m_pLib;
	void* m_pFun;
	void* m_pApi;

	CXSpi *m_pSpi;
	char m_LibPath[1024];
};

