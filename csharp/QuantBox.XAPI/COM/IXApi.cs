using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.COM
{
    [ComVisible(true)]
    [Guid("DBCAEFA9-4337-47B8-9B47-66070604CE2C")]
    public interface IXApi
    {
        [DispId(1)]
        void SetLibPath(string LibPath);
        [DispId(2)]
        void SetServerInfo(ref ServerInfoClass ServerInfo);
        [DispId(3)]
        void SetUserInfo(ref UserInfoClass UserInfo);


        [DispId(10)]
        void Connect();
        [DispId(11)]
        void Disconnect();

        [DispId(20)]
        void Subscribe(string szInstrument, string szExchange);
        [DispId(21)]
        void Unsubscribe(string szInstrument, string szExchange);

        [DispId(31)]
        void SendOrder(ref OrderClass[] orders); // , out string[] OrderRefs
        [DispId(32)]
        void CancelOrder(ref string[] ids);
        
            
        [DispId(100)]
        void ReqQuery(int type, ref ReqQueryClass query);
        
    }
}
