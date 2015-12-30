#ifndef _QUEUE_ENUM_H_
#define _QUEUE_ENUM_H_

/// 基本请求
enum RequestType :char
{
	RequestType_GetApiTypes = 0,
	RequestType_GetApiVersion,
	RequestType_GetApiName,

	RequestType_Create, // 创建
	RequestType_Release, // 销毁
	RequestType_Register, // 注册接收队列回调
	RequestType_Config,		// 配置参数

	RequestType_Connect, // 开始/连接
	RequestType_Disconnect, // 停止/断开

	RequestType_Clear, // 清理
	RequestType_Process, // 处理

	RequestType_Subscribe,	// 订阅
	RequestType_Unsubscribe, // 取消订阅

	RequestType_SubscribeQuote, // 订阅询价
	RequestType_UnsubscribeQuote, // 取消订阅询价

	RequestType_ReqOrderInsert,
	RequestType_ReqQuoteInsert,
	RequestType_ReqOrderAction,
	RequestType_ReqQuoteAction,
};

///查询
enum QueryType :char
{
	QueryType_ReqQryInstrument = 32,
	QueryType_ReqQryTradingAccount,
	QueryType_ReqQryInvestorPosition,

	QueryType_ReqQryOrder,
	QueryType_ReqQryTrade,
	QueryType_ReqQryQuote,

	QueryType_ReqQryInstrumentCommissionRate,
	QueryType_ReqQryInstrumentMarginRate,
	QueryType_ReqQrySettlementInfo,
	QueryType_ReqQryInvestor,

	QueryType_ReqQryHistoricalTicks,
	QueryType_ReqQryHistoricalBars,
};

///响应
enum ResponeType :char
{
	ResponeType_OnConnectionStatus = 64,
	ResponeType_OnRtnError,
	ResponeType_OnLog,

	ResponeType_OnRtnDepthMarketData,
	ResponeType_OnRspQryInstrument,
	ResponeType_OnRspQryTradingAccount,
	ResponeType_OnRspQryInvestorPosition,
	ResponeType_OnRspQrySettlementInfo,

	ResponeType_OnRspQryOrder,
	ResponeType_OnRspQryTrade,
	ResponeType_OnRspQryQuote,

	ResponeType_OnRtnOrder,
	ResponeType_OnRtnTrade,
	ResponeType_OnRtnQuote,

	ResponeType_OnRtnQuoteRequest,

	ResponeType_OnRspQryHistoricalTicks,
	ResponeType_OnRspQryHistoricalBars,
	ResponeType_OnRspQryInvestor,

	ResponeType_OnFilterSubscribe,
};

// >=100表示API自定义

#endif
