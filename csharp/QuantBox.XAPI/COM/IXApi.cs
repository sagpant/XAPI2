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
        void SetServerInfo(string key,object value);
        [DispId(3)]
        void SetUserInfo(string key, object value);


        [DispId(10)]
        void Connect();
        [DispId(11)]
        void Disconnect();

        [DispId(20)]
        void Subscribe(string szInstrument, string szExchange);
        [DispId(21)]
        void Unsubscribe(string szInstrument, string szExchange);

        [DispId(31)]
        string SendOrder(ref OrderClass[] orders);
        [DispId(32)]
        string CancelOrder(ref string[] ids);
        
            
        [DispId(100)]
        void ReqQuery(int type, ref ReqQueryClass query);

        [DispId(200)]
        void SetOnTest(Delegate del);

    }
}
