// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.3.0.20151216"
#define API_NAME	"CTP"
#define DLL_PUBLIC_KEY ""

#include "../../include/CTP/ThostFtdcUserApiDataType.h"
#include "../../include/CTP/ThostFtdcUserApiStruct.h"
#include "../../include/CTP/ThostFtdcMdApi.h"

#ifdef USE_CMAKE
	#if _WIN32
		#ifdef _WIN64
			#pragma comment(lib, "../include/CTP/win64/thostmduserapi.lib")
		#else
			#pragma comment(lib, "../include/CTP/win32/thostmduserapi.lib")
		#endif
	#endif
#else
#ifdef _WIN64
	#pragma comment(lib, "../../include/CTP/win64/thostmduserapi.lib")
#else
	#pragma comment(lib, "../../include/CTP/win32/thostmduserapi.lib")
#endif
#endif


// Ìõ¼þ±àÒë
#include "../CTP/include_pub.h"