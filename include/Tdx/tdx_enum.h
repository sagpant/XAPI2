#pragma once

#ifndef _TDX_ENUM_H_
#define _TDX_ENUM_H_

#include "tdx_request.h"

// 125_帐号类别
// 37 期权
// 40 沪港通

//	603_交易类型
#define JYLX_Creation			0	// ETF申购
#define JYLX_Redemption			1	// ETF赎回
#define MMBZ_ETF_Creation		JYLX_Creation	// ETF申购
#define MMBZ_ETF_Redemption		JYLX_Redemption	// ETF赎回

// 130_买卖标志
// 下单时使用买卖标志来指定报单类型
// 查询时，单子只简单的表示了买卖方向，是否要启用委托类别？
#define MMBZ_Buy_Limit			0	// 限价买,		204下:可转债转股
#define MMBZ_Sell_Limit			1	// 限价卖,		204下:债券回售
#define MMBZ_Cancel				2	// 撤买/撤卖
#define MMBZ_Buy_Cancel			3	// 撤买，没有测的
#define MMBZ_Sell_Cancel		4	// 撤卖，信达证券结果
#define MMBZ_Merge				12	// 基金合并
#define MMBZ_Split				13	// 基金分拆
#define MMBZ_Buy_DBP			15	// 担保品买入
#define MMBZ_Sell_DBP			16	// 担保品卖出
#define MMBZ_Sell_RQ_ZRT		24	// 转融通融券卖出
#define MMBZ_Buy_HQ_ZRT			34	// 转融通买券还券
#define MMBZ_XQ_HQ_ZRT			36	// 转融通现券还券
#define MMBZ_Buy_Market			67	// 市价买
#define MMBZ_Sell_Market		68	// 市价卖
#define MMBZ_Buy_RZ				69	// 融资买入
#define MMBZ_Sell_RQ			70	// 融券卖出
#define MMBZ_Buy_HQ				71	// 买券还券
#define MMBZ_XQ_HQ				73	// 现券还券
#define MMBZ_DBP_HZ				74	// 担保品划转
#define MMBZ_XJ_HK				75	// 现金还款
#define MMBZ_Sell_HK			76	// 卖券还款
#define MMBZ_Creation			79	// 基金申购
#define MMBZ_Redemption			80	// 基金赎回
#define MMBZ_86					86	// 权证行权

// 131_委托类别，这是自己定定义的，用来将字符串转成数字
#define WTLB_Buy				REQUEST_WT*100+ MMBZ_Buy_Limit		// 买入
#define WTLB_Sell				REQUEST_WT*100+ MMBZ_Sell_Limit		// 卖出
#define WTLB_Buy_Cancel			REQUEST_WT*100+ 2		// 撤买
#define WTLB_Sell_Cancel		REQUEST_WT*100+ 3		// 撤卖
#define WTLB_LOFCreation		REQUEST_WT*100+ MMBZ_Creation		// 基金申购
#define WTLB_LOFRedemption		REQUEST_WT*100+ MMBZ_Redemption		// 基金赎回
#define WTLB_ETFCreation		REQUEST_ETF_SGSH*100+ MMBZ_ETF_Creation	// ETF申购
#define WTLB_ETFRedemption		REQUEST_ETF_SGSH*100+ MMBZ_ETF_Redemption	// ETF赎回
#define WTLB_Merge				REQUEST_WT*100+ MMBZ_Merge		// 合并
#define WTLB_Split				REQUEST_WT*100+ MMBZ_Split		// 分拆

#define MMBZ_CB_Conv	 		0	// 204下:可转债转股
#define MMBZ_CB_Red				1	// 204下:债券回售




// 5565_委托类别
#define WTLB_MM	0;	// 买卖
#define WTLB_CD	1;	// 撤单


// 166_委托方式
// 上海只有046,深圳只有012345,所以市价只发4最简单
#define WTFS_Limit				0 // 限价,限价委托,买卖
#define WTFS_Best_Reverse		1 // 对手方最优价格委托,对方最优价格,对手方最优价格委托,
#define WTFS_Best_Forward		2 // 本方最优价格委托,本方最优价格,本方最优价格委托,
#define WTFS_IOC				3 // 即时成交剩余撤销委托,即时成交剩余撤销,即时成交剩余撤销委托,
#define WTFS_Five_IOC			4 // 五档即时成交剩余撤销,五档即成剩撤,最优五档即时成交剩余撤消委托,
#define WTFS_FOK				5 // 全额成交或撤销委托,全额成交或撤销,全额成交或撤销委托,
#define WTFS_Five_Limit			6 // 五档即时成交剩余转限,五档即成转限价,最优五档即时成交剩余转限价委托,

#define WTFS_HRXYZH				0 // 划入信用账户
#define WTFS_HCXYZH				1 // 划出信用账户

#define WTFS_ZQXJP				2 // 增强限价盘
#define WTFS_JJXJP				3 // 竞价限价盘

// 113_操作标志
#define CZBZ_Open				1	// 开仓
#define CZBZ_Close				2	// 平仓
#define CZBZ_3					3	// ?
#define CZBZ_Lock				4	// 锁定
#define CZBZ_Unlock				5	// 解锁

// 281_融资融券标识
#define RZRQBS_NO				0	// 非融资融券
#define RZRQBS_YES				1	// 融资融券

// 147_状态说明
#define ZTSM_NotSent			0	// 0-未申报
#define ZTSM_1					1	// 
#define ZTSM_New				2	// 2-已申报未成交,未成交,已报
#define ZTSM_Illegal			3	// 3-非法委托
#define ZTSM_4					4	//
#define ZTSM_PartiallyFilled	5	// 5-部分成交
#define ZTSM_AllFilled			6	// 6-全部成交,已成,全部成交
#define ZTSM_PartiallyCancelled	7	// 7-部成部撤，部撤
#define ZTSM_AllCancelled		8	// 8-全部撤单,已撤,全部撤单
#define ZTSM_CancelRejected		9	// 9-撤单未成					只会出现撤单记录中
#define ZTSM_WaitingForReport	10	// 10-等待人工申报	

// 已成,部成,废单,已撤,部撤

#endif