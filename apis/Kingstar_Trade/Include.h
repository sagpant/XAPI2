// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.3.0.20151216"
#define API_NAME	"Kingstar"
#define DLL_PUBLIC_KEY ""

#include "../../include/Kingstar/IncEx/KSUserApiDataTypeEx.h"
#include "../../include/Kingstar/IncEx/KSUserApiStructEx.h"
#include "../../include/Kingstar/inc/KSTradeAPI.h"

using namespace KingstarAPI;

#ifdef _WIN64
	#pragma comment(lib, "../../include/Kingstar/win64/KSTradeAPI.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x64d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x64.lib")
	#endif
#else
	#pragma comment(lib, "../../include/Kingstar/win32/KSTradeAPI.lib")
	#ifdef _DEBUG
	#pragma comment(lib, "../../lib/Queue_x86d.lib")
	#else
	#pragma comment(lib, "../../lib/Queue_x86.lib")
	#endif
#endif

// 为了解决金仕达授权问题而加的功能
#define KS_LKC_FILENAME	"KSInterB2C"
#define KS_LKC_EXT	"lkc"

#define KS_COPYFILE 1

// CTP有多个版本有些没有ExchangeID，而有些有
#define HAS_ExchangeID	1