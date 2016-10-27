#!/usr/bin/python
# -*- coding: utf-8 -*-

from XDataType import *
from XEnum import *
import pydevd

# 定义枚举类型在C结接体中的类型
_BusinessType = c_char
_PositionSide = c_char
_HedgeFlagType = c_char
_OrderStatus = c_char
_ExecType = c_char
_OpenCloseType = c_char
_OrderSide = c_char
_OrderType = c_char
_TimeInForce = c_char
_ResumeType = c_char
_LogLevel = c_char
_InstrumentType = c_char
_PutCall = c_char
_IdCardType = c_char
_ExchangeType = c_char
_TradingPhaseType = c_char
_InstLifePhaseType = c_char


class ReqQueryField(Structure):
    _pack_ = 1
    _fields_ = [
        ("InstrumentName", InstrumentNameType),
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("ClientID", IDChar32Type),
        ("AccountID", IDChar32Type),

        ("CurrencyID", CurrencyIDType),

        ("DateStart", DateIntType),
        ("DateEnd", DateIntType),
        ("TimeStart", TimeIntType),
        ("TimeEnd", TimeIntType),

        ("Char64ID", OrderIDType),
        ("Int32ID", Int32Type),
        ("Char64PositionIndex", OrderIDType),
        ("Int32PositionIndex", Int32Type),

        ("PortfolioID1", IDChar32Type),
        ("PortfolioID2", IDChar32Type),
        ("PortfolioID3", IDChar32Type),
        ("Business", _BusinessType),
    ]


class PositionField(Structure):
    _pack_ = 1
    _fields_ = [
        ("InstrumentName", InstrumentNameType),
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("ClientID", IDChar32Type),
        ("AccountID", IDChar32Type),

        ("Side", _PositionSide),
        ("HedgeFlag", _HedgeFlagType),
        ("Date", DateIntType),
        ("PositionCost", MoneyType),

        ("Position", QtyType),
        ("TodayPosition", QtyType),
        ("HistoryPosition", QtyType),
        ("HistoryFrozen", QtyType),

        ("TodayBSPosition", QtyType),
        ("TodayBSFrozen", QtyType),
        ("TodayPRPosition", QtyType),
        ("TodayPRFrozen", QtyType),

        ("PortfolioID1", IDChar32Type),
        ("PortfolioID2", IDChar32Type),
        ("PortfolioID3", IDChar32Type),
        ("Business", _BusinessType),
    ]

    def get_instrument_name(self):
        return self.InstrumentName.decode('GBK')

    def __str__(self):
        return u'[InstrumentID={0};ExchangeID={1};HedgeFlag={2};' \
               u'Side={3};Position={4};TodayPosition={5};HistoryPosition={6}]'\
            .format(self.InstrumentID, self.ExchangeID,
                    HedgeFlagType[ord(self.HedgeFlag)],
                    PositionSide[ord(self.Side)],
                    self.Position, self.TodayPosition, self.HistoryPosition)


class QuoteField(Structure):
    _pack_ = 1
    _fields_ = [
        ("InstrumentName", InstrumentNameType),
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("ClientID", IDChar32Type),
        ("AccountID", IDChar32Type),

        ("AskQty", QtyType),
        ("AskPrice", PriceType),
        ("AskOpenClose", _OpenCloseType),
        ("AskHedgeFlag", _HedgeFlagType),

        ("BidQty", QtyType),
        ("BidPrice", PriceType),
        ("BidOpenClose", _OpenCloseType),
        ("BidHedgeFlag", _HedgeFlagType),

        ("Status", _OrderStatus),
        ("ExecType", _ExecType),

        ("QuoteReqID", OrderIDType),
        ("LocalID", OrderIDType),
        ("ID", OrderIDType),
        ("AskID", OrderIDType),
        ("BidID", OrderIDType),
        ("AskOrderID", OrderIDType),
        ("BidOrderID", OrderIDType),

        ("XErrorID", Int32Type),
        ("RawErrorID", Int32Type),
        ("Text", Char256Type),
    ]


