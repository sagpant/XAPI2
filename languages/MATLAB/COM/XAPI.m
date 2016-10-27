classdef XAPI < handle
    properties
        api
    end
    methods
        function Init(obj)
            obj.api = actxserver('XAPI.COM');
            obj.api.SetLibPath(fullfile(cd,'XAPI\x86\Tdx\Tdx_Trade_x86.dll'));
            obj.api.SetLibPath('C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\Tdx\Tdx_Trade_x86.dll');
            
            registerevent(obj.api,{'OnConnectionStatus' mycallbacks('global_dispatcher')});
%             registerevent(obj.api,{'OnRtnOrder' mycallbacks('global_dispatcher')});
%             registerevent(obj.api,{'OnRspQryOrder' mycallbacks('global_dispatcher')});
%             registerevent(obj.api,{'OnRspQryInvestorPosition' mycallbacks('global_dispatcher')});
            
        end
        
        function Login(obj)
            obj.api.SetServerInfo('ExtInfoChar128','D:\new_gjzq_v6\');
            obj.api.SetServerInfo('Address','D:\new_gjzq_v6\Login.lua');

            obj.api.SetUserInfo('UserID','123456');
            obj.api.SetUserInfo('Password','123');
            obj.api.SetUserInfo('UserID','123456');
            obj.api.SetUserInfo('Password','123');
            
            obj.api.Connect();
        end
        
        function dispatcher(obj,varargin)
            % varargin中每个cell的意思请参考doc中的COM Event Handlers
            varargin_ = varargin{1,1};
            event_struct = varargin_{1,end-1};
            
            e = object_2_table(event_struct);
            t = object_2_table(e{1,end-1});
            % 可能因为换行的原因，导致
            disp(t)
        end
    end
end