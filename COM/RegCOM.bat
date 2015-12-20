@echo off
echo 此工具完成XAPI的COM版的注册。
echo 请右键“以管理员方式运行”否则无权限操作
@echo on
cd %~dp0
%~d0
RegAsm.exe QuantBox.XAPI.DLL /nologo /codebase /tlb
pause