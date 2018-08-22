#if defined WINDOWS || _WIN32
#include <Windows.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <string>

#include "../../../include/XApiCpp.h"

static const char *szExchange_SH = "SSE";
static const char *szExchange_SZ = "SZSE";

class CXSpiImpl : public CXSpi
{
public:
	CXSpiImpl()
		: CXSpi()
	{
		client_id_ = 1;
	}

	virtual void OnConnectionStatus(CXApi* pApi, ConnectionStatus status, RspUserLoginField* pUserLogin, int size1)
	{
		std::cout << "connection status: " << status << std::endl;
		if (status == ConnectionStatus::ConnectionStatus_Done)
		{
			if ((pApi->GetApiTypes() & ApiType::ApiType_MarketData) == ApiType::ApiType_MarketData)
			{
				std::cout << "XTP Api Ready" << std::endl;

				char *symbols[] = {
					"600090",
					"600887",
				};
				int count = sizeof(symbols) / sizeof(char*) - 1;

				std::string buf = "";
				for (int i = 0; i < count; i++)
				{
					buf.append(symbols[i]);
					if (i != count - 1)
					{
						buf.append(";");
					}
				}
				pApi->Subscribe(buf.c_str(), szExchange_SH);
			}
		}
	}

	virtual void OnRtnDepthMarketDataN(CXApi* pApi, DepthMarketDataNField* pMarketData)
	{
		std::cout << "{"
			<< "'msg': 'OnRtnDepthMarketDataN', "
			<< "'exchangeID': '" << pMarketData->ExchangeID << "', "
			<< "'exchangeEnum': " << pMarketData->Exchange << ", "
			<< "'symbol': '" << pMarketData->Symbol << "', "
			<< "'last_price': " << pMarketData->LastPrice << ", "
			<< "'volume': " << pMarketData->Volume << ", "
			<< "'turnover': " << pMarketData->Turnover << ", "
			<< "'avg_price': " << pMarketData->AveragePrice << ", "
			<< "'open_price': " << pMarketData->OpenPrice << ", "
			<< "'close_price': " << pMarketData->ClosePrice << ", "
			<< "'high_price': " << pMarketData->HighestPrice << ", "
			<< "'low_price': " << pMarketData->LowestPrice << ", "
			<< "'upper_limit_price': " << pMarketData->UpperLimitPrice << ", "
			<< "'lower_limit_price': " << pMarketData->LowerLimitPrice << ", "
			<< "'pre_close_price': " << pMarketData->PreClosePrice << ", "
			<< "'pre_settle_price': " << pMarketData->PreSettlementPrice << ", ";

		int size = sizeof(DepthField);
		char* pBid = ((char*)pMarketData) + sizeof(DepthMarketDataNField);
		int AskCount = (pMarketData->Size - sizeof(DepthMarketDataNField)) / size - pMarketData->BidCount;
		char* pAsk = ((char*)pMarketData) + sizeof(DepthMarketDataNField) + pMarketData->BidCount*size;

		std::cout << "'bids': [";
		for (int i = 0; i < pMarketData->BidCount; i++)
		{
			DepthField* pDF = (DepthField*)(pBid + i * size);
			std::cout << "[" << pDF->Price << ", " << pDF->Size << "]";
			if (i != pMarketData->BidCount - 1)
			{
				std::cout << ", ";
			}
		}
		std::cout << "], ";

		std::cout << "'asks': [";
		for (int i = 0; i < pMarketData->BidCount; i++)
		{
			DepthField* pDF = (DepthField*)(pAsk + i * size);
			std::cout << "[" << pDF->Price << ", " << pDF->Size << "]";
			if (i != pMarketData->BidCount - 1)
			{
				std::cout << ", ";
			}
		}
		std::cout << "]";

		std::cout << "}" << std::endl;
	}

public:
	int client_id_;
};

int main(int argc, char *argv[])
{
	CXSpiImpl* p = new CXSpiImpl();

#if _WIN32
	#if _WIN64
		char shared_lib_quote[250] = "XTP_Quote_x64.dll";
	#else
		char shared_lib_quote[250] = "XTP_Quote_x86.dll";
	#endif
#else
	char shared_lib_quote[250] = "XTP_Quote.so";
#endif

	ServerInfoField				quote_server_info = { 0 };
	UserInfoField				user_info = { 0 };

	strcpy(quote_server_info.Address, "120.27.164.138");
	quote_server_info.IsUsingUdp = false;
	quote_server_info.Port = 6002;

	strcpy(user_info.UserID, "15033731");
	strcpy(user_info.Password, "LSMph6g6");
	user_info.ExtInfoInt32 = p->client_id_;

	CXApi* p_api_quote = CXApi::CreateApi(shared_lib_quote);

	if (!p_api_quote->Init())
	{
		printf("%s\r\n", p_api_quote->GetLastError());
		p_api_quote->Disconnect();
		return 1;
	}

	p_api_quote->RegisterSpi(p);
	p_api_quote->Connect("./", &quote_server_info, &user_info, 0);
	printf("已经执行完Connect\n");

	getchar();

	p_api_quote->Disconnect();
	printf("已经执行完Disconnect");

	return 0;
}