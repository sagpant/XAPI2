#!/usr/bin/python
# -*- coding: utf-8 -*-

import time
import os
from xapi import *
from MyXSpi import *


def send_order(api):
    # 打新测试，也可以进行普通的股票买卖
    order = (OrderField*2)()

    order[0].InstrumentID = b'732203'
    order[0].Type = chr(OrderType.Limit)
    order[0].Side = chr(OrderSide.Buy)
    order[0].Qty = 6000
    order[0].Price = 16.5

    order[1].InstrumentID = b'300558'
    order[1].Type = chr(OrderType.Limit)
    order[1].Side = chr(OrderSide.Buy)
    order[1].Qty = 6500
    order[1].Price = 17.57

    orderid = (OrderIDTypeField*2)()
    orderid[0].OrderIDType = ''
    orderid[1].OrderIDType = ''

    # 下单
    ret = api.send_order(order[0], order[0], 1)
    print(ret)
    ret = api.send_order(order[1], order[1], 1)
    print(ret)
    return


def cancel_order(api):
    # 输入查询委托后的委托ID
    s = raw_input(u'请输入ID\n')

    orderid2 = (OrderIDTypeField*1)()
    orderid2[0].OrderIDType = s

    orderid3 = (OrderIDTypeField*1)()
    orderid3[0].OrderIDType = '0'

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
    # 查资金
    api.req_query(QueryType.ReqQryTradingAccount, query)


def sub_quote(api):
    # 订阅行情
    api.subscribe('600000', '')
    api.subscribe('000001', '')

def usage(api):
    print(u'1 - 订阅行情')
    print(u'2 - 查询')
    print(u'3 - 下单')
    print(u'4 - 撤单')
    print(u'q - 退出')


if __name__ == '__main__':
    # 解决控制台的中文问题
    import sys
    reload(sys)
    sys.setdefaultencoding('UTF-8')

    _menu = {
        '1': sub_quote,
        '2': query,
        '3': send_order,
        '4': cancel_order
            }

    api = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\XAPI_CPP_x86.dll')

    # TDX接入示例
    api.init(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\Tdx\Tdx_Trade_x86.dll')
    # 本来想输入D:\new_gjzq_v6\ 但由于\会转义后面的'，导致出错
    api.ServerInfo.ExtInfoChar128 = r'D:\new_gjzq_v6' + '\\'
    api.ServerInfo.Address = r'D:\new_gjzq_v6\Login.lua'
    api.UserInfo.UserID = '123'
    api.UserInfo.Password = '456'

    # CTP接入示例
    # api.init(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Quote_x86.dll')
    # api.ServerInfo.Address = r'tcp://yhzx-front1.yhqh.com:41213'
    # api.ServerInfo.BrokerID = 4040
    # api.UserInfo.UserID = '111111'
    # api.UserInfo.Password = '22222'

    print(ord(api.get_api_type()))
    print(api.get_api_name())
    print(api.get_api_version())

    spi = MyXSpi()
    api.register_spi(spi)
    api.connect()

    print os.getpid()

    while True:
        usage(api)
        # 这一句在控制台中的中文还是有问题，所以把中文去了
        x = raw_input('')
        print(x)
        _menu.get(x, usage)(api)

        if x == "q":
            break

    api.disconnect()
