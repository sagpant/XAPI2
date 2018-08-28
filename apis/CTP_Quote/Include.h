// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.3.0.20151216"
#define API_NAME	"CTP"
#define DLL_PUBLIC_KEY ""

#include "../../include/CrossPlatform.h"
#include "../../include/CTP/ThostFtdcUserApiDataType.h"
#include "../../include/CTP/ThostFtdcUserApiStruct.h"
#include "../../include/CTP/ThostFtdcMdApi.h"

#ifndef USE_CMAKE
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP/win64/thostmduserapi.lib")
#else
	#pragma comment(lib, "../../include/CTP/win32/thostmduserapi.lib")
#endif
#endif


// Ìõ¼þ±àÒë
#include "../CTP/include_pub.h"
