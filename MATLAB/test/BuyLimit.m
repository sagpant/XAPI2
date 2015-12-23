function OrderRef = BuyLimit(Instrument,Qty,Price)

import QuantBox.XAPI.*;

global td;

% ÏÂµ¥
order = QuantBox.XAPI.OrderField;
order.InstrumentID = Instrument;
order.Type = QuantBox.OrderType.Limit;
order.Side = QuantBox.OrderSide.Buy;
order.Qty = Qty;
order.Price = Price;
order.OpenClose = QuantBox.OpenCloseType.Open;
order.HedgeFlag = QuantBox.HedgeFlagType.Speculation;

OrderRef = td.SendOrder(order);

end
