#include "stdafx.h"
#include "SingleUser.h"

#include "../QuantBox_Queue/MsgQueue.h"

CSingleUser::CSingleUser()
{
}


CSingleUser::~CSingleUser()
{
}

void CSingleUser::UpdateQueryOrderTime(time_t t, double db, const char* szSource)
{
	m_QueryOrderTime = t;
}

void CSingleUser::UpdateQueryTradeTime(time_t t, double db, const char* szSource)
{
	m_QueryTradeTime = t;
}

void CSingleUser::CompareTradeMapAndEmit(unordered_map<string, TradeField*> &oldMap, unordered_map<string, TradeField*> &newMap)
{
	for (unordered_map<string, TradeField*>::iterator it = newMap.begin(); it != newMap.end(); ++it)
	{
		TradeField* pNewField = it->second;
		unordered_map<string, TradeField*>::iterator it2 = oldMap.find(pNewField->ID);
		if (it2 == oldMap.end())
		{
			// 没找到,是新单
			m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pNewField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		}
		else
		{
			TradeField* pOldField = it2->second;
			int Qty = pNewField->Qty - pOldField->Qty;
			if (Qty > 0)
			{
				// 有变化的单
				TradeField* pField = new TradeField;
				memcpy(pField, pNewField, sizeof(TradeField));
				pField->Qty = Qty;
				m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pNewField, sizeof(TradeField), nullptr, 0, nullptr, 0);
				delete[] pField;
			}
		}
	}
}

void CSingleUser::CompareTradeListAndEmit(list<TradeField*> &oldList, list<TradeField*> &newList)
{
	int i = 0;
	list<TradeField*>::iterator it2 = oldList.begin();
	for (list<TradeField*>::iterator it = newList.begin(); it != newList.end(); ++it)
	{
		TradeField* pField = *it;

		bool bUpdate = false;
		if (i >= oldList.size())
		{
			bUpdate = true;
		}
		//else
		//{
		//	// 相同位置的部分
		//	TradeField* pOldField = *it2;
		//	if (pOldField->Qty != pField->Qty)
		//	{
		//		bUpdate = true;
		//	}
		//}

		if (bUpdate)
		{
			m_msgQueue->Input_Copy(ResponeType::OnRtnTrade, m_msgQueue, m_pClass, 0, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
		}

		// 前一个可能为空，移动到下一个时需要注意
		if (it2 != oldList.end())
		{
			++it2;
		}

		++i;
	}
}