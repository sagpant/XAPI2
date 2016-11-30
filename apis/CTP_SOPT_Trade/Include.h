// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20161126"
#define API_NAME	"CTP_SOPT"
#define DLL_PUBLIC_KEY "0"

#include "../../include/CTP_SOPT/ThostFtdcUserApiDataType.h"
#include "../../include/CTP_SOPT/ThostFtdcUserApiStruct.h"
#include "../../include/CTP_SOPT/ThostFtdcTraderApi.h"

#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP_SOPT/win64/thosttraderapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x64d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x64.lib")
	#endif
#else
	#pragma comment(lib, "../../include/CTP_SOPT/win32/thosttraderapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x86d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x86.lib")
	#endif
#endif

// CTP有多个版本有些没有ExchangeID，而有些有
#define HAS_ExchangeID	1
// 飞鼠的Quote中没有交易所
#undef HAS_ExchangeID_Quote