class OrderField(Structure):
    _pack_ = 1
    _fields_ = [
        ("InstrumentName", InstrumentNameType),
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("ClientID", IDChar32Type),
        ("AccountID", IDChar32Type),

        ("Side", _OrderSide),
        ("Qty", QtyType),
        ("Price", PriceType),
        ("OpenClose", _OpenCloseType),
        ("HedgeFlag", _HedgeFlagType),
        ("Date", DateIntType),
        ("Time", TimeIntType),

        ("ID", OrderIDType),

        ("OrderID", OrderIDType),
        ("LocalID", OrderIDType),

        ("Type", _OrderType),
        ("StopPx", PriceType),
        ("TimeInForce", _TimeInForce),
        ("Status", _OrderStatus),
        ("ExecType", _ExecType),
        ("LeavesQty", QtyType),
        ("CumQty", QtyType),
        ("AvgPx", PriceType),

        ("XErrorID", Int32Type),
        ("RawErrorID", Int32Type),
        ("Text", Char256Type),
        ("ReserveInt32", Int32Type),
        ("ReserveChar64", Char64Type),

        ("PortfolioID1", IDChar32Type),
        ("PortfolioID2", IDChar32Type),
        ("PortfolioID3", IDChar32Type),
        ("Business", _BusinessType),
    ]

    def get_text(self):
        return self.Text.decode('GBK')

    def __str__(self):
        return u'[InstrumentID={0};ExchangeID={1};Side={2};Qty={3};LeavesQty={4};Price={5};' \
               u'OpenClose={6};HedgeFlag={7};LocalID={8};ID={9};OrderID={10};' \
               u'Date={11};Time={12};Type={13};TimeInForce={14};Status={15};ExecType={16};' \
               u'XErrorID={17};RawErrorID={18};Text={19}]'\
            .format(self.InstrumentID, self.ExchangeID, OrderSide[ord(self.Side)],
                    self.Qty, self.LeavesQty, self.Price,
                    OpenCloseType[ord(self.OpenClose)],
                    HedgeFlagType[ord(self.HedgeFlag)],
                    self.LocalID, self.ID, self.OrderID, self.Date, self.Time,
                    OrderType[ord(self.Type)],
                    TimeInForce[ord(self.TimeInForce)],
                    OrderStatus[ord(self.Status)],
                    ExecType[ord(self.ExecType)],
                    self.XErrorID, self.RawErrorID, self.get_text())


class OrderIDTypeField(Structure):
    _pack_ = 1
    _fields_ = [
        ("OrderIDType", OrderIDType),
    ]


class TradeField(Structure):
    _pack_ = 1
    _fields_ = [
        ("InstrumentName", InstrumentNameType),
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("ClientID", IDChar32Type),
        ("AccountID", IDChar32Type),

        ("Side", _OrderSide),
        ("Qty", QtyType),
        ("Price", PriceType),
        ("OpenClose", _OpenCloseType),
        ("HedgeFlag", _HedgeFlagType),
        ("Date", DateIntType),
        ("Time", TimeIntType),

        ("ID", OrderIDType),

        ("TradeID", TradeIDType),
        ("Commission", MoneyType),

        ("ReserveInt32", Int32Type),
        ("ReserveChar64", Char64Type),

        ("PortfolioID1", IDChar32Type),
        ("PortfolioID2", IDChar32Type),
        ("PortfolioID3", IDChar32Type),
        ("Business", _BusinessType),
    ]


class ServerInfoField(Structure):
    _pack_ = 1
    _fields_ = [
        ("IsUsingUdp", BooleanType),
        ("IsMulticast", BooleanType),
        ("TopicId", Int32Type),
        ("Port", Int32Type),

        ("MarketDataTopicResumeType", _ResumeType),
        ("PrivateTopicResumeType", _ResumeType),
        ("PublicTopicResumeType", _ResumeType),
        ("UserTopicResumeType", _ResumeType),

        ("BrokerID", BrokerIDType),
        ("UserProductInfo", ProductInfoType),
        ("AuthCode", AuthCodeType),
        ("Address", AddressType),
        ("ConfigPath", Char256Type),
        ("ExtInfoChar128", Char128Type),
    ]


class UserInfoField(Structure):
    _pack_ = 1
    _fields_ = [
        ("UserID", IDChar32Type),
        ("Password", PasswordType),
        ("ExtInfoChar64", Char64Type),
        ("ExtInfoInt32", Int32Type),
    ]


