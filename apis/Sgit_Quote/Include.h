// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.1.0.20161121"
#define API_NAME	"Sgit"
#define DLL_PUBLIC_KEY ""


#include "../../include/Sgit/SgitFtdcUserApiDataType.h"
#include "../../include/Sgit/SgitFtdcUserApiStruct.h"
#include "../../include/Sgit/SgitFtdcMdApi.h"

using namespace fstech;

#ifdef _WIN64
	#pragma comment(lib, "../../include/Sgit/win64/sgitquotapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x64d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x64.lib")
	#endif
#else
	#pragma comment(lib, "../../include/Sgit/win32/sgitquotapi.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x86d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x86.lib")
	#endif
#endif

// Ìõ¼þ±àÒë
#include "../CTP/include_pub.h"