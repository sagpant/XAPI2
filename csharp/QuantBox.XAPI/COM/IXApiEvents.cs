using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.COM
{
    [ComVisible(true)]
    [Guid("4D7F9B54-3098-45AE-8EEB-4ED24AC47613")]
    [InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    public interface IXApiEvents
    {
        void OnConnectionStatus(object sender, int status,string status_string, [In] ref RspUserLoginClass userLogin, int size1);
        void OnRtnDepthMarketData(object sender, [In]ref DepthMarketDataNClass marketData);
    }
}
