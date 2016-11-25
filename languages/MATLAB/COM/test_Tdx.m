%% 交易
td = Tdx_Connect();

% 需要循环等待才行
SetWait(0);
Wait();
return

%% 一定要等待登录成功才做
positions = ReqQryInvestorPosition(td,10)
orders = ReqQryOrder(td,10)
return;

BuyLimit(td,'600000', 100, 16.4);

td.CancelOrder('20161024:A499180558:1166')

td.Subscribe('600000','')
td.Subscribe('000001','')
