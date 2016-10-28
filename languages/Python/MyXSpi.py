#!/usr/bin/python
# -*- coding: utf-8 -*-

from xapi import XSpi


class MyXSpi(XSpi):
    """
    继承XSpi可以实现回调方法的各种功能
    """
    def __init__(self):
        pass

    # def OnLog(self, pLog):
    #     # 由于TDX接口的日志太多，屏蔽一下，对于其它软件可能需要打开
    #     pass

    def OnConnectionStatus(self, status, pUserLogin, size1):
        super(MyXSpi, self).OnConnectionStatus(status, pUserLogin, size1)
        pass
