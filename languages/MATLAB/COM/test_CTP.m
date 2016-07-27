%% 启动行情
%md = actxcontrol('XAPI.COM');
md = actxserver('XAPI.COM');

% 查看所支持的事件
events(md);


md.SetLibPath(fullfile(cd,'XAPI\x86\CTP\CTP_Quote_x86.dll'));

md.SetServerInfo('BrokerID','9999');
md.SetServerInfo('Address','tcp://180.168.146.187:10010');

md.SetServerInfo('BrokerID','4040');
md.SetServerInfo('Address','tcp://yhzx-front2.yhqh.com:51213');

md.SetUserInfo('UserID','123456');
md.SetUserInfo('Password','123456');

registerevent(md,{'OnConnectionStatus' @OnMdConnectionStatus});
registerevent(md,{'OnRtnDepthMarketData' @OnRtnDepthMarketData});
% 查看注册的事件
eventlisteners(md)

md.Connect();

%% 订阅
md.Subscribe('IF1608;IF1609;IF1612','');
md.Unsubscribe('IF1608;IF1609;IF1612','');

%% 交易
%td = actxcontrol('XAPI.COM');
td = actxserver('XAPI.COM');
td.SetLibPath(fullfile(cd,'XAPI\x86\CTP\CTP_Trade_x86.dll'));
td.SetServerInfo('BrokerID','9999');
td.SetServerInfo('Address','tcp://180.168.146.187:10000');

td.SetUserInfo('UserID','037505');
td.SetUserInfo('Password','123456');

registerevent(td,{'OnConnectionStatus' @OnTdConnectionStatus});
registerevent(td,{'OnRtnOrder' @OnRtnOrder});

td.Connect();

%% 下单
order1 = BuyLimit(td,'IF1608',1,3000);
disp(order1);

%% 撤单
td.CancelOrder(order1);

%% 退出
% md.Dispose() %行情退出
% td.Dispose() %交易退出

