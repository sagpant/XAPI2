// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define API_VERSION	"0.3.0.20151216"
#define API_NAME	"Kingstar"

#include "../include/Kingstar/IncEx/KSUserApiDataTypeEx.h"
#include "../include/Kingstar/IncEx/KSUserApiStructEx.h"
#include "../include/Kingstar/inc/KSMarketDataAPI.h"

using namespace KingstarAPI;

#ifdef _WIN64
#pragma comment(lib, "../include/Kingstar/win64/KSMarketDataAPI.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x64.lib")
#else
#pragma comment(lib, "../include/Kingstar/win32/KSMarketDataAPI.lib")
#pragma comment(lib, "../lib/QuantBox_Queue_x86.lib")
#endif