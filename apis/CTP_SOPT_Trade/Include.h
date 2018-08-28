// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20161126"
#define API_NAME	"CTP_SOPT"
#define DLL_PUBLIC_KEY "0"

#include "../../include/CrossPlatform.h"
#include "../../include/CTP_SOPT/ThostFtdcUserApiDataType.h"
#include "../../include/CTP_SOPT/ThostFtdcUserApiStruct.h"
#include "../../include/CTP_SOPT/ThostFtdcTraderApi.h"

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP_SOPT/win64/thosttraderapi.lib")
#else
	#pragma comment(lib, "../../include/CTP_SOPT/win32/thosttraderapi.lib")
#endif
#endif


// 条件编译
#include "../CTP/include_pub.h"

#undef USE_LONG_ORDER_REF
