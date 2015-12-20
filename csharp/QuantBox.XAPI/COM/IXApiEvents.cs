using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.COM
{
    [ComVisible(true)]
    [Guid("4D7F9B54-3098-45AE-8EEB-4ED24AC47613")]
    [InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    public interface IXApiEvents
    {
        [DispId(1)]
        void OnConnectionStatus(object sender, int status, string status_String, [In] ref RspUserLoginClass userLogin, int size1);
        //public delegate void DelegateOnRtnError(object sender, [In] ref ErrorField error);
        //public delegate void DelegateOnLog(object sender, [In] ref LogField log);

        [DispId(2)]
        void OnRtnDepthMarketData(object sender, [In]ref DepthMarketDataNClass marketData);
        //public delegate void DelegateOnRtnQuoteRequest(object sender, [In]ref QuoteRequestField quoteRequest);

        [DispId(3)]
        void OnRspQryInstrument(object sender, [In] ref InstrumentClass instrument, int size1, bool bIsLast);
        [DispId(6)]
        void OnRspQryTradingAccount(object sender, [In] ref AccountClass account, int size1, bool bIsLast);
        [DispId(7)]
        void OnRspQryInvestorPosition(object sender, [In] ref PositionClass position, int size1, bool bIsLast);
        //public delegate void DelegateOnRspQrySettlementInfo(object sender, [In] ref SettlementInfoClass settlementInfo, int size1, bool bIsLast);
        //public delegate void DelegateOnRspQryInvestor(object sender, [In] ref InvestorField investor, int size1, bool bIsLast);
        [DispId(8)]
        void OnRtnOrder(object sender, [In] ref OrderClass order);
        [DispId(9)]
        void OnRtnTrade(object sender, [In] ref TradeClass trade);
        //public delegate void DelegateOnRtnQuote(object sender, [In] ref QuoteField quote);
        //public delegate void DelegateOnRspQryOrder(object sender, [In] ref OrderField order, int size1, bool bIsLast);
        //public delegate void DelegateOnRspQryTrade(object sender, [In] ref TradeField trade, int size1, bool bIsLast);
        //public delegate void DelegateOnRspQryQuote(object sender, [In] ref QuoteField quote, int size1, bool bIsLast);

        //public delegate void DelegateOnRspQryHistoricalTicks(object sender, IntPtr pTicks, int size1, [In] ref HistoricalDataRequestField request, int size2, bool bIsLast);
        //public delegate void DelegateOnRspQryHistoricalBars(object sender, IntPtr pBars, int size1, [In] ref HistoricalDataRequestField request, int size2, bool bIsLast);

        //public delegate bool DelegateOnFilterSubscribe(object sender, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, IntPtr pInstrument);
    }
}
