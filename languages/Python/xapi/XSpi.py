#!/usr/bin/env python
# -*- coding: utf-8 -*-

# import pydevd
from .XStruct import *


class XSpi(object):

    def OnConnectionStatus(self, api, status, pUserLogin, size1):
        print(u'OnConnectionStatus={0}'.format(ConnectionStatus[status]))
        if size1 > 0:
            print(pUserLogin)

    def OnRtnError(self, api, pError):
        print(pError)

    def OnRtnInstrumentStatus(self, api, pInstrumentStatus):
        print(pInstrumentStatus)

    def OnLog(self, api, pLog):
        print(pLog)

    def OnRspQryInstrument(self, api, pInstrument, size1, bIsLast):
        if size1 > 0:
            print(pInstrument)

    def OnRspQryTradingAccount(self, api, pAccount, size1, bIsLast):
        if size1 > 0:
            print(pAccount)

    def OnRspQryInvestorPosition(self, api, pPosition, size1, bIsLast):
        if size1 > 0:
            print(pPosition)

    def OnRspQryOrder(self, api, pOrder, size1, bIsLast):
        if size1 > 0:
            print(pOrder)

    def OnRspQryInvestor(self, api, pInvestor, size1, bIsLast):
        if size1 > 0:
            print(pInvestor)

    def OnRtnOrder(self, api, pOrder):
        print(pOrder)

    def OnRtnTrade(self, api, pTrade):
        print(pTrade)

    def OnRtnDepthMarketData(self, api, ptr1, size1):
        # 这是从交易接口拿到的行情
        obj = cast(ptr1, POINTER(DepthMarketDataNField)).contents
        print(obj)

        # 打印N档行情
        for i in range(obj.get_ticks_count()):
            p = ptr1 + sizeof(DepthMarketDataNField) + sizeof(DepthField) * i
            d = cast(p, POINTER(DepthField)).contents
            print(d)

    def OnRspQrySettlementInfo(self, api, ptr1, size1, bIsLast):
        if size1 > 0:
            obj = cast(ptr1, POINTER(SettlementInfoField)).contents
            print(obj)
