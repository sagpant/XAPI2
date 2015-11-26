using Ideafixxxer.Generics;
using QuantBox.XAPI.Callback;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace QuantBox.XAPI.COM
{
    [ComVisible(true)]
    [Guid("825E3182-8444-4580-8A8C-965485FBF451"), ClassInterface(ClassInterfaceType.None), ComSourceInterfaces(typeof(IXApiEvents))]
    [ProgId("XApiCom")]
    public class XApiCom :IXApi
    {
        public event DelegateOnConnectionStatus OnConnectionStatus;
        public event DelegateOnRtnDepthMarketData OnRtnDepthMarketData;

        private XApi api;

        public XApiCom()
        {
            api = new XApi();

            api.OnConnectionStatus = OnConnectionStatus_callback;
            //base.OnRtnError = OnRtnError_callback;
            //base.OnLog = OnLog_callback;

            api.OnRtnDepthMarketData = OnRtnDepthMarketData_callback;
            //base.OnRtnQuoteRequest = OnRtnQuoteRequest_callback;

            //base.OnRspQryInstrument = OnRspQryInstrument_callback;
            //base.OnRspQryTradingAccount = OnRspQryTradingAccount_callback;
            //base.OnRspQryInvestorPosition = OnRspQryInvestorPosition_callback;
            //base.OnRspQrySettlementInfo = OnRspQrySettlementInfo_callback;

            //base.OnRspQryOrder = OnRspQryOrder_callback;
            //base.OnRspQryTrade = OnRspQryTrade_callback;
            //base.OnRspQryQuote = OnRspQryQuote_callback;

            //base.OnRtnOrder = OnRtnOrder_callback;
            //base.OnRtnTrade = OnRtnTrade_callback;
            //base.OnRtnQuote = OnRtnQuote_callback;

            //base.OnRspQryHistoricalTicks = OnRspQryHistoricalTicks_callback;
            //base.OnRspQryHistoricalBars = OnRspQryHistoricalBars_callback;

            //base.OnRspQryInvestor = OnRspQryInvestor_callback;
        }

        public void SetLibPath(string LibPath)
        {
            api.LibPath = LibPath;
        }

        public void SetServerInfo(ServerInfoClass ServerInfo)
        {
            api.Server.IsUsingUdp = ServerInfo.IsUsingUdp;
            api.Server.IsMulticast = ServerInfo.IsMulticast;
            api.Server.TopicId = ServerInfo.TopicId;
            api.Server.Port = ServerInfo.Port;
            api.Server.MarketDataTopicResumeType = (QuantBox.ResumeType)ServerInfo.MarketDataTopicResumeType;
            api.Server.PrivateTopicResumeType = (QuantBox.ResumeType)ServerInfo.PrivateTopicResumeType;
            api.Server.PublicTopicResumeType = (QuantBox.ResumeType)ServerInfo.PublicTopicResumeType;
            api.Server.UserTopicResumeType = (QuantBox.ResumeType)ServerInfo.UserTopicResumeType;
            api.Server.BrokerID = ServerInfo.BrokerID;
            api.Server.UserProductInfo = ServerInfo.UserProductInfo;
            api.Server.AuthCode = ServerInfo.AuthCode;
            api.Server.Address = ServerInfo.Address;
            api.Server.ConfigPath = ServerInfo.ConfigPath;
            api.Server.ExtInfoChar128 = ServerInfo.ExtInfoChar128;
        }

        public void SetUserInfo(UserInfoClass UserInfo)
        {
            api.User.UserID = UserInfo.UserID;
            api.User.Password = UserInfo.Password;
            api.User.ExtInfoChar64 = UserInfo.ExtInfoChar64;
            api.User.ExtInfoInt32 = UserInfo.ExtInfoInt32;
        }

        public void Connect()
        {
            api.Connect();
        }

        public void Disconnect()
        {
            api.Disconnect();
        }

        public void Subscribe(string szInstrument, string szExchange)
        {
            api.Subscribe(szInstrument, szExchange);
        }

        public void Unsubscribe(string szInstrument, string szExchange)
        {
            api.Unsubscribe(szInstrument, szExchange);
        }

        public void ReqQuery()
        {

        }


        private void OnConnectionStatus_callback(object sender, QuantBox.ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            if (null != OnConnectionStatus)
            {
                RspUserLoginClass cls = new RspUserLoginClass();
                
                if(size1>0)
                {
                    cls.TradingDay = userLogin.TradingDay;
                    cls.LoginTime = userLogin.LoginTime;
                    cls.SessionID = userLogin.SessionID;
                    cls.UserID = userLogin.UserID;
                    cls.AccountID = userLogin.AccountID;
                    cls.InvestorName = userLogin.InvestorName();
                    cls.XErrorID = userLogin.XErrorID;
                    cls.RawErrorID = userLogin.RawErrorID;
                    cls.Text = userLogin.Text();
                }


                OnConnectionStatus(this, (int)status, Enum<QuantBox.ConnectionStatus>.ToString(status), ref cls, size1);
            }
        }

        private void OnRtnDepthMarketData_callback(object sender, ref QuantBox.XAPI.DepthMarketDataNClass marketData)
        {
            if (null != OnRtnDepthMarketData)
            {
                DepthMarketDataNClass cls = new DepthMarketDataNClass();
                cls.TradingDay = marketData.TradingDay;
                cls.ActionDay = marketData.ActionDay;
                cls.UpdateTime = marketData.UpdateTime;
                cls.UpdateMillisec = marketData.UpdateMillisec;
                cls.Exchange = (int)marketData.Exchange;
                cls.Symbol = marketData.Symbol;
                cls.InstrumentID = marketData.InstrumentID;
                cls.LastPrice = marketData.LastPrice;
                cls.Volume = marketData.Volume;
                cls.Turnover = marketData.Turnover;
                cls.OpenInterest = marketData.OpenInterest;
                cls.AveragePrice = marketData.AveragePrice;
                cls.OpenPrice = marketData.OpenPrice;
                cls.HighestPrice = marketData.HighestPrice;
                cls.LowestPrice = marketData.LowestPrice;
                cls.ClosePrice = marketData.ClosePrice;
                cls.SettlementPrice = marketData.SettlementPrice;
                cls.UpperLimitPrice = marketData.UpperLimitPrice;
                cls.LowerLimitPrice = marketData.LowerLimitPrice;
                cls.PreClosePrice = marketData.PreClosePrice;
                cls.PreSettlementPrice = marketData.PreSettlementPrice;
                cls.PreOpenInterest = marketData.PreOpenInterest;
                cls.TradingPhase = (int)marketData.TradingPhase;
                //cls.Bids = marketData.TradingDay;
                //cls.TradingDay = marketData.TradingDay;

                OnRtnDepthMarketData(this, ref cls);
            }
        }
    }
}
