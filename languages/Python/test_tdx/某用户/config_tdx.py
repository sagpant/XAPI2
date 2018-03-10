#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys

sys.path.append(r'D:\Users\Kan\Documents\GitHub\XAPI2\languages\Python')

# 解决Python 3.6的pandas不支持中文路径的问题
print(sys.getfilesystemencoding())  # 查看修改前的
try:
    sys._enablelegacywindowsfsencoding()  # 修改
    print(sys.getfilesystemencoding())  # 查看修改后的
except:
    pass

from test_tdx_api import main

configs = {
    # 根据目录，存放交易清单和中间文件等信息
    'root_dir': r'd:\\test_tdx\\某用户',
    # 交易，这下面的配置要求需要参考每种API的说明文档
    'td': {
        # TDX安装目录，注意最后有一个\
        'ExtInfoChar128': br'd:\new_hbzq' + b'\\',
        # 登录脚本。注意路径出现中文需要转码
        'Address': r'd:\test_tdx\Login_lua\Login_国信证券.lua'.encode('GBK'),
        # 资金账号
        'UserID': b'123456',
        # 用户密码
        'Password': b'654321',
        # 通迅密码。注意，不是验证码，不需要通迅密码的券商请留空
        'ExtInfoChar64': br'',
    },
}

if __name__ == '__main__':
    main(configs)
