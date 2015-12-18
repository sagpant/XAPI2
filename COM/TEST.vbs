Run32()

Dim XApi
Set XApi = CreateObject("XApiCom")
XApi.SetLibPath "C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\CTP\x86\QuantBox_CTP_Quote.dll"
XApi.SetServerInfo "Address","tcp://180.168.146.187:10010"
XApi.SetServerInfo "BrokerID","9999"
XApi.SetUserInfo "UserID","037505"
XApi.SetUserInfo "Password","123456"
XApi.OnTest = GetRef("OnTest")
XApi.Connect

Function OnTest(ByVal status)
    MsgBox status
End Function 

Sub TdApi_OnConnectionStatus(sender, status, status_String, userLogin, size1)
    MsgBox status_String
End Sub

Sub Run32()
    'Author: Demon
    'Date: 2015/7/9
    'Website: http://demon.tw

    Dim strComputer, objWMIService, colItems, objItem, strSystemType
    strComputer = "."
    Set objWMIService = GetObject("winmgmts:\\" & strComputer & "\root\cimv2")
    Set colItems = objWMIService.ExecQuery("Select * from Win32_ComputerSystem",,48)
    
    For Each objItem in colItems
        strSystemType = objItem.SystemType
    Next
    
    If InStr(strSystemType, "x64") > 0 Then
        Dim fso, WshShell, strFullName
        Set fso = CreateObject("Scripting.FileSystemObject")
        Set WshShell = CreateObject("WScript.Shell")
        strFullName = WScript.FullName
        If InStr(1, strFullName, "system32", 1) > 0 Then
            strFullName = Replace(strFullName, "system32", "SysWOW64", 1, 1, 1)
            WshShell.Run strFullName & " " &_
                """" & WScript.ScriptFullName & """", 10, False
            WScript.Quit
        End If
    End If
End Sub