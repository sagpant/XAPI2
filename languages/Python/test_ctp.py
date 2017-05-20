#!/usr/bin/env python
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


def read_target_positions(api):
    """
    从固定地方读取目标持仓
    :param api:
    :return:
    """
    y = pd.read_csv(r'd:\y.csv', dtype={'Symbol': str})


def query_positions(api):
    """
    查询当前持仓，最后持仓会回到一个变量中存下
    :param api:
    :return:
    """
    query = ReqQueryField()
    # 查持仓
    api.req_query(QueryType.ReqQryInvestorPosition, query)


def send_order(api):
    # 打新测试，也可以进行普通的股票买卖
    order = (OrderField * 2)()

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

    orderid = (OrderIDTypeField * 2)()
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

    orderid2 = (OrderIDTypeField * 1)()
    orderid2[0].OrderIDType = s

    orderid3 = (OrderIDTypeField * 1)()
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
    # 将两个持仓的合并，然后计算合约清单，对清单中的订阅
    # 行情打印如何处理？
    y = pd.read_csv(r'd:\x.csv', dtype={'Symbol': str})
    x = y['Symbol'].str.encode('gbk')
    for i in range(len(x)):
        api.subscribe(x[i], b'')


def usage(api):
    print(u'1 - 读取目标仓位')
    print(u'2 - 查询实盘仓位')
    print(u'3 - 订阅行情')
    print(u'4 - 计算交易清单,显示进度')
    print(u'5 - 下单')
    print(u'6 - 撤单')
    print(u'q - 退出')


if __name__ == '__main__':
    print(sys.stdin.encoding)
    _menu = {
        1: sub_quote,
        2: query,
        3: send_order,
        4: cancel_order
    }

    td = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\XAPI_CPP_x86.dll')

    # TDX接入示例
    ret = td.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Trade_x86.dll')
    if not ret:
        print(td.get_last_error())
        exit(-1)

    # 本来想输入D:\new_gjzq_v6\ 但由于\会转义后面的'，导致出错
    td.ServerInfo.ExtInfoChar128 = br'D:\new_gjzq_v6' + b'\\'
    td.ServerInfo.Address = br'D:\new_gjzq_v6\Login.lua'
    td.UserInfo.UserID = b'111'
    td.UserInfo.Password = b'123'
    td.UserInfo.ExtInfoChar64 = b''  # 华泰需要输入通迅密码

    # CTP接入示例
    td.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Quote_x86.dll')
    td.ServerInfo.Address = br'tcp://180.168.146.187:10010'
    td.ServerInfo.BrokerID = b'9999'
    td.UserInfo.UserID = b'111'
    td.UserInfo.Password = b'123456'

    print(ord(td.get_api_type()))
    print(td.get_api_name())
    print(td.get_api_version())

    spi = MyXSpi()
    td.register_spi(spi)
    td.connect()

    print(os.getpid())

    # 查询持仓
    # 查询清单
    # 合并清单
    # 订阅行情
    # 计算开平列表
    # 下单
    # 撤单
    # 再下
    # 对比持仓


    # 当初在OQ中的开平仓功能怎么没有做好，无语了

    while True:
        usage(td)
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

        _menu.get(i, usage)(td)

    td.disconnect()
