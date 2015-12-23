#include "stdafx.h"
#include "MsgQueue.h"
#include "../include/QueueHeader.h"
#include "../include/QueueEnum.h"
#include "../include/QueueStruct.h"

#include "../include/ApiHeader.h"
#include "../include/ApiStruct.h"

inline CMsgQueue* GetQueue(void* pMsgQueue)
{
	return static_cast<CMsgQueue*>(pMsgQueue);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	switch (rt)
	{
	case RequestType_GetApiType:
		return nullptr;
	case RequestType_GetApiVersion:
		return (void*)"0.4.0.20150526";
	case RequestType_GetApiName:
		return (void*)"Queue";
	case RequestType_Create:
		return new CMsgQueue();
	default:
		break;
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CMsgQueue* pQueue = GetQueue(pApi1);

	switch (rt)
	{
	case RequestType_Release:
		delete pQueue;
		return 0;
	case RequestType_Register:
		pQueue->Register(ptr1,ptr2);
		break;
	case RequestType_Config:
		return (void*)pQueue->Config((ConfigInfoField*)ptr1);
	case RequestType_Connect:
		pQueue->StartThread();
		break;
	case RequestType_Disconnect:
		pQueue->StopThread();
		break;
	case RequestType_Clear:
		pQueue->Clear();
		break;
	case RequestType_Process:
		pQueue->Process();
		break;
	default:
		// 如果动态调用，使用直接输出的方式
		pQueue->Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		break;
	}

	return pApi1;
}