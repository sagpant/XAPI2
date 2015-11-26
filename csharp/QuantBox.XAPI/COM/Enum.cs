using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace QuantBox.XAPI.COM
{
    public class ConnectionStatus
    {
        public const int Uninitialized = 0;
        public const int Initialized = 1;
        public const int Disconnected = 2;
        public const int Connecting = 3;
        public const int Connected = 4;
        public const int Authorizing = 5;
        public const int Authorized = 6;
        public const int Logining = 7;
        public const int Logined = 8;
        public const int Confirming = 9;
        public const int Confirmed = 10;
        public const int Doing = 11;
        public const int Done = 12;
        public const int Unknown = 13;
    }

    [ComVisible(false)]
    [Flags]
    public enum ApiType : byte
    {
        Nono = 0,
        Trade = 1,
        MarketData = 2,
        Level2 = 4,
        QuoteRequest = 8,
        HistoricalData = 16,
        Instrument = 32,
        Query = 64,
    };

    public class ResumeType
    {
        public const int Restart = 0;
        public const int Resume = 1;
        public const int Quick = 2;
        public const int Undefined = 3;
    };

    [ComVisible(false)]
    public enum LogLevel : byte
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,
    };

    [ComVisible(false)]
    public enum PutCall : byte
    {
        Put,
        Call,
    };

    [ComVisible(false)]
    public enum OrderStatus : byte
    {
        NotSent,
        PendingNew,
        New,
        Rejected,
        PartiallyFilled,
        Filled,
        PendingCancel,
        Cancelled,
        Expired,
        PendingReplace,
        Replaced,
    };

    [ComVisible(false)]
    public enum OrderSide : byte
    {
        Buy,
        Sell,
        Creation,	///申购
        Redemption,	///赎回
        Merge,		///合并
        Split,		///拆分
    };

    [ComVisible(false)]
    public enum OrderType : byte
    {
        Market,
        Stop,
        Limit,
        StopLimit,
        MarketOnClose,
        Pegged,
        TrailingStop,
        TrailingStopLimit,
    };

    [ComVisible(false)]
    public enum TimeInForce : byte
    {
        ATC,
        Day,
        GTC,
        IOC,
        OPG,
        OC,
        FOK,
        GTX,
        GTD,
        GFS,
        AUC,
    };

    [ComVisible(false)]
    public enum PositionSide : byte
    {
        Long,
        Short,
    };

    [ComVisible(false)]
    public enum ExecType : byte
    {
        New,
        Stopped,
        Rejected,
        Expired,
        Trade,
        PendingCancel,
        Cancelled,
        CancelReject,
        PendingReplace,
        Replace,
        ReplaceReject,
    };

    [ComVisible(false)]
    public enum OpenCloseType : byte
    {
        Open,
        Close,
        CloseToday,
    };

    [ComVisible(false)]
    public enum HedgeFlagType : byte
    {
        Speculation,
        Arbitrage,
        Hedge,
        Covered,
        MarketMaker,
    };

    [ComVisible(false)]
    public enum InstrumentType : byte
    {
	    Stock,
	    Future,
	    Option,
	    FutureOption,
	    Bond,
	    FX,
	    Index,
	    ETF,
	    MultiLeg,
	    Synthetic,
    };

    [ComVisible(false)]
    public enum IdCardType : byte
    {
        EID,			///组织机构代码
        IDCard,			///中国公民身份证
        Passport,		///护照
        LicenseNo,		///营业执照号
        TaxNo,			///税务登记号/当地纳税ID
        DrivingLicense,	///驾照
        SocialID,		///当地社保ID
        LocalID,		///当地身份证
        OtherCard,		///其他证件
    };

    [ComVisible(false)]
    public enum ExchangeType : byte
    {
        Undefined,	    /// 未定义
        SHFE,			/// 上期所
        DCE,			/// 大商所
        CZCE,			/// 郑商所
        CFFEX,			/// 中金所
        INE,			/// 能源中心
        SSE,			/// 上交所
        SZSE,			/// 深交所
        SGE,			/// 上海黄金交易所
        NEEQ,			/// 全国中小企业股份转让系统,三板
        HKEx,			/// 港交所
    };

    ///合约生命周期状态类型
    [ComVisible(false)]
    public enum InstLifePhaseType : byte
    {
        NotStart,		///未上市
        Started,		///上市
        Pause,		///停牌
        Expired,		///到期

        Issue,		///发行,参考于XSpeed
        FirstList,	///首日上市,参考于XSpeed
        UnList,		///退市,参考于XSpeed
    };


    ///交易阶段类型
    [ComVisible(false)]
    public enum TradingPhaseType : byte
    {
        BeforeTrading,		///开盘前
        NoTrading,			///非交易
        Continuous,		///连续交易
        AuctionOrdering,	///集合竞价报单
        AuctionBalance,	///集合竞价价格平衡
        AuctionMatch,		///集合竞价撮合
        Closed,			///收盘
        Suspension,		///停牌时段,参考于LTS
        Fuse,				///熔断时段,参考于LTS
    };





    /// <summary>
    /// /////////////////////////////////////////////////////////
    /// </summary>
    [ComVisible(false)]
    public enum BarType:byte
    {
        Time = 1,
        Tick,
        Volume,
        Range,
        Session,
    };

    [ComVisible(false)]
    public enum DataObjetType : byte
    {
        DataObject,
        Tick,
        Bid,
        Ask,
        Trade,
        Quote,
        Bar,
        Level2,
        Level2Snapshot,
        Level2Update,
    };

    /// <summary>
    /// 
    /// </summary>
    [ComVisible(false)]
    public enum DepthLevelType : byte
    {
        L0,
        L1,
        L3,
        L5,
        L10,
        FULL,
    };
}
