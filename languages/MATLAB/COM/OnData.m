function OnData(varargin)
disp('abc')
data = varargin{1,1}.TryDequeue();
if isempty(data)
    return;
end

% switch(data.Type_String)
%     case 'OnConnectionStatus'
%         OnMdConnectionStatus(data.Type,data.Type_String,data.Sender,data.Data1,data.Data2,data.Data3,data.Data4);
%     otherwise
%         disp(data.Type_String)
% end

end