class ErrorField(Structure):
    _pack_ = 1
    _fields_ = [
        ("XErrorID", Int32Type),
        ("RawErrorID", Int32Type),
        ("Text", Char256Type),
        ("Source", Char64Type),
    ]

    def get_text(self):
        return self.Text.decode('GBK')

    def __str__(self):
        # pydevd.settrace(suspend=True, trace_only_current_thread=True)
        return u'[XErrorID={0};RawErrorID={1};Text={2};Source={3}]'\
            .format(self.XErrorID, self.RawErrorID, self.get_text(), self.Source)


class LogField(Structure):
    _pack_ = 1
    _fields_ = [
        ("Level", _LogLevel),
        ("Message", Char256Type),
    ]

    def get_message(self):
        return self.Message.decode('GBK')

    def __str__(self):
        return u'[Level={0};Message={1}]'\
            .format(LogLevel[ord(self.Level)], self.get_message())


class RspUserLoginField(Structure):
    _pack_ = 1
    _fields_ = [
        ("TradingDay", DateIntType),
        ("LoginTime", TimeIntType),
        ("SessionID", SessionIDType),
        ("UserID", IDChar32Type),
        ("AccountID", IDChar32Type),
        ("InvestorName", PartyNameType),
        ("XErrorID", Int32Type),
        ("RawErrorID", Int32Type),
        ("Text", Char256Type),
        ("Version", Char32Type),
        ("Lang", Char32Type),
    ]

    def get_investor_name(self):
        return self.InvestorName.decode('GBK')

    def get_text(self):
        return self.Text.decode('GBK')

    def __str__(self):
        return u'[TradingDay={0};LoginTime={1};SessionID={2};InvestorName={3};XErrorID={4};RawErrorID={5};Text={6}]'\
            .format(self.TradingDay, self.LoginTime, self.SessionID, self.get_investor_name(),
                    self.XErrorID, self.RawErrorID, self.get_text())


# 深度行情N档
class DepthMarketDataNField(Structure):
    _pack_ = 1
    _fields_ = [
        # 占用总字节大小
        ("Size", SizeType),
        # 交易所时间
        # 交易日，用于给数据接收器划分到同一文件使用，基本没啥别的用处
        ("TradingDay", DateIntType),
        ("ActionDay", DateIntType),
        ("UpdateTime", TimeIntType),
        ("UpdateMillisec", TimeIntType),

        #唯一符号
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("Exchange", _ExchangeType),

        #最新价
        ("LastPrice", PriceType),
        #数量
        ("Volume", LargeVolumeType),
        #成交金额
        ("Turnover", MoneyType),
        #持仓量
        ("OpenInterest", LargeVolumeType),
        #当日均价
        ("AveragePrice", PriceType),
        #今开盘
        ("OpenPrice", PriceType),
        #最高价
        ("HighestPrice", PriceType),
        #最低价
        ("LowestPrice", PriceType),
        #今收盘
        ("ClosePrice", PriceType),
        #本次结算价
        ("SettlementPrice", PriceType),

        #涨停板价
        ("UpperLimitPrice", PriceType),
        #跌停板价
        ("LowerLimitPrice", PriceType),
        #昨收盘
        ("PreClosePrice", PriceType),
        #上次结算价
        ("PreSettlementPrice", PriceType),
        #昨持仓量
        ("PreOpenInterest", LargeVolumeType),

        ("TradingPhase", _TradingPhaseType),
        #买档个数
        ("BidCount", SizeType)
    ]

    def get_bid_count(self):
        return self.BidCount

    def get_ticks_count(self):
        count = (self.Size - sizeof(DepthMarketDataNField))/sizeof(DepthField)
        return count

    def get_ask_count(self):
        return self.get_ticks_count() - self.BidCount

    def __str__(self):
        #pydevd.settrace(suspend=True, trace_only_current_thread=True)
        return u'[TradingDay={0};ActionDay={1};UpdateTime={2};UpdateMillisec={3};Symbol={4};BidCount={5};AskCount={6}]'\
            .format(self.TradingDay, self.ActionDay, self.UpdateTime, self.UpdateMillisec, self.Symbol,
                    self.BidCount, self.get_ask_count())


