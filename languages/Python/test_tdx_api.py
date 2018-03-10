#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
调用Tdx接口进行目标持仓调整示例
"""
import getopt

from MyXSpi import MyXSpi
from xapi import *

# 从当前config_tdx类中加载配置文件
# import config_tdx as config
# import importlib
# config = importlib.import_module('config_tdx2')

def init(config):
    # 交易连接
    td = XApi(r'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\XAPI_CPP_x86.dll')
    td.ServerInfo.ExtInfoChar128 = config['td']['ExtInfoChar128']
    td.ServerInfo.Address = config['td']['Address']

    td.UserInfo.UserID = config['td']['UserID']
    td.UserInfo.Password = config['td']['Password']
    td.UserInfo.ExtInfoChar64 = config['td']['ExtInfoChar64']

    ret = td.init(br'C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\Tdx\Tdx_Trade_x86.dll')
    if not ret:
        print(td.get_last_error())
        exit(-1)
    print(ord(td.get_api_type()))
    print(td.get_api_name())
    print(td.get_api_version())

    # 由于行情与交易都是通过交易接口实现，所以返回一样
    return td, td


def main(configs):
    print(sys.stdin.encoding)
    td, md = init(configs)
    spi = MyXSpi(root_dir=configs['root_dir'],
                 portfolios=['portfolio_1.csv', 'portfolio_2.csv', 'portfolio_3.csv'],
                 td=td, md=md, is_stock=True)
    spi.connect()
    # 在这最好等那么一会儿
    spi.wait_logined(30, 1)
    print("登录等待结束")

    # 参数的依次执行，由于等待的问题存在，1-6怎么处理
    opts, args = getopt.getopt(sys.argv[1:], "", ["input="])
    for op, value in opts:
        print("参数", op, value)
        if op == "--input":
            input_str = value
            input_list = input_str.split(';')
            for x in input_list:
                print(x)
                i = int(x)
                spi.input(i)
            # exit(0)

    while True:
        spi.usage()
        x = input('请输入：\r\n')

        # 输入数字时，python3是str，python2是int，需要处理一下
        print(x, type(x))

        if x == "q":
            break

        # 处理输入是回车或别符号的可能性
        try:
            i = int(x)
        except:
            continue

        spi.input(i)

    # 循环外，退出
    spi.disconnect()
