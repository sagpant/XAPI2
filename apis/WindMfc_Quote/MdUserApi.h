#pragma once

#include "../../include/ApiStruct.h"

#ifdef _WIN64
#pragma comment(lib, "../../include/Wind/win64/WAPIWrapperCpp.lib")
#pragma comment(lib, "../../lib/Queue_x64.lib")
#else
#pragma comment(lib, "../../include/Wind/win32/WAPIWrapperCpp.lib")
#pragma comment(lib, "../../lib/Queue_x86.lib")
#endif

#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <map>
#include <thread>

using namespace std;

class CMsgQueue;
class CWindDialog;

class CMdUserApi
{
    enum RequestType
    {
        Start,
        Stop,
        QueryInstrument,
    };

public:
    CMdUserApi(void);
    virtual ~CMdUserApi(void);

    void Register(void* pCallback, void* pClass);

    void Connect();
    void Disconnect();

    void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);
    void Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID);
    
    void WindStart() const;
    void WindStop();
    static bool WindConnected();
    void WindQryInstrument() const;

private:
    friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
    void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

    //订阅行情
    void Subscribe(const set<string>& instrumentIDs, const string& szExchangeID);
    void DialogThread();
private:
    mutex m_csMapInstrumentIDs;
    mutex m_csMapQuoteInstrumentIDs;

    //正在订阅的合约
    map<string, set<string> > m_mapInstrumentIDs;
    //正在订阅的合约
    map<string, set<string> > m_mapQuoteInstrumentIDs;

    //消息队列指针
    CMsgQueue* m_msgQueue;
    CMsgQueue* m_msgQueue_Query;
    void* m_pClass;
    CWindDialog* m_pWindDialog;
    thread* m_pDialogThread;
};