class DepthField(Structure):
    _pack_ = 1
    _fields_ = [
        ("Price", PriceType),
        ("Size", VolumeType),
        ("Count", VolumeType)
    ]

    def __str__(self):
        return u'[Price={0};Size={1};Count={2}]'\
            .format(self.Price, self.Size, self.Count)

# 合约
class InstrumentField(Structure):
    _pack_ = 1
    _fields_ = [
        ("InstrumentName", InstrumentNameType),
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("ClientID", IDChar32Type),
        ("AccountID", IDChar32Type),
        ("ExchangeInstID", InstrumentIDType),

        # 合约名称
        ("Type", _InstrumentType),
        # 合约数量乘数
        ("VolumeMultiple", VolumeMultipleType),
        # 最小变动价位
        ("PriceTick", PriceType),
        # 到期日
        ("ExpireDate", DateIntType),
        # 执行价
        ("StrikePrice", PriceType),
        # 期权类型
        ("OptionsType", _PutCall),

        # 产品代码
        ("ProductID", InstrumentIDType),
        # 基础商品代码
        ("UnderlyingInstrID", InstrumentIDType),
        ("InstLifePhase", _InstLifePhaseType),
    ]


# 账号
class AccountField(Structure):
    _pack_ = 1
    _fields_ = [
        ("ClientID", IDChar32Type),
        ("AccountID", IDChar32Type),
        ("CurrencyID", CurrencyIDType),

        # 上次结算准备金
        ("PreBalance", MoneyType),
        # 当前保证金总额
        ("CurrMargin", MoneyType),
        # 平仓盈亏
        ("CloseProfit", MoneyType),
        # 持仓盈亏
        ("PositionProfit", MoneyType),
        # 期货结算准备金
        ("Balance", MoneyType),
        # 可用资金
        ("Available", MoneyType),
        # 入金金额
        ("Deposit", MoneyType),
        # 出金金额
        ("Withdraw", MoneyType),
        ("WithdrawQuota", MoneyType),

        # 冻结的过户费
        ("FrozenTransferFee", MoneyType),
        # 冻结的印花税
        ("FrozenStampTax", MoneyType),
        # 冻结的手续费
        ("FrozenCommission", MoneyType),
        # 冻结的资金
        ("FrozenCash", MoneyType),

        # 过户费
        ("TransferFee", MoneyType),
        # 印花税
        ("StampTax", MoneyType),
        # 手续费
        ("Commission", MoneyType),
        # 资金差额
        ("CashIn", MoneyType),
    ]

    def __str__(self):
        return u'[AccountID={0};CurrencyID={1};Balance={2};Available={3}]'\
            .format(self.AccountID, self.CurrencyID, self.Balance, self.Available)


# 发给做市商的询价请求
class QuoteRequestField(Structure):
    _pack_ = 1
    _fields_ = [
        # 唯一符号
        ("Symbol", SymbolType),
        # 合约代码
        ("InstrumentID", InstrumentIDType),
        # 交易所代码
        ("ExchangeID", ExchangeIDType),

        # 交易日
        ("TradingDay", DateIntType),
        # 询价时间
        ("QuoteTime", TimeIntType),
        # 询价编号
        ("QuoteID", OrderIDType),
    ]


# 账号
class SettlementInfoField(Structure):
    _pack_ = 1
    _fields_ = [
        ("Size", SizeType),
        # 交易日
        ("TradingDay", DateIntType),
    ]


# 投资者
class InvestorField(Structure):
    _pack_ = 1
    _fields_ = [
        # 用户代码
        ("InvestorID", IDChar32Type),

        ("BrokerID", BrokerIDType),

        ("IdentifiedCardType", _IdCardType),

        # 证件号码
        ("IdentifiedCardNo", IdentifiedCardNoType),
        # 投资者名称
        ("InvestorName", PartyNameType),
    ]

    def get_investor_name(self):
        return self.InvestorName.decode('GBK')

    def __str__(self):
        return u'[BrokerID={0};InvestorID={1};IdentifiedCardType={2},IdentifiedCardNo={3};InvestorName={4}]'\
            .format(self.BrokerID, self.InvestorID, IdCardType[ord(self.IdentifiedCardType)], self.IdentifiedCardNo, self.get_investor_name())