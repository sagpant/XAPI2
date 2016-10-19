%% 交易
td = Tdx_Connect();

% 需要循环等待才行
SetWait(0);
Wait();

%% 一定要等待登录成功才做
orders = ReqQryOrder(td)
positions = ReqQryInvestorPosition(td)
return;


%% 通过列表取行情
w = WindConnect();
MarketData = GetQuoteFromWind(wind_code,w,1);
MarketData.stock_code = WindCode2StockCode(MarketData.wind_code);

% 开盘涨幅度
MarketData.RT_OPEN_CHG = (MarketData.RT_OPEN - MarketData.RT_PRE_SETTLE)./MarketData.RT_PRE_SETTLE;

[m,n] = size(MarketData);
MarketData.price = zeros(m,1);

% 注意，这是根据开盘价设置价格，而不是根据最新价
for i=1:m
    RT_OPEN_CHG = MarketData.RT_OPEN_CHG(i);
    if RT_OPEN_CHG>0.07
        MarketData.price(i) = MarketData.RT_OPEN(i)*0.98;
    elseif RT_OPEN_CHG<-0.07
        MarketData.price(i) = MarketData.RT_OPEN(i)*0.99;
    else
        MarketData.price(i) = MarketData.RT_OPEN(i)*1;
        %MarketData.price(i) = MarketData.RT_OPEN(i) - MarketData.RT_PRE_SETTLE(i)*0.01;
    end
end

TradeList = MarketData(MarketData.Enable~=0,{'stock_code','price'});
[m,n] = size(TradeList);

TradeList.qty = ones(m,1).*500;

for i=1:m
    BuyLimit(td,TradeList.stock_code{i}, TradeList.qty(i), TradeList.price(i));
end

