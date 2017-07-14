REM 11 合并持仓
REM 7 循环下单
python.exe test_ctp_api.py --input=11;22;32 >log/%date:~0,4%%date:~5,2%%date:~8,2%.log
REM 最后这里加一个发邮件就成了
python mail.py --username=123 --password=456 --from=123@qq.com --to=456@qq.com --log=log --bat=%~f0