#!/usr/bin/env python
# -*- coding: utf-8 -*-

# 参考此文进行测试
# 提供的方法很多，最后找了一个简单又能达到目的
# http://stackoverflow.com/questions/36932/how-can-i-represent-an-enum-in-python

from ctypes import *


class EnumBase(type):
    def __init__(self, name, base, fields):
        super(EnumBase, self).__init__(name, base, fields)
        self.__mapping = dict((v, k) for k, v in fields.iteritems())

    def __getitem__(self, val):
        return self.__mapping[val]


def enum_kv(*seq, **named):

    enums = dict(zip(seq, range(len(seq))), **named)
    return EnumBase('Enum', (), enums)


def enum_k(start, *sequential, **named):
    enums = dict(zip(sequential, range(start, start+len(sequential))), **named)
    return EnumBase('Enum', (), enums)

Numbers = enum_kv(ONE=1, TWO=2, THREE='three')
print(Numbers.TWO)
print(Numbers[Numbers.ONE])
print(Numbers[2])
print(Numbers['three'])


ConnectionStatus = enum_k(
    10,
    'Uninitialized',
    'Initialized',
    'Disconnected',
    'Connecting',
    'Connected',
    'Authorizing',
    'Authorized',
    'Logining',
    'Logined',
    'Confirming',
    'Confirmed',
    'Doing',
    'Done',
    'Unknown',
)

print(ConnectionStatus)
print(type(ConnectionStatus))
print(ConnectionStatus.Authorized)
print(ConnectionStatus[ConnectionStatus.Authorized])
print(type(ConnectionStatus.Authorized))
