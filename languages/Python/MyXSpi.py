#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pandas as pd
import shelve
import copy
from xapi import XSpi
from xapi.XStruct import *
from xapi.utils import *


class MyXSpi(XSpi):
    """
    继承XSpi可以实现回调方法的各种功能
    """

    def __init__(self):
        # 持仓
        columns, formats = get_fields_columns_formats(PositionField)
        self.position_dtype = np.dtype({'names': columns, 'formats': formats})
        self.position_dict = {}
        self.position_df = None

        # 报单
        columns, formats = get_fields_columns_formats(OrderField)
        self.order_dtype = np.dtype({'names': columns, 'formats': formats})
        self.order_dict = {}
        self.order_df = None

    # def OnLog(self, pLog):
    #     # 由于TDX接口的日志太多，屏蔽一下，对于其它软件可能需要打开
    #     pass

    def OnConnectionStatus(self, status, pUserLogin, size1):
        super(MyXSpi, self).OnConnectionStatus(status, pUserLogin, size1)
        pass

    def OnRspQryInvestorPosition(self, pPosition, size1, bIsLast):
        # 一定要用copy,不然最后一个会覆盖前面的
        self.position_dict[pPosition.get_id()] = copy.copy(pPosition)
        if not bIsLast:
            return

        self.position_df = ctypes_dict_2_dataframe(self.position_dict, self.position_dtype)
        self.position_df = decode_dataframe(self.position_df)
        pass

    def OnRtnOrder(self, pOrder):
        self.order_dict[pOrder.get_id()] = copy.copy(pOrder)

        print(pOrder)
