using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace QuantBox.XAPI.COM
{
    /// <summary>
    /// 查询持仓,查委托，查成交
    /// </summary>
    [ComVisible(true)]
    [Guid("6E232C62-E200-4C15-863F-D319B74414C9")]
    public class ReqQueryClass
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public string InstrumentName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ClientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string AccountID;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 4)]
        public string CurrencyID;

        public int DateStart;
        public int DateEnd;
        public int TimeStart;
        public int TimeEnd;


        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Char64ID;
        public int Int32ID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Char64RefID;
        public int Int32RefID;
    }

    ///持仓
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct PositionField
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public byte[] InstrumentName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ClientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string AccountID;

        public PositionSide Side;
        public HedgeFlagType HedgeFlag;
        public int Date;
        public double PositionCost;
        
        public double Position;
        public double TodayPosition;
        public double HistoryPosition;
        public double HistoryFrozen;

        ///今日买卖持仓
        public double TodayBSPosition;
        ///今日买卖持仓冻结
        public double TodayBSFrozen;
        ///今日申赎持仓
        public double TodayPRPosition;
        ///今日申赎持仓冻结
        public double TodayPRFrozen;
    }

    /// <summary>
    /// 做市商双向报价
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct QuoteField
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public byte[] InstrumentName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ClientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string AccountID;


        public double AskQty;
        public double AskPrice;
        public OpenCloseType AskOpenClose;
        public HedgeFlagType AskHedgeFlag;

        public double BidQty;
        public double BidPrice;
        public OpenCloseType BidOpenClose;
        public HedgeFlagType BidHedgeFlag;

        public OrderStatus Status;
        public ExecType ExecType;


        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string QuoteReqID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string LocalID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string AskID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string BidID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string AskOrderID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string BidOrderID;

        public int XErrorID;
        public int RawErrorID;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public byte[] Text;
    }

    /// <summary>
    /// 订单信息
    /// </summary>
    [ComVisible(true)]
    [Guid("23FC4E48-54FF-4A89-B924-B8668C5D9570")]
    public class OrderClass
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public string InstrumentName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ClientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string AccountID;


        public int Side;
        public string Side_String;
        public double Qty;
        public double Price;
        public int OpenClose;
        public string OpenClose_String;
        public int HedgeFlag;
        public string HedgeFlag_String;
        public int Date;
        public int Time;


        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string OrderID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string LocalID;


        public int Type;
        public string Type_String;
        public double StopPx;
        public int TimeInForce;
        public string TimeInForce_String;

        public int Status;
        public string Status_String;
        public int ExecType;
        public string ExecType_String;
        public double LeavesQty;
        public double CumQty;
        public double AvgPx;

        public int XErrorID;
        public int RawErrorID;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public string Text;

        public int ReserveInt32;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ReserveChar64;
    }

    /// <summary>
    /// 成交回报
    /// </summary>
    [ComVisible(true)]
    [Guid("9FF68AA1-CB14-49E5-91F9-856400663C5E")]
    public class TradeClass
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public string InstrumentName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ClientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string AccountID;

        public int Side;
        public string Side_String;
        public double Qty;
        public double Price;
        public int OpenClose;
        public string OpenClose_String;
        public int HedgeFlag;
        public string HedgeFlag_String;
        public int Date;
        public int Time;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string TradeID;

        public double Commission;

        public int ReserveInt32;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ReserveChar64;
    }


    /// <summary>
    /// 服务器信息
    /// </summary>
    [ComVisible(true)]
    [Guid("44C1222D-D53A-40E7-ADE6-8106ED187389")]
    public class ServerInfoClass
    {
        /// <summary>
        /// 订阅主题
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public bool IsUsingUdp;
        /// <summary>
        /// 订阅主题
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public bool IsMulticast;
        /// <summary>
        /// 订阅主题
        /// </summary>
        public int TopicId;
        /// <summary>
        /// 端口号
        /// </summary>
        public int Port;
        /// <summary>
        /// 流恢复
        /// </summary>
        public int MarketDataTopicResumeType;
        public int PrivateTopicResumeType;
        public int PublicTopicResumeType;
        public int UserTopicResumeType;
        /// <summary>
        /// 经纪公司代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 11)]
        public string BrokerID;
        /// <summary>
        /// 用户端产品信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 11)]
        public string UserProductInfo;
        /// <summary>
        /// 认证码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 17)]
        public string AuthCode;
        /// <summary>
        /// 地址
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
        public string Address;
        /// <summary>
        /// 配置文件路径
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string ConfigPath;
        /// <summary>
        /// 扩展信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string ExtInfoChar128;
    }


    /// <summary>
    /// 用户信息
    /// </summary>
    [ComVisible(true)]
    [Guid("64B0070A-B601-45C2-BD7D-CE507AFB924E")]
    public class UserInfoClass
    {
        /// <summary>
        /// 用户代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string UserID;
        /// <summary>
        /// 密码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 41)]
        public string Password;
        /// <summary>
        /// 扩展信息，通达信中用来做通讯密码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ExtInfoChar64;
        /// <summary>
        /// 扩展信息
        /// </summary>
        public int ExtInfoInt32;
    }


    /// <summary>
    /// 错误信息
    /// </summary>
    [ComVisible(true)]
    public class ErrorField
    {
        /// <summary>
        /// 错误代码
        /// </summary>
        public int XErrorID;
        /// <summary>
        /// 错误代码
        /// </summary>
        public int RawErrorID;
        /// <summary>
        /// 错误信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public string Text;
        /// <summary>
        /// 信息来源
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Source;
    }


    /// <summary>
    /// 日志信息
    /// </summary>
    [ComVisible(true)]
    public class LogField
    {
        /// <summary>
        /// 日志级别
        /// </summary>
        public int Level;
        public string Level_String;
        /// <summary>
        /// 错误信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public string Message;
    }


    [ComVisible(true)]
    [Guid("D642BDD2-8BB7-4BC9-BDC4-29CF05D1EAD4")]
    public class RspUserLoginClass
    {
        public int TradingDay;
        public int LoginTime;
        public string SessionID;
        public string UserID;
        public string AccountID;
        public string InvestorName;
        public int XErrorID;
        public int RawErrorID;
        public string Text;
    }

    

    /// <summary>
    /// DepthField行情
    /// </summary>
    [ComVisible(true)]
    public struct DepthClass
    {
        public double Price;
        public int Size;
        public int Count;
    }


    /// <summary>
    /// 合约信息
    /// </summary>
    [ComVisible(true)]
    [Guid("37ED5322-1C11-477A-AC97-F947AD8532B2")]
    public class InstrumentClass
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
        public string InstrumentName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ClientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string AccountID;


        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string ExchangeInstID;


        /// <summary>
        /// 类型
        /// </summary>
        public int Type;
        public string Type_String;
        /// <summary>
        /// 合约数量乘数
        /// </summary>
        public int VolumeMultiple;
        /// <summary>
        /// 最小变动价位
        /// </summary>
        public double PriceTick;
        /// <summary>
        /// 到期日
        /// </summary>
        public int ExpireDate;

        /// <summary>
        /// 执行价
        /// </summary>
        public double StrikePrice;
        /// <summary>
        /// 期权类型
        /// </summary>
        public int OptionsType;
        public string OptionsType_String;
        /// <summary>
        /// 产品代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string ProductID;
        /// <summary>
        /// 基础商品代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string UnderlyingInstrID;
        ///合约生命周期状态
        public int InstLifePhase;
        public string InstLifePhase_String;
    }

    /// <summary>
    /// 账号
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct AccountField
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string ClientID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string AccountID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 4)]
        public string CurrencyID;
        /// <summary>
        /// 上次结算准备金
        /// </summary>
        public double PreBalance;
        /// <summary>
        /// 当前保证金总额
        /// </summary>
        public double CurrMargin;
        /// <summary>
        /// 平仓盈亏
        /// </summary>
        public double CloseProfit;
        /// <summary>
        /// 持仓盈亏
        /// </summary>
        public double PositionProfit;
        /// <summary>
        /// 期货结算准备金
        /// </summary>
        public double Balance;
        /// <summary>
        /// 可用资金
        /// </summary>
        public double Available;
        /// <summary>
        /// 入金金额
        /// </summary>
        public double Deposit;
        /// <summary>
        /// 出金金额
        /// </summary>
        public double Withdraw;
        /// <summary>
        /// 可取资金
        /// </summary>
        public double WithdrawQuota;
        /// <summary>
        /// 冻结的过户费
        /// </summary>
        public double FrozenTransferFee;
        /// <summary>
        /// 冻结的印花税
        /// </summary>
        public double FrozenStampTax;
        /// <summary>
        /// 冻结的手续费
        /// </summary>
        public double FrozenCommission;
        /// <summary>
        /// 冻结的资金
        /// </summary>
        public double FrozenCash;
        /// <summary>
        /// 过户费
        /// </summary>
        public double TransferFee;
        /// <summary>
        /// 印花税
        /// </summary>
        public double StampTax;
        /// <summary>
        /// 手续费
        /// </summary>
        public double Commission;
        /// <summary>
        /// 资金差额
        /// </summary>
        public double CashIn;
    }



    /// <summary>
    /// 发给做市商的询价请求
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct QuoteRequestField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        /// <summary>
        /// 交易日
        /// </summary>
        public int TradingDay;
        /// <summary>
        /// 询价时间
        /// </summary>
        public int QuoteTime;

        /// <summary>
        /// 询价编号
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string QuoteID;
    }

    /// <summary>
    /// 结算信息
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct SettlementInfoField
    {
        public int Size;
        /// <summary>
        /// 交易日
        /// </summary>
        public int TradingDay;
        ///// <summary>
        ///// 消息正文
        ///// </summary>
        //[MarshalAs(UnmanagedType.ByValArray, SizeConst = 501)]
        //public byte[] Content;
    }

    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct SettlementInfoClass
    {
        /// <summary>
        /// 交易日
        /// </summary>
        public int TradingDay;
        ///// <summary>
        ///// 消息正文
        ///// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2)]
        public string Content;
    }

    /// <summary>
    /// 投资者
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct InvestorField
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string InvestorID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 11)]
        public string BrokerID;
        public IdCardType IdentifiedCardType;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 51)]
        public string IdentifiedCardNo;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 81)]
        public byte[] InvestorName;
    }


    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct OrderIDType
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ID;
    }

    /// <summary>
    /// 深度行情N档
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct DepthMarketDataNField
    {
        ///占用总字节大小
        public int Size;

        public int TradingDay;
        public int ActionDay;
        public int UpdateTime;
        public int UpdateMillisec;

        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        public ExchangeType Exchange;


        /// <summary>
        /// 最新价
        /// </summary>
        public double LastPrice;
        /// <summary>
        /// 数量
        /// </summary>
        public double Volume;
        /// <summary>
        /// 成交金额
        /// </summary>
        public double Turnover;
        /// <summary>
        /// 持仓量
        /// </summary>
        public double OpenInterest;
        /// <summary>
        /// 当日均价
        /// </summary>
        public double AveragePrice;


        /// <summary>
        /// 今开盘
        /// </summary>
        public double OpenPrice;
        /// <summary>
        /// 最高价
        /// </summary>
        public double HighestPrice;
        /// <summary>
        /// 最低价
        /// </summary>
        public double LowestPrice;
        /// <summary>
        /// 今收盘
        /// </summary>
        public double ClosePrice;
        /// <summary>
        /// 本次结算价
        /// </summary>
        public double SettlementPrice;

        /// <summary>
        /// 涨停板价
        /// </summary>
        public double UpperLimitPrice;
        /// <summary>
        /// 跌停板价
        /// </summary>
        public double LowerLimitPrice;
        /// <summary>
        /// 昨收盘
        /// </summary>
        public double PreClosePrice;
        /// <summary>
        /// 上次结算价
        /// </summary>
        public double PreSettlementPrice;
        /// <summary>
        /// 昨持仓量
        /// </summary>
        public double PreOpenInterest;

        ///交易阶段类型
        public TradingPhaseType TradingPhase;

        ///买档个数
        public int BidCount;
    }

    [ComVisible(true)]
    [Guid("468A2E0A-A1F3-4B68-8CC4-D1C4DBA8E45B")]
    public class DepthMarketDataNClass
    {
        public int TradingDay;
        public int ActionDay;
        public int UpdateTime;
        public int UpdateMillisec;

        /// <summary>
        /// 交易所代码
        /// </summary>
        public int Exchange;

        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;


        /// <summary>
        /// 最新价
        /// </summary>
        public double LastPrice;
        /// <summary>
        /// 数量
        /// </summary>
        public double Volume;
        /// <summary>
        /// 成交金额
        /// </summary>
        public double Turnover;
        /// <summary>
        /// 持仓量
        /// </summary>
        public double OpenInterest;
        /// <summary>
        /// 当日均价
        /// </summary>
        public double AveragePrice;


        /// <summary>
        /// 今开盘
        /// </summary>
        public double OpenPrice;
        /// <summary>
        /// 最高价
        /// </summary>
        public double HighestPrice;
        /// <summary>
        /// 最低价
        /// </summary>
        public double LowestPrice;
        /// <summary>
        /// 今收盘
        /// </summary>
        public double ClosePrice;
        /// <summary>
        /// 本次结算价
        /// </summary>
        public double SettlementPrice;

        /// <summary>
        /// 涨停板价
        /// </summary>
        public double UpperLimitPrice;
        /// <summary>
        /// 跌停板价
        /// </summary>
        public double LowerLimitPrice;
        /// <summary>
        /// 昨收盘
        /// </summary>
        public double PreClosePrice;
        /// <summary>
        /// 上次结算价
        /// </summary>
        public double PreSettlementPrice;
        /// <summary>
        /// 昨持仓量
        /// </summary>
        public double PreOpenInterest;

        ///交易阶段类型
        public int TradingPhase;
        public string TradingPhase_String;

        ///买档个数
        public DepthField[] Bids;
        public DepthField[] Asks;
    }















    /// <summary>
    /// Tick行情
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct TickField
    {
        public int Date;
        public int Time;
        public int Millisecond;

        public double LastPrice;
        public double Volume;
        public double OpenInterest;
        public double BidPrice1;
        public double AskPrice1;
        public int BidSize1;
        public int AskSize1;
    }

    /// <summary>
    /// Bar行情
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct BarField
    {
        public int Date;
        public int Time;

        public double Open;
        public double High;
        public double Low;
        public double Close;
        public double Volume;
        public double OpenInterest;
        public double Turnover;
    }



    /// <summary>
    /// 合约信息
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct HistoricalDataRequestField
    {
        public int Date1;
        public int Date2;
        public int Time1;
        public int Time2;

        public DataObjetType DataType;
        public BarType BarType;
        public int BarSize;

        public int RequestId;
        public int CurrentDate;
        public int lRequest;

        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
    }
}
