using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace QuantBox.XAPI.Interface
{
    [ComVisible(false)]
    public interface IXApi
    {
        DelegateOnConnectionStatus OnConnectionStatus { get; set; }
        DelegateOnRtnError OnRtnError { get; set; }
        DelegateOnLog OnLog { get; set; }


        void Connect();
        void Disconnect();

        void ReqQuery(QueryType type, ref ReqQueryField query);

        ApiType GetApiType { get;}
        string GetApiName { get;}
        string GetApiVersion { get;}
    }

    [ComVisible(false)]
    public interface IXMarketData
    {
        DelegateOnRtnDepthMarketData OnRtnDepthMarketData{ get; set; }
        void Subscribe(string szInstrument, string szExchange);
        void Unsubscribe(string szInstrument, string szExchange);
    }

    [ComVisible(false)]
    public interface IXInstrument
    {
        DelegateOnRspQryInstrument OnRspQryInstrument { get; set; }
    }

    [ComVisible(false)]
    public interface IXQuery
    {
        DelegateOnRspQryTradingAccount OnRspQryTradingAccount { get; set; }
        DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition { get; set; }
        DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo { get; set; }

        DelegateOnRspQryOrder OnRspQryOrder { get; set; }
        DelegateOnRspQryTrade OnRspQryTrade { get; set; }
        DelegateOnRspQryQuote OnRspQryQuote { get; set; }
    }

    [ComVisible(false)]
    public interface IXTrade
    {
        DelegateOnRtnOrder OnRtnOrder { get; set; }
        DelegateOnRtnTrade OnRtnTrade { get; set; }
        DelegateOnRtnQuote OnRtnQuote { get; set; }

        string SendOrder(ref OrderField[] orders);
        string CancelOrder(string[] szId);
        string SendQuote(ref QuoteField quote);
        string CancelQuote(string szId);
    }

    [ComVisible(false)]
    public interface IXHistoricalData
    {
        DelegateOnRspQryHistoricalTicks OnRspQryHistoricalTicks { get; set; }
        DelegateOnRspQryHistoricalBars OnRspQryHistoricalBars { get; set; }

        //int ReqQryHistoricalTicks(ref HistoricalDataRequestField request);
        //int ReqQryHistoricalBars(ref HistoricalDataRequestField request);
    }
}
