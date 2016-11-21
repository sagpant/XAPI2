// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.3.0.20151216"
#define API_NAME	"CTP"
#define DLL_PUBLIC_KEY "30819D300D06092A864886F70D010101050003818B0030818702818100A5411C1BAC072723B8EC84F609B88DF913308C6B38D2F304B4285AA290686888D05D731793AD30342E8917ACC57AF8DA4976877AC1B701A664EDEB668B14027F5ED4A96A61DA00AB0208E3E7DD4C8249E88B7A3BA511C7C082979B23AA1876ED2B7A77BB1A405D15B164E586431CD3ECFB3E448A193A41BA21C34A659196C353020111"

#include "../../include/CTP/ThostFtdcUserApiDataType.h"
#include "../../include/CTP/ThostFtdcUserApiStruct.h"
#include "../../include/CTP/ThostFtdcTraderApi.h"

#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP/win64/thosttraderapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x64d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x64.lib")
	#endif
#else
	#pragma comment(lib, "../../include/CTP/win32/thosttraderapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x86d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x86.lib")
	#endif
#endif

// CTP有多个版本有些没有ExchangeID，而有些有
//#define HAS_ExchangeID	1
#undef HAS_ExchangeID
// 飞鼠的Quote中没有交易所
#define HAS_ExchangeID_Quote	1