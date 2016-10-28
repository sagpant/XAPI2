#!/usr/bin/python
# -*- coding: utf-8 -*-

from .XQueueEnum import *
from .XSpi import *


# function
fnOnRespone = WINFUNCTYPE(c_void_p, c_char, c_void_p, c_void_p, c_double, c_double, c_void_p, c_int, c_void_p, c_int,
                          c_void_p, c_int)
# fnOnRespone = CFUNCTYPE(c_void_p, c_char, c_void_p, c_void_p, c_double, c_double, c_void_p, c_int, c_void_p, c_int,
#                           c_void_p, c_int)


class XApi(object):
    _cdll_api = None
    _lib = None
    _fun = None
    _api = None
    _response = None
    _spi = None

    ServerInfo = ServerInfoField()
    UserInfo = UserInfoField()

    _response_type_dict = None

    def __init__(self, xapi_path):
        # 加载XAPI_CPP_x86.dll，使用它来加载其它的API的C封装
        self._cdll_api = CDLL(xapi_path)
        # 记录回调函数
        self._response = fnOnRespone(self._on_response)
        # 记录回调dispatcher,将一个回调函数的请求分解为多个函数
        self._response_type_dict = {
            ResponseType.OnConnectionStatus: self._OnConnectionStatus,
            ResponseType.OnRtnError: self._OnRtnError,
            ResponseType.OnLog: self._OnLog,
            ResponseType.OnRspQryOrder: self._OnRspQryOrder,
            ResponseType.OnRspQryInvestorPosition: self._OnRspQryInvestorPosition,
            ResponseType.OnRspQryInvestor: self._OnRspQryInvestor,
            ResponseType.OnRtnOrder: self._OnRtnOrder,
            ResponseType.OnRtnDepthMarketData: self._OnRtnDepthMarketData,
            ResponseType.OnRspQryTradingAccount: self._OnRspQryTradingAccount,
        }

    def init(self, lib_path):
        func = self._cdll_api.X_LoadLib
        func.restype = c_void_p
        func.argtypes = [c_char_p]
        self._lib = func(lib_path)
        if self._lib == None:
            return False
        func = self._cdll_api.X_GetFunction
        func.restype = c_void_p
        func.argtypes = [c_void_p, c_char_p]
        self._fun = func(self._lib, b"XRequest")
        return True

    def get_last_error(self):
        func = self._cdll_api.X_GetLastError
        func.restype = c_char_p
        func.argtypes = []
        ptr = func()
        return ptr.decode('GBK')

    def get_api_type(self):
        func = self._cdll_api.X_GetApiTypes
        func.restype = c_char
        func.argtypes = [c_void_p]
        ret = func(self._fun)
        return ret

    def get_api_version(self):
        func = self._cdll_api.X_GetApiVersion
        func.restype = c_char_p
        func.argtypes = [c_void_p]
        ptr = func(self._fun)
        return ptr.decode('GBK')

    def get_api_name(self):
        func = self._cdll_api.X_GetApiName
        func.restype = c_char_p
        func.argtypes = [c_void_p]
        ptr = func(self._fun)
        return ptr.decode('GBK')

    def register_spi(self, spi):
        self._spi = spi

    def connect(self):
        func = self._cdll_api.X_Create
        func.restype = c_void_p
        func.argtypes = [c_void_p]
        self._api = func(self._fun)

        func = self._cdll_api.X_Register
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_void_p, c_void_p]
        func(self._fun, self._api, self._response, id(self))

        func = self._cdll_api.X_Connect
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_char_p, POINTER(ServerInfoField), POINTER(UserInfoField), c_int]
        func(self._fun, self._api, b'', byref(self.ServerInfo), byref(self.UserInfo), 1)

    def disconnect(self):
        func = self._cdll_api.X_Disconnect
        func.restype = None
        func.argtypes = [c_void_p, c_void_p]
        func(self._fun, self._api)

        func = self._cdll_api.X_FreeLib
        func.restype = None
        func.argtypes = [c_void_p]
        func(self._lib)

        self._lib = None
        self._fun = None
        self._api = None

    def req_query(self, query_type, query):
        func = self._cdll_api.X_ReqQuery
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_char, POINTER(ReqQueryField)]
        # c_char(query_type) python3中可用
        # c_char(chr(query_type)) python2中可用
        # query_type是int
        func(self._fun, self._api, c_char(b'%c' % query_type), byref(query))

    def send_order(self, order, in_out_orderid, count):
        func = self._cdll_api.X_SendOrder
        func.restype = c_char_p
        func.argtypes = [c_void_p, c_void_p, POINTER(OrderField), c_int, POINTER(OrderIDTypeField)]
        p1 = POINTER(OrderField)()
        p1.contents = order
        p2 = POINTER(OrderIDTypeField)()
        p2.contents = in_out_orderid
        return func(self._fun, self._api, p1, count, p2)

    def cancel_order(self, p_in, p_out, count):
        func = self._cdll_api.X_CancelOrder
        func.restype = c_char_p
        func.argtypes = [c_void_p, c_void_p, POINTER(OrderIDTypeField), c_int, POINTER(OrderIDTypeField)]
        p1 = POINTER(OrderIDTypeField)()
        p1.contents = p_in
        p2 = POINTER(OrderIDTypeField)()
        p2.contents = p_out
        return func(self._fun, self._api, p1, count, p2)

    def subscribe(self, instruments, exchange):
        func = self._cdll_api.X_Subscribe
        func.restype = None
        func.argtypes = [c_void_p, c_void_p, c_char_p, c_char_p]
        func(self._fun, self._api, c_char_p(instruments), c_char_p(exchange))

    def _on_response(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        # 解决在回调函数中断点不生效的问题，把这行代码放在要断的地方的前面
        # http://stackoverflow.com/questions/34299082/not-working-python-breakpoints-in-c-thread-in-pycharm-or-eclipsepydev
        #pydevd.settrace(suspend=True, trace_only_current_thread=True)
        self._response_type_dict.get(ord(response_type), self._on_default)(response_type, p_api1, p_api2, double1, double2,
                                                                      ptr1, size1, ptr2, size2, ptr3, size3)

    def _on_default(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        print(u"功能还没实现:%d" % ord(response_type))

    def _OnConnectionStatus(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(RspUserLoginField)).contents
        else:
            obj = RspUserLoginField()
        self._spi.OnConnectionStatus(int(double1), obj, size1)

    def _OnRtnError(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(ErrorField)).contents
        self._spi.OnRtnError(obj)

    def _OnLog(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(LogField)).contents
        self._spi.OnLog(obj)

    def _OnRspQryOrder(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(OrderField)).contents
        else:
            obj = OrderField()
        self._spi.OnRspQryOrder(obj, size1, bool(double1))

    def _OnRspQryInvestorPosition(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(PositionField)).contents
        else:
            obj = PositionField()
        self._spi.OnRspQryInvestorPosition(obj, size1, bool(double1))

    def _OnRspQryInvestor(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(InvestorField)).contents
        else:
            obj = InvestorField()
        self._spi.OnRspQryInvestor(obj, size1, bool(double1))

    def _OnRtnOrder(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 <= 0:
            return
        obj = cast(ptr1, POINTER(OrderField)).contents
        self._spi.OnRtnOrder(obj)

    def _OnRtnDepthMarketData(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        # 由于这个结构体比较特殊，内存区域需要解析，所以直接传回去
        self._spi.OnRtnDepthMarketData(ptr1, size1)

    def _OnRspQryTradingAccount(self, response_type, p_api1, p_api2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3):
        if size1 > 0:
            obj = cast(ptr1, POINTER(AccountField)).contents
        else:
            obj = AccountField()
        self._spi.OnRspQryTradingAccount(obj, size1, bool(double1))
