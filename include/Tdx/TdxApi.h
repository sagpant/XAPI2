#if !defined(_TDX_API_H)
#define _TDX_API_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef TDXAPI_EXPORTS
#define TDXAPI_API __declspec(dllexport)
#else
#define TDXAPI_API __declspec(dllimport)
#endif

#include "tdx_enum.h"
#include "tdx_struct.h"
#include "tdx_function.h"
#include "tdx_request.h"
#include "tdx_field.h"


class CTdxApi;

class CTdxSpi
{
public:
	virtual void OnResponse(CTdxApi* pApi, RequestResponse_STRUCT* pRespone){};
};

//////////////////////////////////////////////////////////////////////////
//支持同券商多账号登录、下单、撤单
class TDXAPI_API CTdxApi
{
public:
	// 创建API
	static CTdxApi* CreateApi();
	virtual void Release() = 0;

public:
	virtual void RegisterSpi(CTdxSpi *pSpi) = 0;

	virtual void LoadScript(const char* LuaFileOrString, bool bFileOrString, bool bEncrypted) = 0;
	//		TdxPath*:通达信安装目录，需以"\\"结束。
	virtual void Init(const char* TdxPath, Error_STRUCT** ppErr) = 0;
	virtual void Exit() = 0;

	virtual void* Login(const char* szAccount, const char* szPassword, const char* szCode, Error_STRUCT** ppErr) = 0;
	virtual void Logout(void* client) = 0;

public:
	// 弹出通达信内部的版本对话框
	virtual void Version() = 0;

	virtual int GetTableHeader(void* client, int request, FieldInfo_STRUCT*** pppResults) = 0;
	virtual bool IsLogined(void* client) = 0;

	virtual int SendRequest(IN RequestResponse_STRUCT* pRequest) = 0;

	virtual RequestResponse_STRUCT* MakeQueryData(ReqQueryData_STRUCT* query) = 0;

	virtual RequestResponse_STRUCT* MakeOrder(Order_STRUCT* pOrder) = 0;
	// 撤单
	virtual RequestResponse_STRUCT* MakeCancelOrder(CancelOrder_STRUCT* pCancelOrder) = 0;
	virtual RequestResponse_STRUCT* MakeCancelOrder(WTLB_STRUCT* pOrder) = 0;
	
protected:
	CTdxApi();
	~CTdxApi(){};
};

#endif

