#include "stdafx.h"
#include "MsgQueue.h"

CMsgQueue::CMsgQueue()//:m_queue(1024)
{
	m_hThread = nullptr;
	m_bRunning = false;
	m_bDirectOutput = false;

	//回调函数地址指针
	m_fnOnResponse = nullptr;
}

void CMsgQueue::Clear()
{
	ResponeItem* pItem = nullptr;
	//清空队列
	while (m_queue.try_dequeue(pItem))
	{
		if (pItem->bNeedDelete)
		{
			delete[] pItem->ptr1;
			delete[] pItem->ptr2;
			delete[] pItem->ptr3;
		}
		delete pItem;
	}
}

bool CMsgQueue::Process()
{
	try
	{
		ResponeItem* pItem = nullptr;
		if (m_queue.try_dequeue(pItem))
		{
			Output(pItem);
			if (pItem->bNeedDelete)
			{
				// 删除的是数组，目前是自己模块创建的自己删
				delete[] pItem->ptr1;
				delete[] pItem->ptr2;
				delete[] pItem->ptr3;
			}
			delete pItem;
			return true;
		}
		return false;
	}
	catch (...)
	{
		return false;
	}
}

void CMsgQueue::Output(ResponeItem* pItem)
{
	try
	{
		if (m_fnOnResponse)
			(*m_fnOnResponse)(pItem->type, pItem->pApi1, pItem->pApi2, pItem->double1, pItem->double2, pItem->ptr1, pItem->size1, pItem->ptr2, pItem->size2, pItem->ptr3, pItem->size3);
	}
	catch (...)
	{
		m_fnOnResponse = nullptr;
	}
}

void CMsgQueue::StartThread()
{
    if(nullptr == m_hThread)
    {
        m_bRunning = true;
        m_hThread = new thread(ProcessThread,this);
    }
}

void CMsgQueue::StopThread()
{
    m_bRunning = false;
	m_cv.notify_all();
	this_thread::sleep_for(chrono::milliseconds(10));
	lock_guard<mutex> cl(m_mtx_del);
    if(m_hThread)
    {
		// 这里很少有机会执行
		m_hThread->detach();
        m_hThread = nullptr;
    }
}


void CMsgQueue::RunInThread()
{
	try
	{
		while (m_bRunning)
		{
			if (Process())
			{
			}
			else
			{
				// 空闲时等1ms,如果立即有事件过来就晚了1ms
				//this_thread::sleep_for(chrono::milliseconds(1));

				// 空闲时过来等1ms,没等到就回去再试
				// 如过正好等到了，就立即去试，应当会快一点吧?

				// wait_for的效果为何不是遇到事件后就立即返回？而是一定要等满时间？太奇怪了，这个地方可以改成seconds(10)就知道效果了
				unique_lock<mutex> lck(m_mtx);
				//m_cv.wait_for(lck, std::chrono::seconds(10), [this]{return m_bRunning == false; }); //这种写法会导致收到事件立即处理的功能失效
				m_cv.wait_for(lck, std::chrono::seconds(2));
			}
		}
	}
	catch (...)
	{

	}
	
	// 这句加了有效果吗？
	// 加了后可能报以下错，但不当了
/*
	************** Exception Text **************
		System.Runtime.InteropServices.SEHException(0x80004005) : External component has thrown an exception.
		at XAPI.Proxy.XRequest(Byte type, IntPtr pApi1, IntPtr pApi2, Double double1, Double double2, IntPtr ptr1, Int32 size1, IntPtr ptr2, Int32 size2, IntPtr ptr3, Int32 size3) in D : \Users\Kan\Documents\GitHub\XAPI2\languages\CSharp\XAPI_CSharp\Proxy.cs:line 79
*/
	//m_hThread->detach();

	
	// 清理线程
	m_hThread = nullptr;
	m_bRunning = false;
}

ConfigInfoField* CMsgQueue::Config(ConfigInfoField* pConfigInfo)
{
	return nullptr;
}
