%% 交易
td = Tdx_Connect();

% 需要循环等待才行
SetWait(0);
Wait();

%% 一定要等待登录成功才做
orders = ReqQryOrder(td)
positions = ReqQryInvestorPosition(td)
return;
