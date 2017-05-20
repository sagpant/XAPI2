#!/usr/bin/env python
# -*- coding: utf-8 -*-


# import pydevd
from .XDataType import *
from .XEnum import *

# 定义枚举类型在C结接体中的类型
# _BusinessType = c_char
# _PositionSide = c_char
# _HedgeFlagType = c_char
# _OrderStatus = c_char
# _ExecType = c_char
# _OpenCloseType = c_char
# _OrderSide = c_char
# _OrderType = c_char
# _TimeInForce = c_char
# _ResumeType = c_char
# _LogLevel = c_char
# _InstrumentType = c_char
# _PutCall = c_char
# _IdCardType = c_char
# _ExchangeType = c_char
# _TradingPhaseType = c_char
# _InstLifePhaseType = c_char

_BusinessType = c_byte
_PositionSide = c_byte
_HedgeFlagType = c_byte
_OrderStatus = c_byte
_ExecType = c_byte
_OpenCloseType = c_byte
_OrderSide = c_byte
_OrderType = c_byte
_TimeInForce = c_byte
_ResumeType = c_byte
_LogLevel = c_byte
_InstrumentType = c_byte
_PutCall = c_byte
_IdCardType = c_byte
_ExchangeType = c_byte
_TradingPhaseType = c_byte
_InstLifePhaseType = c_byte

import sys


# 转码工具，输入u，输出根据版本变化
def to_str(s):
    if isinstance(u'0', str):
        # py3
        # 但在PyCharm中的控制台中还是显示乱码
        return s
    else:
        # py2
        # DOS中需要设置成GBK
        # 但PyCharm Debug中需要设置成UTF-8才能看
        # print sys.stdin.encoding
        return s.encode(sys.stdin.encoding)
        # return s.encode('UTF-8')
        # return s.encode('GBK')


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
        ("ID", PositionIDType),

        ("PortfolioID1", IDChar32Type),
        ("PortfolioID2", IDChar32Type),
        ("PortfolioID3", IDChar32Type),
        ("Business", _BusinessType),
    ]

    def get_instrument_id(self):
        return self.InstrumentID.decode('GBK')

    def get_exchange_id(self):
        return self.ExchangeID.decode('GBK')

    def get_instrument_name(self):
        return self.InstrumentName.decode('GBK')

    def get_id(self):
        return self.ID.decode('GBK')

    def __str__(self):
        return to_str(
            u'[InstrumentID={0};ExchangeID={1};HedgeFlag={2};'
            u'Side={3};Position={4};TodayPosition={5};HistoryPosition={6};ID={7}]'
                .format(self.get_instrument_id(), self.get_exchange_id(),
                        HedgeFlagType[self.HedgeFlag],
                        PositionSide[self.Side],
                        self.Position, self.TodayPosition, self.HistoryPosition, self.get_id())
        )

    def __len__(self):
        return sizeof(self)


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

    def get_instrument_id(self):
        return self.InstrumentID.decode('GBK')

    def get_exchange_id(self):
        return self.ExchangeID.decode('GBK')

    def get_local_id(self):
        return self.LocalID.decode('GBK')

    def get_id(self):
        return self.ID.decode('GBK')

    def get_order_id(self):
        return self.OrderID.decode('GBK')

    def get_text(self):
        return self.Text.decode('GBK')

    def __str__(self):
        return to_str(
            u'[InstrumentID={0};ExchangeID={1};Side={2};Qty={3};LeavesQty={4};Price={5};'
            u'OpenClose={6};HedgeFlag={7};LocalID={8};ID={9};OrderID={10};'
            u'Date={11};Time={12};Type={13};TimeInForce={14};Status={15};ExecType={16};'
            u'XErrorID={17};RawErrorID={18};Text={19}]'
                .format(self.get_instrument_id(), self.get_exchange_id(), OrderSide[self.Side],
                        self.Qty, self.LeavesQty, self.Price,
                        OpenCloseType[self.OpenClose],
                        HedgeFlagType[self.HedgeFlag],
                        self.get_local_id(), self.get_id(), self.get_order_id(), self.Date, self.Time,
                        OrderType[self.Type],
                        TimeInForce[self.TimeInForce],
                        OrderStatus[self.Status],
                        ExecType[self.ExecType],
                        self.XErrorID, self.RawErrorID, self.get_text())
        )

    def __len__(self):
        return sizeof(self)


# 这个比较特殊，只是为了传一个结构体字符串
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

    def get_instrument_id(self):
        return self.InstrumentID.decode('GBK')

    def get_instrument_name(self):
        return self.InstrumentName.decode('GBK')

    def get_exchange_id(self):
        return self.ExchangeID.decode('GBK')

    def get_id(self):
        return self.ID.decode('GBK')

    def get_trade_id(self):
        return self.TradeID.decode('GBK')

    def __str__(self):
        return to_str(
            u'[InstrumentID={0};ExchangeID={1};Side={2};Qty={3};Price={4};OpenClose={5};HedgeFlag={6};'
            u'ID={7};TradeID={8};Date={9};Time={10};Commission={11}]'
                .format(self.get_instrument_id(), self.get_exchange_id(), OrderSide[self.Side], self.Qty, self.Price,
                        OpenCloseType[self.OpenClose], HedgeFlagType[self.HedgeFlag], self.get_id(),
                        self.get_trade_id(),
                        self.Date, self.Time, self.Commission)
        )


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

    def get_source(self):
        return self.Source.decode('GBK')

    def __str__(self):
        return to_str(
            u'[XErrorID={0};RawErrorID={1};Text={2};Source={3}]'
                .format(self.XErrorID, self.RawErrorID, self.get_text(), self.get_source())
        )


