REM 2 查询持仓
REM 32 持仓回报等10秒
REM 12 写回目标持仓文件
REM 13 合并增量文件
REM 7 循环下单
python.exe test_ctp_api.py --input=2;32;12;13;7  >log/%date:~0,4%%date:~5,2%%date:~8,2%.log
REM 最后这里加一个发邮件就成了
python mail.py --username=123 --password=456 --from=123@qq.com --to=123@qq.com;456@qq.com --log=log --bat=%~f0
