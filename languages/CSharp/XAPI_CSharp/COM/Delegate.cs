using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace XAPI.COM
{
    // 中文问题，以及枚举问题，使用新Class来替代Struct
    [ComVisible(false)]
    public delegate void DelegateOnConnectionStatus(object sender, int status, string status_String, [In] ref RspUserLoginClass userLogin, int size1);
    [ComVisible(false)]
    public delegate void DelegateOnRtnError(object sender, [In] ref ErrorClass error);
    [ComVisible(false)]
    public delegate void DelegateOnLog(object sender, [In] ref LogClass log);

    [ComVisible(false)]
    public delegate void DelegateOnRtnDepthMarketData(object sender, [In]ref DepthMarketDataNClass marketData);
    //public delegate void DelegateOnRtnQuoteRequest(object sender, [In]ref QuoteRequestField quoteRequest);

    [ComVisible(false)]
    public delegate void DelegateOnRspQryInstrument(object sender, [In] ref InstrumentClass instrument, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryTradingAccount(object sender, [In] ref AccountClass account, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryInvestorPosition(object sender, [In] ref PositionClass position, int size1, bool bIsLast);
    //public delegate void DelegateOnRspQrySettlementInfo(object sender, [In] ref SettlementInfoClass settlementInfo, int size1, bool bIsLast);
    //public delegate void DelegateOnRspQryInvestor(object sender, [In] ref InvestorField investor, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRtnOrder(object sender, [In] ref OrderClass order);
    [ComVisible(false)]
    public delegate void DelegateOnRtnTrade(object sender, [In] ref TradeClass trade);
    //public delegate void DelegateOnRtnQuote(object sender, [In] ref QuoteField quote);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryOrder(object sender, [In] ref OrderClass order, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryTrade(object sender, [In] ref TradeClass trade, int size1, bool bIsLast);
    //public delegate void DelegateOnRspQryQuote(object sender, [In] ref QuoteField quote, int size1, bool bIsLast);

    //public delegate void DelegateOnRspQryHistoricalTicks(object sender, IntPtr pTicks, int size1, [In] ref HistoricalDataRequestField request, int size2, bool bIsLast);
    //public delegate void DelegateOnRspQryHistoricalBars(object sender, IntPtr pBars, int size1, [In] ref HistoricalDataRequestField request, int size2, bool bIsLast);

    //public delegate bool DelegateOnFilterSubscribe(object sender, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, IntPtr pInstrument);
}
