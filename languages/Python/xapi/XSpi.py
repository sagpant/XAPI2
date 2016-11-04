#!/usr/bin/python
# -*- coding: utf-8 -*-

# import pydevd
from .XStruct import *


class XSpi(object):
    def __init__(self):
        pass

    def OnConnectionStatus(self, status, pUserLogin, size1):
        print(u'OnConnectionStatus={0}'.format(ConnectionStatus[status]))
        if size1 > 0:
            print(pUserLogin)

    def OnRtnError(self, pError):
        print(pError)

    def OnLog(self, pLog):
        print(pLog)

    def OnRspQryInvestorPosition(self, pPosition, size1, bIsLast):
        if size1 > 0:
            print(pPosition)

    def OnRspQryOrder(self, pOrder, size1, bIsLast):
        if size1 > 0:
            print(pOrder)

    def OnRspQryInvestor(self, pInvestor, size1, bIsLast):
        if size1 > 0:
            print(pInvestor)

    def OnRspQryTradingAccount(self, pAccount, size1, bIsLast):
        if size1 > 0:
            print(pAccount)

    def OnRtnOrder(self, pOrder):
        print(pOrder)

    def OnRtnDepthMarketData(self, ptr1, size1):
        # 这是从交易接口拿到的行情
        obj = cast(ptr1, POINTER(DepthMarketDataNField)).contents
        print(obj)

        # 打印N档行情
        for i in range(obj.get_ticks_count()):
            p = ptr1 + sizeof(DepthMarketDataNField) + sizeof(DepthField)*i
            d = cast(p, POINTER(DepthField)).contents
            print(d)







