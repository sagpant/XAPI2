using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace QuantBox.XAPI.COM
{
    [ComVisible(true)]
    [Guid("2EFCD079-B32E-497A-9966-71EE65222344")]
    public class QueryType
    {
        public const int ReqQryInstrument = 32;
        public const int ReqQryTradingAccount = 33;
        public const int ReqQryInvestorPosition = 34;

        public const int ReqQryOrder = 35;
        public const int ReqQryTrade = 36;
        public const int ReqQryQuote = 37;

        public const int ReqQryInstrumentCommissionRate = 38;
        public const int ReqQryInstrumentMarginRate = 39;
        public const int ReqQrySettlementInfo = 40;
        public const int ReqQryInvestor = 41;

        public const int ReqQryHistoricalTicks = 42;
        public const int ReqQryHistoricalBars = 43;
    }
}
