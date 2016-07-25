function OnRtnOrder(varargin)

event_struct = varargin{1,end-1};
order = event_struct.order;

% ´òÓ¡ÄÚÈİ
disp(order.InstrumentID);
disp(order.Status_String);
disp(order.ExecType_String);
disp(order.OpenClose_String);
disp(order.Text);

disp(order.LocalID);
disp(order.ID);

end
