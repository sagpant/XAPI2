// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20120722"
#define API_NAME	"ATP"
#define DLL_PUBLIC_KEY ""

#include "../../include/ATP/ThostFtdcUserApiDataType.h"
#include "../../include/ATP/ThostFtdcUserApiStruct.h"
#include "../../include/ATP/ThostFtdcMdApi.h"

#ifdef _WIN64
	#pragma comment(lib, "../../include/ATP/win64/thostmduserapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x64d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x64.lib")
	#endif
#else
	#pragma comment(lib, "../../include/ATP/win32/thostmduserapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x86d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x86.lib")
	#endif
#endif

// Ìõ¼þ±àÒë
#include "../CTP/include_pub.h"