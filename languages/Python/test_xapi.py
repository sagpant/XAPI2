#!/usr/bin/python
# -*- coding: utf-8 -*-

import time
import os
import sys
from xapi import *
from MyXSpi import *

# 为了解决raw_input与input不兼容的问题
# http://stackoverflow.com/questions/21731043/use-of-input-raw-input-in-python-2-and-3
try:
    input = raw_input
except NameError:
    pass



def send_order(api):
    # 打新测试，也可以进行普通的股票买卖
    order = (OrderField*2)()

    order[0].InstrumentID = b'732060'
    order[0].Type = b'%c' % OrderType.Limit
    order[0].Side = b'%c' % OrderSide.Buy
    order[0].Qty = 6000
    order[0].Price = 10.04

    order[1].InstrumentID = b'002819'
    order[1].Type = b'%c' % OrderType.Limit
    order[1].Side = b'%c' % OrderSide.Buy
    order[1].Qty = 7000
    order[1].Price = 4.96

    orderid = (OrderIDTypeField*2)()
    orderid[0].OrderIDType = b''
    orderid[1].OrderIDType = b''

    # 下单
    ret = api.send_order(order[0], order[0], 1)
    print(ret)
    ret = api.send_order(order[1], order[1], 1)
    print(ret)
    return


def cancel_order(api):
    # 输入查询委托后的委托ID
    s = input(u'请输入ID\n')
    if isinstance(u'0', str):
        # py3
        s = s.encode('gbk')


    orderid2 = (OrderIDTypeField*1)()
    orderid2[0].OrderIDType = s

    orderid3 = (OrderIDTypeField*1)()
    orderid3[0].OrderIDType = b'0'

    ret = api.cancel_order(orderid2, orderid3, 1)
    print(ret)


def query(api):
    # 等一等
    query = ReqQueryField()
    # Tdx的查所有委托功能需要设置查询开始位置，其它API看情况
    query.Int32ID = -1

    # 查当日委托
    api.req_query(QueryType.ReqQryOrder, query)
    # 查持仓
    api.req_query(QueryType.ReqQryInvestorPosition, query)
    # 查资金，会当，需要修复
    api.req_query(QueryType.ReqQryTradingAccount, query)


def sub_quote(api):
    # 订阅行情
    api.subscribe(b'600000', b'')
    api.subscribe(b'000001', b'')


def usage(api):
    print(u'1 - 订阅行情')
    print(u'2 - 查询')
    print(u'3 - 下单')
    print(u'4 - 撤单')
    print(u'q - 退出')


if __name__ == '__main__':
    print(sys.stdin.encoding)
    _menu = {
        1: sub_quote,
        2: query,
        3: send_order,
        4: cancel_order
            }

    api = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\XAPI_CPP_x86.dll')

    # TDX接入示例
    ret = api.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\Tdx\Tdx_Trade_x86.dll')
    if not ret:
        print(api.get_last_error())
        exit(-1)

    # 本来想输入D:\new_gjzq_v6\ 但由于\会转义后面的'，导致出错
    api.ServerInfo.ExtInfoChar128 = br'D:\new_gjzq_v6' + b'\\'
    api.ServerInfo.Address = br'D:\new_gjzq_v6\Login.lua'
    api.UserInfo.UserID = b'18931244679'
    api.UserInfo.Password = b'123'
    api.UserInfo.ExtInfoChar64 = b''  # 华泰需要输入通迅密码

    # CTP接入示例
    # api.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Quote_x86.dll')
    # api.ServerInfo.Address = br'tcp://yhzx-front1.yhqh.com:41213'
    # api.ServerInfo.BrokerID = 4040
    # api.UserInfo.UserID = b'111111'
    # api.UserInfo.Password = b'22222'

    print(ord(api.get_api_type()))
    print(api.get_api_name())
    print(api.get_api_version())

    spi = MyXSpi()
    api.register_spi(spi)
    api.connect()

    print(os.getpid())

    while True:
        usage(api)
        # 这一句在控制台中的中文还是有问题，所以把中文去了
        x = input('')
        # 输入输字时，python3是str，python2是int，需要处理一下
        print(x)
        print(type(x))

        if x == "q":
            break

        # 处理输入是回车或别符号的可能性
        try:
            i = int(x)
        except:
            continue

        _menu.get(i, usage)(api)

    api.disconnect()
