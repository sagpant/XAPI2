#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import pandas as pd
from itertools import *


def get_fields_columns_formats(struct):
    # 想转成DataFrame
    # 对于单个的C，在转换的时候是看不出来的，这种应当转换成数字方便
    columns = []
    formats = []
    for f in struct._fields_:
        columns.append(f[0])
        t = f[1]._type_
        if isinstance(t, str):
            formats.append(t)
        else:
            l = 'S' + str(f[1]._length_)
            formats.append(l)
    return columns, formats


def decode_dataframe(df, ctypes_struct=None):
    """
    将DataFrame中的b字符串，转成str字符串
    默认要object当成是字符串，如果中间出现别的也成字符串就会出错，这时就得原始ctypes来指明了

    字符串转换后与从其它源如csv中进行相互处理时比较方便，但再要下单时又得转回成byte比较麻烦
    :param df:
    :param ctypes_struct:
    :return:
    """
    if ctypes_struct is None:
        for i, f in enumerate(df.dtypes):
            # 以object类型做判断可能有误
            if f == object:
                df.iloc[:, i] = df.iloc[:, i].str.decode('gbk')
    else:
        for f in ctypes_struct._fields_:
            t = f[1]._type_
            if isinstance(t, str):
                pass
            else:
                name = f[0]
                df[name] = df[name].str.decode('gbk')
    return df


def encode_dataframe(df, ctypes_struct=None):
    """
    将DataFrame中的b字符串，转成str字符串
    默认要object当成是字符串，如果中间出现别的也成字符串就会出错，这时就得原始ctypes来指明了

    字符串转换后与从其它源如csv中进行相互处理时比较方便，但再要下单时又得转回成byte比较麻烦
    :param df:
    :param ctypes_struct:
    :return:
    """
    if ctypes_struct is None:
        for i, f in enumerate(df.dtypes):
            # 以object类型做判断可能有误
            if f == object:
                df.iloc[:, i] = df.iloc[:, i].str.encode('gbk')
    else:
        for f in ctypes_struct._fields_:
            t = f[1]._type_
            if isinstance(t, str):
                pass
            else:
                name = f[0]
                df[name] = df[name].str.encode('gbk')
    return df


def ctypes_dict_2_array(dict, dtype):
    """
    将特殊的字典转成数组
    :param dict:
    :param dtype:
    :return:
    """
    values = []
    keys = []
    for k, v in dict.items():
        d = np.frombuffer(v, dtype=dtype)
        values.extend(d)
        keys.extend(k)
    return keys, values


def ctypes_dict_2_dataframe(dict, dtype):
    """
    将ctypes字典转换成dataframe
    :param dict:
    :param dtype:
    :return:
    """
    keys, values = ctypes_dict_2_array(dict, dtype)
    df = pd.DataFrame.from_records(values, columns=dtype.names)
    return df


def extend_dataframe_product(df, symbols, columns=['InstrumentID', 'Side', 'HedgeFlag']):
    """
    将持仓列表按合约，方向，投保，进行扩展
    扩展的目的是为了后期计算方便
    最好能对上期所的单子进行扩展处理，因为上海的单子需要处理今昨指令

    TODO: 如果没有指定今昨，统一用平仓
    :param df:
    :param symbols:
    :param columns:
    :return:
    """
    # 生成叉乘序列
    x = product(symbols, [0, 1], [0, 1])
    y = pd.DataFrame(list(x), columns=columns)

    if df is None:
        return None

    z = pd.merge(df, y, how='outer', on=columns)
    # 因为很多na，做运算会出问题，所以要填充
    z.fillna(0, inplace=True)
    return z


def lock_positions(df, columns, input_position, output_position):
    """
    锁仓，通过开仓的方式达到会计上的平仓，用于解决某些直接平仓导致的手续费过高等问题
    如果两头都有持仓，都取最大那个
    建议锁仓记录存盘，然后由人来核对一下
    :param df:
    :param columns:
    :param input_position:
    :param output_position:
    :return:
    """
    # 先分组，对组里取最大值
    grp = df.groupby(columns)
    grp = grp.agg({input_position: 'max'})
    # 由于两个字段完全一样，合并时会出错，所以得另行处理
    grp.columns = [output_position]
    grp = grp.reset_index()
    x = pd.merge(df, grp, how='outer', on=columns)
    return x


def close_one_row(series, close_today_first):
    """
    平当前一行
    这里需要指定先平今还是先平昨

    :param series:
    :param close_today_first:
    :return:
    """

    ss = []
    # 选择先平今还是平昨，先平的放循环前面
    if close_today_first:
        fields = ['TodayPosition', 'HistoryPosition']
    else:
        fields = ['HistoryPosition', 'TodayPosition']

    leave = series['Open_Amount']
    for i in range(len(fields)):
        # 标记是否平今
        series['CloseToday_Flag'] = int(fields[i] == 'TodayPosition')
        if leave == 0:
            # 没有要平的了，可以退了
            break
        sum_ = series[fields[i]] + leave
        if sum_ < 0:
            series['Open_Amount'] = - series[fields[i]]
            if series['Open_Amount'] != 0:
                ss.append(series)
        else:
            series['Open_Amount'] = leave
            ss.append(series)
        leave = sum_

    return ss


def calc_target_orders(df, target_position, init_position):
    """
    计算中间变动委托单的记录
    一开始计算出来的是不区别平今与平昨的列表
    :param df:
    :param target_position:
    :param init_position:
    :return:
    """
    # 先将Long/Short转成1/-1，可用于后面的计算
    # 目前从XAPI中拿到的是0/1转成1/-1

    # df['Long_Flag'] = 1
    # df['Long_Flag'][df['Side'] == 1] = -1
    df['Long_Flag'] = 1 - 2 * df['Side']  # 多空方向

    # 正负就是开平仓，后面会对它进行修改
    df['Open_Amount'] = df[target_position] - df[init_position]
    df2 = df[df['Open_Amount'] != 0]  # 只留下仓位有变化的

    if df2.empty:
        return None

    # df2.loc[:, 'CloseToday_Flag'] = 0  # 新建一列，0表示昨仓或平仓，1表示今仓
    df2 = df2.assign(CloseToday_Flag=0)  # 换成这个后不再出现SettingWithCopy警告

    # 对于要平仓的数据，可以试着用循环的方法生成两条进行处理
    df3 = []
    for i in range(len(df2)):
        s = df2.iloc[i].copy()  # copy一下，后再才不会再出SettingWithCopy警告
        # 上海的平仓操作需要分解成两个
        if s['IsSHFE'] and s['Open_Amount'] < 0:
            # 扩展成两个，这里只做平仓，不做开仓，所以逻辑会简单一些
            # 但是针对不同的产品，开平的先后是有区别的
            # 如果平今与平昨的价格相差不大，先开先平是没有区别的
            # 如果开仓钱不够，还是应当先平
            df3.extend(close_one_row(s, True))
        else:
            # 不用扩展
            df3.append(s)
        pass
    df4 = pd.DataFrame.from_records(df3)
    # 重新计算买卖数量,正负就是买卖
    df4.loc[:, 'Buy_Amount'] = df4['Long_Flag'] * df4['Open_Amount']

    return df4
