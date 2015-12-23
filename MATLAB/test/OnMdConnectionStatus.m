function OnMdConnectionStatus(sender,arg)

import QuantBox.XAPI.*;

global md;

disp('MD')
% 交易连接回报
disp(arg.status);

switch arg.status
    case QuantBox.ConnectionStatus.Disconnected
        % 打印错误信息
        disp(Extensions_GBK.Text(arg.userLogin));
    case QuantBox.ConnectionStatus.Done
        % 订阅行情，支持";"分隔
        md.Subscribe('IF1602;IF1603;IF1606','');
end

end
