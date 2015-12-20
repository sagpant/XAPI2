using Ideafixxxer.Generics;
using QuantBox.XAPI.Callback;
using System;
using System.Collections.Generic;
using System.EnterpriseServices;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace QuantBox.XAPI.COM
{
    [ComVisible(true)]
    [Guid("825E3182-8444-4580-8A8C-965485FBF451")]
    [ClassInterface(ClassInterfaceType.None)]
    [ComSourceInterfaces(typeof(IXApiEvents))]
    [ProgId("QuantBox.XApiCom")]
    //[EventTrackingEnabled(true)]
    [Description("Interface Serviced Component")]
    public class XApiCom : /*ServicedComponent,*/ IXApi,
        IObjectSafety // implement IObjectSafety to supress the unsafe for scripting 
                       // warning message
    {
        #region Constants
        // Constants for implementation of the IObjectSafety interface.
        private const int INTERFACESAFE_FOR_UNTRUSTED_CALLER = 0x00000001;
        private const int INTERFACESAFE_FOR_UNTRUSTED_DATA = 0x00000002;
        private const int S_OK = 0;
        #endregion

        #region IObjectSafety Methods
        public int GetInterfaceSafetyOptions(ref Guid riid, out int pdwSupportedOptions, out int pdwEnabledOptions)
        {
            pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
            pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
            return S_OK;   // return S_OK
        }

        public int SetInterfaceSafetyOptions(ref Guid riid, int dwOptionSetMask, int dwEnabledOptions)
        {
            return S_OK;   // return S_OK
        }
        #endregion

        public event DelegateOnConnectionStatus OnConnectionStatus;
        public event DelegateOnRtnDepthMarketData OnRtnDepthMarketData;
        public event DelegateOnRspQryInstrument OnRspQryInstrument;
        public event DelegateOnRspQryTradingAccount OnRspQryTradingAccount;
        public event DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition;
        public event DelegateOnRtnOrder OnRtnOrder;
        public event DelegateOnRtnTrade OnRtnTrade;
        public event DelegateOnStatus OnStatus;
        public event DelegateOnTest OnTest;

        private XApi api;
        private ServerInfoField _Server;
        private UserInfoField _User;
        private OrderField _Order;
        private ReqQueryField _Query;

        public XApiCom()
        {
            api = new XApi();

            api.OnConnectionStatus = OnConnectionStatus_callback;
            //base.OnRtnError = OnRtnError_callback;
            //base.OnLog = OnLog_callback;

            api.OnRtnDepthMarketData = OnRtnDepthMarketData_callback;
            //base.OnRtnQuoteRequest = OnRtnQuoteRequest_callback;

            api.OnRspQryInstrument = OnRspQryInstrument_callback;
            api.OnRspQryTradingAccount = OnRspQryTradingAccount_callback;
            api.OnRspQryInvestorPosition = OnRspQryInvestorPosition_callback;
            //base.OnRspQrySettlementInfo = OnRspQrySettlementInfo_callback;

            //base.OnRspQryOrder = OnRspQryOrder_callback;
            //base.OnRspQryTrade = OnRspQryTrade_callback;
            //base.OnRspQryQuote = OnRspQryQuote_callback;

            api.OnRtnOrder = OnRtnOrder_callback;
            api.OnRtnTrade = OnRtnTrade_callback;
            //base.OnRtnQuote = OnRtnQuote_callback;

            //base.OnRspQryHistoricalTicks = OnRspQryHistoricalTicks_callback;
            //base.OnRspQryHistoricalBars = OnRspQryHistoricalBars_callback;

            //base.OnRspQryInvestor = OnRspQryInvestor_callback;
        }

        public void SetLibPath(string LibPath)
        {
            api.LibPath = LibPath;
        }

        public void SetServerInfo(string key, object value)
        {
            Type type = typeof(ServerInfoField);
            FieldInfo field = type.GetField(key,BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key +" is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValueForValueType(ref _Server, obj);
            }
            catch(Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public void SetUserInfo(string key, object value)
        {
            Type type = typeof(UserInfoField);
            FieldInfo field = type.GetField(key, BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key + " is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValueForValueType(ref _User, obj);
            }
            catch (Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public void Connect()
        {
            api.Server = _Server;
            api.User = _User;
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

        public void NewOrder()
        {
            _Order = default(OrderField);
        }
        public void SetOrder(string key, object value)
        {
            Type type = typeof(OrderField);
            FieldInfo field = type.GetField(key, BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key + " is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValueForValueType(ref _Order, obj);
            }
            catch (Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public string SendOrder()
        {
            OrderField[] fields = new OrderField[1];
            fields[0] = _Order;
            return api.SendOrder(ref fields);
        }
        public string CancelOrder(string id)
        {
            string[] ids = new string[1];
            ids[0] = id;
            return api.CancelOrder(ids);
        }

        public void NewQuery()
        {
            _Query = default(ReqQueryField);
        }
        public void SetQuery(string key, object value)
        {
            Type type = typeof(ReqQueryField);
            FieldInfo field = type.GetField(key, BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key + " is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValueForValueType(ref _Query, obj);
            }
            catch (Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public void ReqQuery(XAPI.QueryType type)
        {
            api.ReqQuery(type, ref _Query);
        }

        private void OnConnectionStatus_callback(object sender, QuantBox.ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            if (null == OnConnectionStatus)
                return;

            RspUserLoginClass cls = null;

            if (size1 > 0)
            {
                cls = new RspUserLoginClass();
                RspUserLoginField field = userLogin;

                cls.TradingDay = field.TradingDay;
                cls.LoginTime = field.LoginTime;
                cls.SessionID = field.SessionID;
                cls.UserID = field.UserID;
                cls.AccountID = field.AccountID;
                cls.InvestorName = field.InvestorName();
                cls.XErrorID = field.XErrorID;
                cls.RawErrorID = field.RawErrorID;
                cls.Text = field.Text();
            }

            OnConnectionStatus(this, (int)status, Enum<QuantBox.ConnectionStatus>.ToString(status), ref cls, size1);
        }

        private void OnRtnDepthMarketData_callback(object sender, ref QuantBox.XAPI.DepthMarketDataNClass marketData)
        {
            if (null == OnRtnDepthMarketData)
                return;

            DepthMarketDataNClass cls = new DepthMarketDataNClass();
            QuantBox.XAPI.DepthMarketDataNClass field = marketData;

            cls.TradingDay = field.TradingDay;
            cls.ActionDay = field.ActionDay;
            cls.UpdateTime = field.UpdateTime;
            cls.UpdateMillisec = field.UpdateMillisec;
            cls.Exchange = (int)field.Exchange;
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.LastPrice = field.LastPrice;
            cls.Volume = field.Volume;
            cls.Turnover = field.Turnover;
            cls.OpenInterest = field.OpenInterest;
            cls.AveragePrice = field.AveragePrice;
            cls.OpenPrice = field.OpenPrice;
            cls.HighestPrice = field.HighestPrice;
            cls.LowestPrice = field.LowestPrice;
            cls.ClosePrice = field.ClosePrice;
            cls.SettlementPrice = field.SettlementPrice;
            cls.UpperLimitPrice = field.UpperLimitPrice;
            cls.LowerLimitPrice = field.LowerLimitPrice;
            cls.PreClosePrice = field.PreClosePrice;
            cls.PreSettlementPrice = field.PreSettlementPrice;
            cls.PreOpenInterest = field.PreOpenInterest;
            cls.TradingPhase = (int)field.TradingPhase;
            cls.TradingPhase_String = Enum<QuantBox.TradingPhaseType>.ToString(field.TradingPhase);
            //cls.Bids = marketData.TradingDay;
            //cls.TradingDay = marketData.TradingDay;

            OnRtnDepthMarketData(this, ref cls);
        }

        private void OnRtnOrder_callback(object sender, ref OrderField order)
        {
            if (null == OnRtnOrder)
                return;

            OrderField field = order;

            OrderClass cls = new OrderClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;
            cls.Side = (int)field.Side;
            cls.Side_String = Enum<QuantBox.OrderSide>.ToString(field.Side);
            cls.Qty = field.Qty;
            cls.Price = field.Price;
            cls.OpenClose = (int)field.OpenClose;
            cls.OpenClose_String = Enum<QuantBox.OpenCloseType>.ToString(field.OpenClose);
            cls.HedgeFlag = (int)field.HedgeFlag;
            cls.HedgeFlag_String = Enum<QuantBox.HedgeFlagType>.ToString(field.HedgeFlag);
            cls.Date = field.Date;
            cls.Time = field.Time;
            cls.ID = field.ID;
            cls.OrderID = field.OrderID;
            cls.LocalID = field.LocalID;
            cls.Type = (int)field.Type;
            cls.Type_String = Enum<QuantBox.OrderType>.ToString(field.Type);
            cls.StopPx = field.StopPx;
            cls.TimeInForce = (int)field.TimeInForce;
            cls.TimeInForce_String = Enum<QuantBox.TimeInForce>.ToString(field.TimeInForce);
            cls.Status = (int)field.Status;
            cls.Status_String = Enum<QuantBox.OrderStatus>.ToString(field.Status);
            cls.ExecType = (int)field.ExecType;
            cls.ExecType_String = Enum<QuantBox.ExecType>.ToString(field.ExecType);
            cls.LeavesQty = field.LeavesQty;
            cls.CumQty = field.CumQty;
            cls.AvgPx = field.AvgPx;
            cls.XErrorID = field.XErrorID;
            cls.RawErrorID = field.RawErrorID;
            cls.Text = field.Text();
            cls.ReserveInt32 = field.ReserveInt32;
            cls.ReserveChar64 = field.ReserveChar64;

            OnRtnOrder(this, ref cls);
        }

        private void OnRtnTrade_callback(object sender, ref TradeField trade)
        {
            if (null == OnRtnTrade)
                return;

            TradeField field = trade;

            TradeClass cls = new TradeClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;
            cls.Side = (int)field.Side;
            cls.Side_String = Enum<QuantBox.OrderSide>.ToString(field.Side);
            cls.Qty = field.Qty;
            cls.Price = field.Price;
            cls.OpenClose = (int)field.OpenClose;
            cls.OpenClose_String = Enum<QuantBox.OpenCloseType>.ToString(field.OpenClose);
            cls.HedgeFlag = (int)field.HedgeFlag;
            cls.HedgeFlag_String = Enum<QuantBox.HedgeFlagType>.ToString(field.HedgeFlag);
            cls.Date = field.Date;
            cls.Time = field.Time;
            cls.ID = field.ID;
            cls.TradeID = field.TradeID;
            cls.ReserveInt32 = field.ReserveInt32;
            cls.ReserveChar64 = field.ReserveChar64;

            OnRtnTrade(this, ref cls);
        }

        private void OnRspQryInstrument_callback(object sender, ref InstrumentField instrument, int size1, bool bIsLast)
        {
            if (null == OnRspQryInstrument)
                return;

            InstrumentField field = instrument;

            InstrumentClass cls = new InstrumentClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;

            cls.ExchangeInstID = field.ExchangeInstID;

            cls.Type = (int)field.Type;
            cls.Type_String = Enum<QuantBox.InstrumentType>.ToString(field.Type);

            cls.VolumeMultiple = field.VolumeMultiple;
            cls.PriceTick = field.PriceTick;
            cls.ExpireDate = field.ExpireDate;
            cls.StrikePrice = field.StrikePrice;

            cls.OptionsType = (int)field.OptionsType;
            cls.OptionsType_String = Enum<QuantBox.PutCall>.ToString(field.OptionsType);

            cls.ProductID = field.ProductID;
            cls.UnderlyingInstrID = field.UnderlyingInstrID;

            cls.InstLifePhase = (int)field.InstLifePhase;
            cls.InstLifePhase_String = Enum<QuantBox.InstLifePhaseType>.ToString(field.InstLifePhase);

            OnRspQryInstrument(this, ref cls, size1, bIsLast);
        }

        private void OnRspQryInvestorPosition_callback(object sender, ref PositionField position, int size1, bool bIsLast)
        {
            if (null == OnRspQryInvestorPosition)
                return;

            PositionField field = position;

            PositionClass cls = new PositionClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;

            cls.Side = (int)field.Side;
            cls.Side_String = Enum<QuantBox.PositionSide>.ToString(field.Side);
            cls.HedgeFlag = (int)field.HedgeFlag;
            cls.HedgeFlag_String = Enum<QuantBox.HedgeFlagType>.ToString(field.HedgeFlag);

            cls.Date = field.Date;
            cls.PositionCost = field.PositionCost;
            cls.Position = field.Position;
            cls.TodayPosition = field.TodayPosition;
            cls.HistoryPosition = field.HistoryPosition;
            cls.HistoryFrozen = field.HistoryFrozen;
            cls.TodayBSPosition = field.TodayBSPosition;
            cls.TodayBSFrozen = field.TodayBSFrozen;
            cls.TodayPRPosition = field.TodayPRPosition;
            cls.TodayPRFrozen = field.TodayPRFrozen;

            OnRspQryInvestorPosition(this, ref cls, size1, bIsLast);
        }

        private void OnRspQryTradingAccount_callback(object sender, ref AccountField account, int size1, bool bIsLast)
        {
            if (null == OnRspQryTradingAccount)
                return;

            AccountField field = account;

            AccountClass cls = new AccountClass();

            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;
            cls.CurrencyID = field.CurrencyID;

            cls.PreBalance = field.PreBalance;
            cls.CurrMargin = field.CurrMargin;
            cls.CloseProfit = field.CloseProfit;
            cls.PositionProfit = field.PositionProfit;
            cls.Balance = field.Balance;
            cls.Available = field.Available;
            cls.Deposit = field.Deposit;
            cls.Withdraw = field.Withdraw;
            cls.WithdrawQuota = field.WithdrawQuota;
            cls.FrozenTransferFee = field.FrozenTransferFee;
            cls.FrozenStampTax = field.FrozenStampTax;
            cls.FrozenCommission = field.FrozenCommission;
            cls.FrozenCash = field.FrozenCash;
            cls.StampTax = field.StampTax;
            cls.Commission = field.Commission;
            cls.CashIn = field.CashIn;

            OnRspQryTradingAccount(this, ref cls, size1, bIsLast);
        }
    }
}
