%% 导入C#库，请按自己目录进行调整
cd 'd:\wukan\Documents\GitHub\XAPI2\Matlab\test'
NET.addAssembly(fullfile(cd,'QuantBox.XAPI.dll'));

import QuantBox.XAPI.Event.*;

%% 行情
global md;
md = XApiWrapper(fullfile(cd,'QuantBox_CTP_Quote.dll'));
md.Server.BrokerID = '9999';
md.Server.Address = 'tcp://180.168.146.187:10010';

md.User.UserID = '123456';
md.User.Password = '123456';

addlistener(md,'OnConnectionStatus',@OnMdConnectionStatus);
addlistener(md,'OnRtnDepthMarketData',@OnRtnDepthMarketData);

md.Connect();

%% 交易
global td;
td = XApiWrapper(fullfile(cd,'QuantBox_CTP_Trade.dll'));
td.Server.BrokerID = '9999';
td.Server.Address = 'tcp://180.168.146.187:10000';

td.User.UserID = '123456';
td.User.Password = '123456';

addlistener(td,'OnConnectionStatus',@OnTdConnectionStatus);
addlistener(td,'OnRtnOrder',@OnRtnOrder);

td.Connect();

%% 下单
order1 = BuyLimit('IF1601',1,3000)
disp(order1)

%% 撤单
td.CancelOrder(order1);

%% 退出
% md.Dispose() %行情退出
% td.Dispose() %交易退出