class LogField(Structure):
    _pack_ = 1
    _fields_ = [
        ("Level", _LogLevel),
        ("Message", Char256Type),
    ]

    def get_message(self):
        return self.Message.decode('GBK')

    def __str__(self):
        return to_str(u'[Level={0};Message={1}]'
                      .format(LogLevel[self.Level], self.get_message())
                      )


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

    def get_session_id(self):
        return self.SessionID.decode('GBK')

    def get_investor_name(self):
        return self.InvestorName.decode('GBK')

    def get_text(self):
        return self.Text.decode('GBK')

    def __str__(self):
        # 没有枚举，全转成b比较合适
        return to_str(
            u'[TradingDay={0};LoginTime={1};SessionID={2};InvestorName={3};XErrorID={4};RawErrorID={5};Text={6}]'
                .format(self.TradingDay, self.LoginTime, self.get_session_id(), self.get_investor_name(),
                        self.XErrorID, self.RawErrorID, self.get_text())
        )


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

        # 唯一符号
        ("Symbol", SymbolType),
        ("InstrumentID", InstrumentIDType),
        ("ExchangeID", ExchangeIDType),
        ("Exchange", _ExchangeType),

        # 最新价
        ("LastPrice", PriceType),
        # 数量
        ("Volume", LargeVolumeType),
        # 成交金额
        ("Turnover", MoneyType),
        # 持仓量
        ("OpenInterest", LargeVolumeType),
        # 当日均价
        ("AveragePrice", PriceType),
        # 今开盘
        ("OpenPrice", PriceType),
        # 最高价
        ("HighestPrice", PriceType),
        # 最低价
        ("LowestPrice", PriceType),
        # 今收盘
        ("ClosePrice", PriceType),
        # 本次结算价
        ("SettlementPrice", PriceType),

        # 涨停板价
        ("UpperLimitPrice", PriceType),
        # 跌停板价
        ("LowerLimitPrice", PriceType),
        # 昨收盘
        ("PreClosePrice", PriceType),
        # 上次结算价
        ("PreSettlementPrice", PriceType),
        # 昨持仓量
        ("PreOpenInterest", LargeVolumeType),

        ("TradingPhase", _TradingPhaseType),
        # 买档个数
        ("BidCount", SizeType)
    ]

    def get_symbol(self):
        return self.Symbol.decode('GBK')

    def get_instrument_id(self):
        return self.InstrumentID.decode('GBK')

    def get_bid_count(self):
        return self.BidCount

    def get_ticks_count(self):
        count = (self.Size - sizeof(DepthMarketDataNField)) / sizeof(DepthField)
        return int(count)

    def get_ask_count(self):
        return int(self.get_ticks_count() - self.BidCount)

    def __str__(self):
        # pydevd.settrace(suspend=True, trace_only_current_thread=True)
        return to_str(
            u'[TradingDay={0};ActionDay={1};UpdateTime={2};UpdateMillisec={3};Symbol={4};BidCount={5};AskCount={6}]'
                .format(self.TradingDay, self.ActionDay, self.UpdateTime, self.UpdateMillisec, self.get_symbol(),
                        self.BidCount, self.get_ask_count())
        )


class DepthField(Structure):
    _pack_ = 1
    _fields_ = [
        ("Price", PriceType),
        ("Size", VolumeType),
        ("Count", VolumeType)
    ]

    def __str__(self):
        return to_str(
            u'[Price={0};Size={1};Count={2}]'
                .format(self.Price, self.Size, self.Count)
        )


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

    def get_symbol(self):
        return self.Symbol.decode('GBK')

    def get_instrument_name(self):
        return self.InstrumentName.decode('GBK')

    def __str__(self):
        return to_str(
            u'[Symbol={0};InstrumentName={1}]'
                .format(self.get_symbol(), self.get_instrument_name())
        )


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

    def get_account_id(self):
        return self.AccountID.decode('GBK')

    def get_currency_id(self):
        return self.CurrencyID.decode('GBK')

    def __str__(self):
        return to_str(
            u'[AccountID={0};CurrencyID={1};Balance={2};Available={3}]'
                .format(self.get_account_id(), self.get_currency_id(), self.Balance, self.Available)
        )


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

    def get_investor_id(self):
        return self.InvestorID.decode('GBK')

    def get_broker_id(self):
        return self.BrokerID.decode('GBK')

    def get_identified_card_no(self):
        return self.IdentifiedCardNo.decode('GBK')

    def get_investor_name(self):
        return self.InvestorName.decode('GBK')

    def __str__(self):
        return to_str(
            u'[BrokerID={0};InvestorID={1};IdentifiedCardType={2},IdentifiedCardNo={3};InvestorName={4}]'
                .format(self.get_broker_id(), self.get_investor_id(), IdCardType[self.IdentifiedCardType],
                        self.get_identified_card_no(), self.get_investor_name())
        )
