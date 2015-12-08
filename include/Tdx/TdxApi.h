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

//////////////////////////////////////////////////////////////////////////
//支持同券商多账号登录、下单、撤单
class TDXAPI_API CTdxApi
{
public:
	// 创建API
	//		TdxPath*:通达信安装目录，需以"\\"结束。
	static CTdxApi* CreateApi(const char* TdxPath);
	virtual void Release() = 0;

public:
	virtual void LoadScript(const char* LuaFileOrString, bool bFileOrString, bool bEncrypted) = 0;

	virtual void Init(const char* TdxPath, Error_STRUCT** ppErr) = 0;
	virtual void Exit() = 0;

	virtual void* Login(const char* szAccount, const char* szPassword, const char* szCode, char*** pppResults, Error_STRUCT** ppErr) = 0;
	virtual void Logout(void* client) = 0;

public:
	// 弹出通达信内部的版本对话框
	virtual void Version() = 0;

	// 下单时必须指定股东代码，所以需要先查询股东列表才能实现下单。下单时会进行授权查询，失败时不会发单出去
	virtual int SendMultiOrders(Order_STRUCT* pOrders, int count, FieldInfo_STRUCT*** pppFieldInfos, char*** pppResults, Error_STRUCT*** pppErrs) = 0;
	virtual int SendMultiOrders(Order_STRUCT** ppOrders, int count, FieldInfo_STRUCT*** pppFieldInfos, char*** pppResults, Error_STRUCT*** pppErrs) = 0;

	virtual int CancelMultiOrders(CancelOrder_STRUCT* pOrders, int count, FieldInfo_STRUCT*** pppFieldInfos, char*** pppResults, Error_STRUCT*** pppErrs) = 0;
	virtual int CancelMultiOrders(CancelOrder_STRUCT** ppOrders, int count, FieldInfo_STRUCT*** pppFieldInfos, char*** pppResults, Error_STRUCT*** pppErrs) = 0;

	virtual int CancelMultiOrders(WTLB_STRUCT** ppOrders, int count, FieldInfo_STRUCT*** pppFieldInfos, char*** pppResults, Error_STRUCT*** pppErrs) = 0;
	// 发送查询请求
	// 有些请求是当日数据，后面的起始和结束日期自动忽略
	// 对于历史数据，需要查询的区别，格式“yyyyMMdd”，客户端上有60天或90天的时间限制，这里没有，但如果数据太多，间隔设成一周或一月
	// 订阅行情时只需要最后的股票代码
	virtual void* ReqQueryData(int requestType, FieldInfo_STRUCT*** pppFieldInfos, char*** pppResults, Error_STRUCT** ppErr, ReqQueryData_STRUCT* query) = 0;

public:
	// 没有指定同账号
	virtual int GetTableHeader(int request, FieldInfo_STRUCT*** pppResults) = 0;

protected:
	CTdxApi();
	~CTdxApi(){};
};

#endif

