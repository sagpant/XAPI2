#include "stdafx.h"
#include "MdUserApi.h"
#include "../../include/QueueEnum.h"
#include "../../include/QueueHeader.h"

#include "../../include/ApiHeader.h"
#include "../../include/ApiStruct.h"

#include "../../include/toolkit.h"
#include "../../include/ApiProcess.h"

#include "../../common/Queue/MsgQueue.h"
#ifdef _REMOTE
#include "../../common/Queue/RemoteQueue.h"
#endif

#include "../../include/Wind/WAPIWrapperCpp.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mutex>
#include <vector>

using namespace std;

int Today(int day)
{
    time_t now = time(0);
    now += day * 86400;
    struct tm *ptmNow = localtime(&now);

    return (ptmNow->tm_year + 1900) * 10000
        + (ptmNow->tm_mon + 1) * 100
        + ptmNow->tm_mday;
}

void WindGetErrorMessage(LONG errCode, char* buffer, int bufferSize){
    WCHAR errMsg[1024] = { 0 };
    int msgLength = sizeof errMsg;
    CWAPIWrapperCpp::getErrorMsg(errCode, eCHN, errMsg, msgLength);
    auto strLen = wcslen(errMsg) + 1;
    char* str = new char[strLen];
    size_t converted;
    wcstombs_s(&converted, str, strLen, errMsg, _TRUNCATE);
    strcpy_s(buffer, bufferSize, str);
    delete[] str;
}

void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
    // 由内部调用，不用检查是否为空
    CMdUserApi* pApi = (CMdUserApi*)pApi2;
    pApi->QueryInThread(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
    return nullptr;
}

CMdUserApi::CMdUserApi(void)
{
    m_CoInitialized = false;
    // 自己维护两个消息队列
    m_msgQueue = new CMsgQueue();
    m_msgQueue_Query = new CMsgQueue();

    m_msgQueue_Query->Register((void*)Query, this);
    m_msgQueue_Query->StartThread();
}

CMdUserApi::~CMdUserApi(void)
{
    Disconnect();
}

void CMdUserApi::QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
    int iRet = 0;
    switch (type)
    {
    case Start:
        WindStart();
        break;
    case Stop:
        WindStop();
        break;
    case QueryInstrument:
        WindQryInstrument();
        break;
    default:
        break;
    }

    this_thread::sleep_for(chrono::milliseconds(1));
}

void CMdUserApi::Register(void* pCallback, void* pClass)
{
    m_pClass = pClass;
    if (m_msgQueue == nullptr)
        return;

    m_msgQueue_Query->Register((void*)Query, this);
    m_msgQueue->Register(pCallback, this);
    if (pCallback)
    {
        m_msgQueue_Query->StartThread();
        m_msgQueue->StartThread();
    }
    else
    {
        m_msgQueue_Query->StopThread();
        m_msgQueue->StopThread();
    }
}

void CMdUserApi::Connect()
{
    m_msgQueue_Query->Input_NoCopy(RequestType::Start, m_msgQueue_Query, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
}

void CMdUserApi::WindStart()
{
    if (!m_CoInitialized){
        CoInitialize(nullptr);
        m_CoInitialized = true;
    }

    m_msgQueue->Input_NoCopy(ResponeType::ResponeType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connecting, 0, nullptr, 0, nullptr, 0, nullptr, 0);

    auto errCode = CWAPIWrapperCpp::start();
    if (errCode == 0){
        //m_msgQueue->Input_NoCopy(ResponeType::ResponeType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Connected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
        m_msgQueue->Input_NoCopy(ResponeType::ResponeType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Done, 0, nullptr, 0, nullptr, 0, nullptr, 0);
    }
    else{
        RspUserLoginField* pField = (RspUserLoginField*)m_msgQueue->new_block(sizeof(RspUserLoginField));
        pField->RawErrorID = errCode;
        WindGetErrorMessage(errCode, pField->Text, sizeof pField->Text);
        m_msgQueue->Input_NoCopy(ResponeType::ResponeType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, pField, sizeof(RspUserLoginField), nullptr, 0, nullptr, 0);
    }
}

void CMdUserApi::WindStop()
{
    if (WindConnected()){
        CWAPIWrapperCpp::stop();
        // 全清理，只留最后一个
        m_msgQueue->Clear();
        m_msgQueue->Input_NoCopy(ResponeType::ResponeType_OnConnectionStatus, m_msgQueue, m_pClass, ConnectionStatus::ConnectionStatus_Disconnected, 0, nullptr, 0, nullptr, 0, nullptr, 0);
        // 主动触发
        m_msgQueue->Process();
    }

    if (m_CoInitialized){
        CoUninitialize();
        m_CoInitialized = false;
    }

    SetEvent(m_ExitEvent);
}

bool CMdUserApi::WindConnected()
{
    return CWAPIWrapperCpp::isconnected();
}

void CMdUserApi::WindQryInstrument()
{
    WindData wd;
    WCHAR options[1024];
    _snwprintf_s(options, sizeof options, _TRUNCATE, L"date=%d;sectorId=a001010100000000;field=wind_code,sec_name", Today(0));
    auto errCode = CWAPIWrapperCpp::wset(wd, L"SectorConstituent", options);
    if (errCode != 0){
        ErrorField* pField = (ErrorField*)m_msgQueue->new_block(sizeof(ErrorField));
        pField->RawErrorID = errCode;
        strcpy_s(pField->Source, sizeof pField->Source, "WindQryInstrument");
        WindGetErrorMessage(errCode, pField->Text, sizeof pField->Text);
        m_msgQueue->Input_NoCopy(ResponeType::ResponeType_OnRtnError, m_msgQueue, m_pClass, 0, 0, pField, sizeof(ErrorField), nullptr, 0, nullptr, 0);
        return;
    }

}

void CMdUserApi::Disconnect()
{
    // 清理查询队列
    if (m_msgQueue_Query)
    {
        m_ExitEvent = CreateEvent(nullptr, true, false, nullptr);
        m_msgQueue_Query->Input_NoCopy(RequestType::Stop, m_msgQueue_Query, this, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0);
        WaitForSingleObject(m_ExitEvent, INFINITE);
        m_msgQueue_Query->StopThread();
        m_msgQueue_Query->Register(nullptr, nullptr);
        m_msgQueue_Query->Clear();
        delete m_msgQueue_Query;
        m_msgQueue_Query = nullptr;
    }

    // 清理响应队列
    if (m_msgQueue)
    {
        m_msgQueue->StopThread();
        m_msgQueue->Register(nullptr, nullptr);
        m_msgQueue->Clear();
        delete m_msgQueue;
        m_msgQueue = nullptr;
    }
}

void CMdUserApi::Subscribe(const string& szInstrumentIDs, const string& szExchangeID)
{

}

void CMdUserApi::Subscribe(const set<string>& instrumentIDs, const string& szExchangeID)
{

}

void CMdUserApi::Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID)
{
}
