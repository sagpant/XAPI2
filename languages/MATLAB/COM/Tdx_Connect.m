%% 交易
function td = Tdx_Connect()

% 只能实例化一次
td = actxserver('XAPI.COM');
td.SetLibPath(fullfile(cd,'XAPI\x86\Tdx\Tdx_Trade_x86.dll'));
td.SetLibPath('C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\Tdx\Tdx_Trade_x86.dll');

td.SetServerInfo('ExtInfoChar128','D:\tc_yhzq_v6\');
td.SetServerInfo('Address','D:\tc_yhzq_v6\Login.lua');

td.SetUserInfo('UserID','123');
td.SetUserInfo('Password','123');

registerevent(td,{'OnConnectionStatus' @OnTdConnectionStatus});
registerevent(td,{'OnRtnOrder' @OnRtnOrder});
registerevent(td,{'OnRspQryOrder' @OnRspQryOrder});
registerevent(td,{'OnRspQryInvestorPosition' @OnRspQryInvestorPosition});

td.Connect();

end

