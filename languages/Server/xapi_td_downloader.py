#!/bin/env python
import sys

sys.path.append(r'../Python')

from td_spi import *

if __name__ == '__main__':
    # 准备
    spi = td_spi()
    td = config_td()
    spi.set_api(td)
    init_td(td)
    td.register_spi(spi)
    if not td.is_connected():
        td.connect()

    input("查持仓")
    query = ReqQueryField()
    td.req_query(QueryType.ReqQryInvestorPosition, query)

    input("查委托")
    query = ReqQueryField()
    td.req_query(QueryType.ReqQryOrder, query)

    input("查成交")
    query = ReqQueryField()
    td.req_query(QueryType.ReqQryTrade, query)

    input("查结算单")
    print("当前交易日", spi.TradingDay)
    date = input('请输入结算日:')
    query = ReqQueryField()
    query.DateStart = int(date)  # 20200110
    td.req_query(QueryType.ReqQrySettlementInfo, query)

    input("结束")
